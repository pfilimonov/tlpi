/*
  This exercise is designed to demonstrate why the atomicity guaranteed by
opening a file with the O_APPEND flag is necessary. Write a program that takes
up to three command-line arguments:

$ atomic_append filename num-bytes [x]

This program should open the specified filename (creating it if necessary) and
append num-bytes bytes to the file by using write() to write a byte at a time.
By default, the program should open the file with the O_APPEND flag, but if a
third command-line argument (x) is supplied, then the O_APPEND flag should be
omitted, and instead the program should perform an lseek(fd, 0, SEEK_END) call
before each write(). Run two instances of this program at the same time without
the x argument to write 1 million bytes to the same file:

$ atomic_append f1 1000000 & atomic_append f1 1000000

Repeat the same steps, writing to a different file, but this time specifying the
x argument:

$ atomic_append f2 1000000 x & atomic_append f2 1000000 x

List the sizes of the files f1 and f2 using ls –l and explain the difference.
  */

#include "tlpi_hdr.h"
#include <fcntl.h>

int main(int argc, char *argv[]) {
  if (argc < 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s filename num-bytes [x]\n", argv[0]);
  }

  Boolean use_o_append = argc < 4 ? TRUE : FALSE;

  int fd =
      open(argv[1], O_CREAT | O_WRONLY | (use_o_append == TRUE ? O_APPEND : 0),
           S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (fd == -1) {
    errExit("open");
  }

  ssize_t num_bytes = atol(argv[2]);

  // O_APPEND guarantees that write and offset move happend atomically inside a
  // single call while without it lseek and write are interleaved randomly
  // since each process has its own descripor and open file description entry
  // (and hence different offsets)
  for (ssize_t i = 0; i < num_bytes; i++) {
    if (use_o_append == FALSE) {
      lseek(fd, 0, SEEK_END);
    }
    if (write(fd, "b", 1) != 1) {
      errExit("write");
    }
  }

  exit(EXIT_SUCCESS);
}
