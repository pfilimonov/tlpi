#include "tlpi_hdr.h"
#include <fcntl.h>

/*
The tee command reads its standard input until end-of-file, writing a copy of
the input to standard output and to the file named in its command-line argument.
(We show an example of the use of this command when we discuss FIFOs in
Section 44.7.) Implement tee using I/O system calls. By default, tee overwrites
any existing file with the given name. Implement the –a command-line option (tee
–a file), which causes tee to append text to the end of a file if it already
exists. (Refer to Appendix B for a description of the getopt() function, which
can be used to parse command-line options.)
*/

#define MAX_READ 100

int main(int argc, char *argv[]) {
  Boolean append = FALSE;
  char *filename;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s file [-a]\n", argv[0]);
  }

  int opt;
  while ((opt = getopt(argc, argv, "a")) != -1) {
    switch (opt) {
    case 'a':
      append = TRUE;
      break;
    default:
      usageErr("%s file [-a]\n", argv[0]);
    }
  }

  if (optind >= argc) {
    usageErr("%s file [-a]\n", argv[0]);
  }
  filename = argv[optind];

  int oflags = O_WRONLY | O_CREAT;
  if (append == TRUE) {
    oflags = oflags | O_APPEND;
  } else {
    oflags = oflags | O_TRUNC;
  }

  int fd = open(filename, oflags,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (fd == -1) {
    errExit("open");
  }

  int nread;
  char buf[MAX_READ];
  while ((nread = read(STDIN_FILENO, buf, MAX_READ)) > 0) {
    if (write(STDOUT_FILENO, buf, nread) != nread) {
      errExit("write stdout");
    }
    if (write(fd, buf, nread) != nread) {
      errExit("write file");
    }
  }

  if (nread == -1) {
    errExit("read");
  }

  if (close(fd) == -1) {
    errExit("close");
  }

  exit(EXIT_SUCCESS);
}
