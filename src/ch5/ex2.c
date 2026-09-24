/*
 * Write a program that opens an existing file for writing with the O_APPEND
 * flag, and then seeks to the beginning of the file before writing some data.
 * Where does the data appear in the file? Why?
 */

#include "tlpi_hdr.h"
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s filename\n", argv[0]);
  }

  int fd = open(argv[1], O_APPEND | O_WRONLY);
  if (fd == -1) {
    errExit("open");
  }

  if (lseek(fd, 0, SEEK_SET) == -1) {
    errExit("lseek");
  }

  // O_APPEND flag moves offset to the end of the file before writing (does
  // this atomically)
  if (write(fd, "gavrik", 6) != 6) {
    errExit("write");
  }

  exit(EXIT_SUCCESS);
}
