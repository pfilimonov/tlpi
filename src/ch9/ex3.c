/*
 * Implement initgroups() using setgroups() and library functions for retrieving
 * information from the password and group files (Section 8.4). Remember that a
 * process must be privileged in order to be able to call setgroups().
 */

#define _DEFAULT_SOURCE
#define _GNU_SOURCE
#include <grp.h>
#include <limits.h>
#include <sys/fsuid.h>
#include <unistd.h>

#include "check.h"
#include "tlpi_hdr.h"

#define SG_SIZE (NGROUPS_MAX + 1)

#define TEST_USER "peter"
#define TEST_GROUP 982

int initgroups_(const char *user, gid_t group);

int main(void) {
  if (initgroups_(TEST_USER, TEST_GROUP) == -1) {
    errExit("initgroups_");
  }
  exit(EXIT_SUCCESS);
}

int initgroups_(const char *user, gid_t group) {
  struct group *grp;
  gid_t suppGroups[SG_SIZE];
  size_t index = 0;

  setgrent();
  while ((grp = getgrent()) != NULL) {
    for (char **member = grp->gr_mem; *member != NULL; member++) {
      if (strcmp(*member, user) == 0) {
        if (index >= SG_SIZE) {
          endgrent();
          return -1;
        }
        suppGroups[index++] = grp->gr_gid;
        break;
      }
    }
  }
  endgrent();

  if (index >= SG_SIZE) {
    return -1;
  }

  suppGroups[index++] = group;

  if (setgroups(index, suppGroups) == -1) {
    return -1;
  }

  return 0;
}
