// Write a program to verify that duplicated file descriptors share a file
// offset value and open file status flags.

#include "error_functions.h"
#include "tlpi_hdr.h"
#include <fcntl.h>

#define CHECK(cond)                                                            \
  do {                                                                         \
    if (!(cond)) {                                                             \
      fatal("check failed: %s", #cond);                                        \
    }                                                                          \
  } while (0)

static void test(int fd1, int fd2) {
  off_t o1, o2;
  o1 = lseek(fd1, 0, SEEK_CUR);
  o2 = lseek(fd2, 0, SEEK_CUR);
  if (o1 == -1 || o2 == -1) {
    errExit("lseek");
  }
  CHECK(o1 == o2);
  int flags1, flags2;
  flags1 = fcntl(fd1, F_GETFL);
  flags2 = fcntl(fd2, F_GETFL);
  if (flags1 == -1 || flags2 == -1) {
    errExit("fcntl (get flags)");
  }
  CHECK(flags1 == flags2);

  // change offset
  char buf;
  ssize_t nread;
  if ((nread = read(fd1, &buf, 1)) == -1) {
    errExit("read");
  }
  CHECK(nread != 0);

  o1 = lseek(fd1, 0, SEEK_CUR);
  o2 = lseek(fd2, 0, SEEK_CUR);
  if (o1 == -1 || o2 == -1) {
    errExit("lseek");
  }
  CHECK(o1 == o2);

  // change flags
  CHECK((flags1 & O_APPEND) == 0);
  if (fcntl(fd1, F_SETFL, flags1 | O_APPEND) == -1) {
    errExit("fcntl (set flags)");
  }
  flags2 = fcntl(fd2, F_GETFL);
  if (flags2 == -1) {
    errExit("fcntl (get flags)");
  }
  CHECK((flags1 | O_APPEND) == flags2);

  // restore flags
  if (fcntl(fd1, F_SETFL, flags1) == -1) {
    errExit("fcntl (set flags)");
  }
}

int main(int argc, char *argv[]) {
  int req_fd = 10;
  if (argc > 1) {
    req_fd = getInt(argv[1], 0, "required fd");
    if (req_fd < 3) {
      cmdLineErr("fd must be >= 3\n");
    }
  }

  char template[] = "/tmp/dupXXXXXX";
  int fd1 = mkstemp(template);
  if (fd1 == -1) {
    errExit("mkstemp");
  }
  unlink(template);

  if (req_fd == fd1) {
    cmdLineErr("choose another fd\n");
  }

  if (write(fd1, "teststr", 7) != 7) {
    errExit("write");
  }
  if (lseek(fd1, 0, SEEK_SET) == -1) {
    errExit("lseek");
  }

  // dup1
  int fd2 = dup(fd1);
  if (fd2 == -1) {
    errExit("dup");
  }
  test(fd1, fd2);
  if (close(fd2) == -1) {
    errExit("close fd2");
  }

  // dup2
  fd2 = dup2(fd1, req_fd);
  if (fd2 == -1) {
    errExit("dup2");
  }
  CHECK(fd2 == req_fd);
  test(fd1, fd2);
  if (close(fd2) == -1) {
    errExit("close fd2");
  }

  // cleanup
  if (close(fd1) == -1) {
    errExit("close fd1");
  }

  exit(EXIT_SUCCESS);
}
