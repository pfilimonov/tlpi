/*
 * Write a program that draws a tree showing the hierarchical parent-child
 * relationships of all processes on the system, going all the way back to init.
 * For each process, the program should display the process ID and the command
 * being executed. The output of the program should be similar to that produced
 * by pstree(1), although it does need not to be as sophisticated. The parent of
 * each process on the system can be found by inspecting the PPid: line of all
 * of the /proc/PID/status files on the system. Be careful to handle the
 * possibility that a process’s parent (and thus its /proc/PID directory)
 * disappears during the scan of all /proc/PID directories.
 */

#include "tlpi_hdr.h"
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>

#define BUF_SIZE 4096

typedef struct proc_info {
  char *name;
  pid_t pid;
  struct proc_info **children;
  size_t num_children;
} proc_info_t;

static long MAX_PATH_LEN;

static proc_info_t *init_proc = NULL;

proc_info_t *build_proc_info(char *name, pid_t pid) {
  proc_info_t *pi = malloc(sizeof(proc_info_t));
  pi->name = malloc(strlen(name) + 1);
  strcpy(pi->name, name);
  pi->pid = pid;
  pi->children = NULL;
  pi->num_children = 0;
  return pi;
}

proc_info_t *get_child(proc_info_t *parent, pid_t pid) {
  if (parent == NULL) {
    return NULL;
  }
  for (size_t i = 0; i < parent->num_children; i++) {
    if (parent->children[i]->pid == pid) {
      return parent->children[i];
    }
    proc_info_t *child = get_child(parent->children[i], pid);
    if (child != NULL) {
      return child;
    }
  }

  return NULL;
}

void add_child(proc_info_t *parent, proc_info_t *child) {
  proc_info_t **temp = realloc(parent->children, (++(parent->num_children)) *
                                                     sizeof(proc_info_t *));
  if (temp == NULL) {
    errExit("realloc");
  }

  temp[parent->num_children - 1] = child;
  parent->children = temp;
}

void free_proc_info(proc_info_t *pi) {
  free(pi->name);
  for (size_t i = 0; i < pi->num_children; i++) {
    free_proc_info(pi->children[i]);
  }

  free(pi->children);
  free(pi);
}

proc_info_t *get_proc_info(pid_t pid) {
  if (pid <= 0) {
    fprintf(stderr, "[ERROR] Invalid pid=%d\n", pid);
    return NULL;
  }
  if (pid == 1 && init_proc != NULL) {
    return init_proc;
  }
  if (init_proc != NULL) {
    proc_info_t *child = get_child(init_proc, pid);
    if (child != NULL) {
      // already processed
      return child;
    }
  }

  char filename[MAX_PATH_LEN];

  snprintf(filename, MAX_PATH_LEN, "/proc/%d/status", pid);
  // printf("[DEBUG] status file name: %s\n", filename);
  FILE *f = fopen(filename, "r");
  if (f == NULL) {
    if (errno == ENOENT) {
      // printf("[DEBUG] %s does not exist\n", filename);
      return NULL;
    } else {
      errExit("open");
    }
  }

  char buf[BUF_SIZE];
  char name[BUF_SIZE];
  pid_t ppid = -1;
  name[0] = '\0';
  while ((fgets(buf, BUF_SIZE, f) != NULL)) {
    // printf("[DEBUG] buf: %s\n", buf);
    if (strncmp("Name:\t", buf, 6) == 0) {
      // printf("[DEBUG] name: %s\n", name);
      strcpy(name, &buf[6]);
      name[strlen(name) - 1] = '\0';
      continue;
    }
    if (strncmp("PPid:\t", buf, 6) == 0) {
      // printf("[DEBUG] uid: %d\n", uid_cand);
      if (sscanf(buf, "PPid:\t%d\n", &ppid) != 1) {
        continue;
      }
    }
    if (strlen(name) > 0 && ppid != -1) {
      break;
    }
  }

  if (ferror(f)) {
    if (errno != ESRCH && errno != ENOENT) {
      errExit("fgets");
    } else {
      fclose(f);
      return NULL;
    }
  }

  if (ppid == -1) {
    errExit("Didn't find PPid in %s\n", filename);
  }

  if (strlen(name) == 0) {
    errExit("Didn't find Name in %s\n", filename);
  }

  fclose(f);

  if (ppid != 0) {
    proc_info_t *pi = build_proc_info(name, pid);
    proc_info_t *parent = get_proc_info(ppid);
    if (parent != NULL) {
      add_child(parent, pi);
      return pi;
    } else {
      free_proc_info(pi);
      return NULL;
    }
  } else if (init_proc == NULL) {
    return build_proc_info(name, pid);
  }

  return NULL;
}

int print_proc_info(proc_info_t *pi, int indent) {
  int idt = indent;
  while (idt > 0) {
    printf("  ");
    idt--;
  }
  printf("pid=%d, name=%s\n", pi->pid, pi->name);
  int proc_cnt = 1;
  for (size_t i = 0; i < pi->num_children; i++) {
    proc_cnt += print_proc_info(pi->children[i], indent + 2);
  }

  return proc_cnt;
}

int main(void) {
  MAX_PATH_LEN = pathconf("/proc", _PC_PATH_MAX);
  if (MAX_PATH_LEN == -1) {
    errExit("Can't get max path length");
  }

  init_proc = get_proc_info(1);
  if (init_proc == NULL) {
    fatal("Failed to get init process info");
  }

  // scan /proc
  DIR *dirp = opendir("/proc");
  if (dirp == NULL) {
    errExit("opendir");
  }

  errno = 0;
  struct dirent *direntp;
  while (1) {
    errno = 0;
    if ((direntp = readdir(dirp)) == NULL) {
      break;
    }

    pid_t pid = atoi(direntp->d_name);
    if (pid == 0) {
      continue; // not an process id
    }

    get_proc_info(pid);
  }

  if (errno != 0) {
    errExit("readdir");
  } else {
    // reached the end of directory
  }

  if (closedir(dirp) == -1) {
    errExit("closedir");
  }

  printf("Number of processes: %d\n", print_proc_info(init_proc, 0));

  free_proc_info(init_proc);
}
