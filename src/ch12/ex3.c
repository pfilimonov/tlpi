/*
 * Write a program that lists all processes that have a particular file pathname
 * open. This can be achieved by inspecting the contents of all of the
 * /proc/PID/fd/* symbolic links. This will require nested loops employing
 * readdir(3) to scan all /proc/PID directories, and then the contents of all
 * /proc/PID/fd entries within each /proc/PID directory. To read the contents of
 * a /proc/PID/fd/n symbolic link requires the use of readlink(), described in
 * Section 18.5.
 */

#include "error_functions.h"
#include "tlpi_hdr.h"

#include <dirent.h>
#include <errno.h>

static long MAX_PATH_LEN;

int main(int argc, char *argv[]) {
  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s filename\n", argv[0]);
  }

  MAX_PATH_LEN = pathconf("/proc", _PC_PATH_MAX);
  if (MAX_PATH_LEN == -1) {
    errExit("Can't get max path length");
  }

  char path[MAX_PATH_LEN];
  if (realpath(argv[1], path) == NULL) {
    errExit("invalid path %s", argv[1]);
  }

  // scan /proc
  DIR *dirp = opendir("/proc");
  if (dirp == NULL) {
    errExit("opendir");
  }

  struct dirent *direntp;
  while (1) {
    errno = 0;
    if ((direntp = readdir(dirp)) == NULL) {
      break;
    }

    char *invalid = NULL;

    pid_t pid = strtol(direntp->d_name, &invalid, 10);
    if (invalid[0] != '\0' || errno == EINVAL || pid <= 0) {
      continue; // not an process id
    }

    char fddirname[MAX_PATH_LEN];

    snprintf(fddirname, MAX_PATH_LEN, "/proc/%d/fd/", pid);

    DIR *fddirp = opendir(fddirname);
    if (fddirp == NULL) {
      if (errno == EMFILE) {
        errExit("opendir");
      }
      continue;
    }

    struct dirent *fddirentp;
    while (1) {
      errno = 0;
      if ((fddirentp = readdir(fddirp)) == NULL) {
        if (errno == ENOENT) {
          errno = 0;
        }
        break;
      }

      errno = 0;
      invalid = NULL;
      int fd = strtol(fddirentp->d_name, &invalid, 10);
      if (invalid[0] != '\0' || errno != 0) {
        continue;
      }

      char buf[MAX_PATH_LEN + 1];
      char fdfilename[MAX_PATH_LEN];
      snprintf(fdfilename, MAX_PATH_LEN, "/proc/%d/fd/%d", pid, fd);

      ssize_t len;
      if ((len = readlink(fdfilename, buf, MAX_PATH_LEN)) == -1) {
        if (errno != EPERM && errno != EACCES && errno != ENOENT) {
          errExit("readlink");
        } else {
          continue;
        }
      }
      buf[len] = '\0';

      if (strcmp(buf, path) == 0) {
        // printf("[DEBUG] fdfilename: %s\n", fdfilename);
        // printf("[DEBUG] len: %zu,buf: %s\n", len, buf);
        printf("%d\n", pid);
        errno = 0;
        break;
      }
    }

    if (errno != 0) {
      errExit("readdir");
    } else {
      // reached the end of directory
    }

    if (closedir(fddirp) == -1) {
      errExit("closedir");
    }
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
