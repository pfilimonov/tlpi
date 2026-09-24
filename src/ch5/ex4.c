/*
Implement dup() and dup2() using fcntl() and, where necessary, close(). (You may
ignore the fact that dup2() and fcntl() return different errno values for some
error cases.) For dup2(), remember to handle the special case where oldfd equals
newfd. In this case, you should check whether oldfd is valid, which can be done
by, for example, checking if fcntl(oldfd, F_GETFL) succeeds. If oldfd is not
valid, then the function should return –1 with errno set to EBADF.
*/

#include "tlpi_hdr.h"
#include <assert.h>
#include <fcntl.h>

int dup_(int oldfd);
int dup2_(int oldfd, int newfd);

int main(int argc, char *argv[]) {
  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s fd1 [fd2]\n", argv[0]);
  }

  int fd1 = atol(argv[1]);
  int fd2 = -1;
  if (argc == 3) {
    fd2 = atol(argv[2]);
  }

  int fd;
  if (argc == 2) {
    fd = dup_(fd1);
  } else {
    fd = dup2_(fd1, fd2);
  }

  printf("fd=%d\n", fd);

  if (fd == -1) {
    errExit("custom dup");
  }

  // test
  if (argc == 2) {
    assert(fd >= 3);
  }
  if (argc == 3) {
    assert(fd == fd2);
  }

  exit(EXIT_SUCCESS);
}

int dup_(int oldfd) { return fcntl(oldfd, F_DUPFD, 0); }

int dup2_(int oldfd, int newfd) {
  if (fcntl(oldfd, F_GETFL) == -1) {
    errno = EBADF;
    return -1;
  }

  if (oldfd == newfd) {
    return newfd;
  }

  close(newfd); // ok to fail

  return fcntl(oldfd, F_DUPFD, newfd);
}
