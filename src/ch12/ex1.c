/*
 *  Write a program that lists the process ID and command name for all processes
 * being run by the user named in the program’s command-line argument. (You may
 * find the userIdFromName() function from Listing 8-1, on page 159, useful.)
 * This can be done by inspecting the Name: and Uid: lines of all of the
 * /proc/PID/status files on the system. Walking through all of the /proc/PID
 * directories on the system requires the use of readdir(3), which is described
 * in Section 18.8. Make sure your program correctly handles the possibility
 * that a /proc/PID directory disappears between the time that the program
 * determines that the directory exists and the time that it tries to open the
 * corresponding /proc/PID/status file.
 */

#include "tlpi_hdr.h"
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>

#define BUF_SIZE 4096

uid_t /* Return UID corresponding to 'name', or -1 on error */
userIdFromName(const char *name) {
  struct passwd *pwd;
  uid_t u;
  char *endptr;

  if (name == NULL || *name == '\0') /* On NULL or empty string */
    return -1;                       /* return an error */

  u = strtol(name, &endptr, 10); /* As a convenience to caller */
  if (*endptr == '\0')           /* allow a numeric string */
    return u;

  pwd = getpwnam(name);
  if (pwd == NULL)
    return -1;

  return pwd->pw_uid;
}

int main(int argc, char *argv[]) {
  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s username\n", argv[0]);
  }

  uid_t uid = userIdFromName(argv[1]);
  if (uid == -1) {
    errExit("userIdFromName");
  }

  DIR *dirp = opendir("/proc");
  if (dirp == NULL) {
    errExit("opendir");
  }

  long max_len = pathconf("/proc", _PC_PATH_MAX);
  if (max_len == -1) {
    errExit("Can't get max path length");
  }

  errno = 0;
  struct dirent *direntp;
  while (1) {
    errno = 0;
    if ((direntp = readdir(dirp)) == NULL) {
      break;
    }

    if (atoi(direntp->d_name) == 0) {
      continue; // not an process id
    }

    // printf("[DEBUG] dirent name: %s\n", direntp->d_name);

    char filename[max_len];

    snprintf(filename, max_len, "/proc/%s/status", direntp->d_name);
    // printf("[DEBUG] status file name: %s\n", filename);
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
      if (errno == ENOENT) {
        // printf("[DEBUG] %s does not exist\n", filename);
        continue;
      } else {
        errExit("open");
      }
    }

    char buf[BUF_SIZE];
    uid_t ruid, euid, suid, fuid;
    char name[BUF_SIZE];
    name[0] = '\0';
    Boolean found = FALSE;
    while ((fgets(buf, BUF_SIZE, f) != NULL)) {
      // printf("[DEBUG] buf: %s\n", buf);
      if (strncmp("Name:\t", buf, 6) == 0) {
        // printf("[DEBUG] name: %s\n", name);
        strcpy(name, &buf[6]);
        name[strlen(name) - 1] = '\0';
        continue;
      }
      if (strncmp("Uid:\t", buf, 5) == 0) {
        // printf("[DEBUG] uid: %d\n", uid_cand);
        if (sscanf(buf, "Uid:\t%u\t%u\t%u\t%u\n", &ruid, &euid, &suid, &fuid) !=
            4) {
          continue;
        }
        if (ruid != uid) {
          break;
        } else {
          found = TRUE;
        }
      }
    }

    if (ferror(f)) {
      if (errno != ESRCH && errno != ENOENT) {
        errExit("fgets");
      }
    }

    if (!found) {
      fclose(f);
      continue;
    }

    if (strlen(name) > 0) {
      printf("[INFO] process name=%s pid=%s\n", name, direntp->d_name);
    }
    fclose(f);
  }

  if (errno != 0) {
    errExit("readdir");
  } else {
    // reached the end of directory
  }

  if (closedir(dirp) == -1) {
    errExit("closedir");
  }
}
