// Implement readv() and writev() using read(), write(), and suitable functions
// from the malloc package (Section 7.1.2).

#include "tlpi_hdr.h"
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

static long iov_max_ = 1024;

ssize_t readv_(int fd, const struct iovec *iov, int iovcnt);
ssize_t writev_(int fd, const struct iovec *iov, int iovcnt);

void test(Boolean test_readv, // true if testing readv, false if testing writev
          Boolean partial_read);

int main(void) {
  long sys_conf = sysconf(_SC_IOV_MAX);
  if (sys_conf != -1) {
    iov_max_ = sys_conf;
  }

  test(TRUE, FALSE);
  test(TRUE, TRUE);
  test(FALSE, FALSE);
  exit(EXIT_SUCCESS);
}

ssize_t readv_(int fd, const struct iovec *iov, int iovcnt) {
  if (iovcnt <= 0 || iovcnt > iov_max_) {
    errno = EINVAL;
    return -1;
  }

  ssize_t total_size = 0;
  for (int i = 0; i < iovcnt; i++) {
    total_size += iov[i].iov_len;
  }

  if (total_size == 0) {
    return 0;
  }

  char *buf = malloc(total_size);
  if (buf == NULL) {
    errno = ENOMEM;
    return -1;
  }
  ssize_t nread = read(fd, buf, total_size);
  int errno_save = errno;
  if (nread == -1) {
    free(buf);
    errno = errno_save;
    return -1;
  }

  ssize_t offset = 0;
  for (int i = 0; i < iovcnt; i++) {
    if (offset >= nread) {
      break;
    }
    memcpy(iov[i].iov_base, buf + offset,
           min(iov[i].iov_len,
               (size_t)(nread - offset))); // safe cast because of earlier check
    offset += iov[i].iov_len;
  }

  free(buf);

  return nread;
}

ssize_t writev_(int fd, const struct iovec *iov, int iovcnt) {
  if (iovcnt <= 0 || iovcnt > iov_max_) {
    errno = EINVAL;
    return -1;
  }

  ssize_t total_size = 0;
  for (int i = 0; i < iovcnt; i++) {
    total_size += iov[i].iov_len;
  }

  if (total_size == 0) {
    return 0;
  }

  char *buf = malloc(total_size);
  if (buf == NULL) {
    errno = ENOMEM;
    return -1;
  }

  ssize_t offset = 0;
  for (int i = 0; i < iovcnt; i++) {
    memcpy(buf + offset, iov[i].iov_base, iov[i].iov_len);
    offset += iov[i].iov_len;
  }

  ssize_t nwrt = write(fd, buf, total_size);

  free(buf);

  return nwrt;
}

struct TestStruct {
  int a;
  int b;
  long *c;
};

void test(Boolean test_readv, Boolean partial_read) {
  if (test_readv == TRUE) {
    printf("Testing readv\n");
  } else {
    if (partial_read == TRUE) {
      return; // no test for partial read when testing writev
    }
    printf("Testing writev\n");
  }

  struct iovec iov[3], iov_test[3];
  struct TestStruct myStruct, myStruct_test; /* First buffer */
  myStruct.a = 10;
  myStruct.b = 100;
  myStruct.c = &iov_max_;

  int x = 4; /* Second buffer */
  int x_test;
  char *str = "teststring"; /* Third buffer */
  char str_test[11];
  ssize_t numRead, numWrite, totRequired;

  char template[] = "/tmp/testXXXXXX";

  int fd = mkstemp(template);

  if (fd == -1) {
    errExit("mkstemp");
  }

  // printf("tmp file: %s\n", template);

  unlink(template);

  totRequired = 0;

  iov[0].iov_base = &myStruct;
  iov[0].iov_len = sizeof(myStruct);
  iov_test[0].iov_base = &myStruct_test;
  iov_test[0].iov_len = sizeof(myStruct);
  totRequired += iov[0].iov_len;

  iov[1].iov_base = &x;
  iov[1].iov_len = sizeof(x);
  iov_test[1].iov_base = &x_test;
  iov_test[1].iov_len = sizeof(x_test);
  totRequired += iov[1].iov_len;

  iov[2].iov_base = str;
  iov[2].iov_len = strlen(str) + 1;
  iov_test[2].iov_base = str_test;
  iov_test[2].iov_len = strlen(str) + 1;
  if (partial_read == FALSE) {
    totRequired += iov[2].iov_len;
  }

  if (test_readv == TRUE) {
    if (partial_read == FALSE) {
      numWrite = writev(fd, iov, 3);
    } else {
      numWrite = writev(fd, iov, 2);
    }
  } else {
    numWrite = writev_(fd, iov, 3);
    printf("Written %zd bytes\n", numWrite);
  }
  if (numWrite == -1) {
    errExit("writev");
  }

  if (numWrite < totRequired) {
    fatal("Wrote fewer bytes (%zd) than requested (%zd)\n", numWrite,
          totRequired);
  }

  // rewind
  if (lseek(fd, 0, SEEK_SET) == -1) {
    errExit("lseek");
  }

  if (test_readv == TRUE) {
    numRead = readv_(fd, iov_test, 3);
    printf("Read %zd bytes\n", numRead);
  } else {
    numRead = readv(fd, iov_test, 3);
  }
  if (numRead == -1)
    errExit("readv");

  if (numRead != totRequired) {
    fatal("Read another amount of bytes (%zd) than requested (%zd)\n", numRead,
          totRequired);
  }

  if (myStruct_test.a != myStruct.a) {
    fatal("myStruct.a != myStruct_test.a \n");
  }
  if (myStruct_test.b != myStruct.b) {
    fatal("myStruct.b != myStruct_test.b\n");
  }
  if (myStruct_test.c != myStruct.c) {
    fatal("myStruct.c != myStruct_test.c\n");
  }
  if (x_test != x) {
    fatal("x != x_test\n");
  }
  if (partial_read == FALSE && strcmp(str, str_test) != 0) {
    fatal("strcmp(str, str_test) != 0\n");
  }

  if (close(fd) == -1) {
    errExit("close");
  }
}
