#include "error_functions.h"
#include "tlpi_hdr.h"
#include <fcntl.h>
#include <stdio.h>

/*
 *
 * Write a program like cp that, when used to copy a regular file that contains
 * holes (sequences of null bytes), also creates corresponding holes in the
 * target file.
 *
 */

#define BUFSIZE 4096

static Boolean zero_block(char *buf, ssize_t size) {
  for (ssize_t i = 0; i < size; i++) {
    if (buf[i] != 0) {
      return FALSE;
    }
  }
  return TRUE;
}

int main(int argc, char *argv[]) {

  if (argc < 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s source-file destination-file\n", argv[0]);
  }

  char *src = argv[1];
  char *dst = argv[2];

  int fdsrc = open(src, O_RDONLY);
  if (fdsrc == -1) {
    errExit("open src");
  }
  int fddst = open(dst, O_WRONLY | O_CREAT | O_TRUNC,
                   S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (fddst == -1) {
    errExit("open dst");
  }

  ssize_t nread;
  char buf[BUFSIZE];
  while ((nread = read(fdsrc, buf, BUFSIZE)) > 0) {
    if (zero_block(buf, nread) == TRUE) {
      if (lseek(fddst, nread, SEEK_CUR) == -1) {
        errExit("lseek");
      }
    } else {
      if (write(fddst, buf, nread) != nread) {
        errExit("write dst");
      }
    }
  }

  if (nread == -1) {
    errExit("read src");
  }

  off_t end = lseek(fddst, 0, SEEK_CUR);
  if (end == -1) {
    errExit("lseek");
  }
  if (ftruncate(fddst, end) == -1) {
    errExit("ftruncate");
  }

  if (close(fdsrc) == -1) {
    errExit("close src");
  }

  if (close(fddst) == -1) {
    errExit("close dst");
  }

  exit(EXIT_SUCCESS);
}
