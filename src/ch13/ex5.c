/*
 * The command tail [ –n num ] file prints the last num lines (ten by default)
 * of the named file. Implement this command using I/O system calls (lseek(),
 * read(), write(), and so on). Keep in mind the buffering issues described in
 * this chapter, in order to make the implementation efficient.
 */

#include "tlpi_hdr.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>

void tail(int n, char *filename);

int main(int argc, char *argv[]) {

  char *filename;
  int n = 10;
  int opt;

  while ((opt = getopt(argc, argv, ":n:")) != -1) {
    switch (opt) {
    case 'n':
      n = strtol(optarg, NULL, 10);
      break;
    default:
      continue;
    }
  }

  filename = argv[optind];
  if (filename == NULL) {
    usageErr("%s [-n num] filename\n", argv[0]);
  }

  tail(n, filename);

  exit(EXIT_SUCCESS);
}

static off_t find_start_off_(int fd, int n) {
  ssize_t filelen = lseek(fd, 0, SEEK_END);
  if (filelen == -1) {
    errExit("lseek1");
  }

  if (filelen == 0) {
    return 0;
  }

  if (lseek(fd, filelen - 1, SEEK_SET) == -1) {
    errExit("lseek2");
  }

  char last_byte;
  ssize_t nread_1;
  while ((nread_1 = read(fd, &last_byte, 1)) <= 0) {
    if (nread_1 == -1 && errno == EINTR) {
      continue;
    } else if (nread_1 == -1) {
      errExit("read");
    }
    if (nread_1 == 0) {
      return 0;
    }
  }
  Boolean last_byte_is_newline = last_byte == '\n' ? TRUE : FALSE;

  off_t offset = filelen;

  int lines_processed = 0;
  int k_buf_read = 1;
  Boolean start_was_touched = FALSE;
  while (lines_processed < n + last_byte_is_newline) {

    offset = filelen - (k_buf_read++) * BUFSIZ;
    size_t to_read = BUFSIZ;
    if (offset < 0 && !start_was_touched) {
      to_read = BUFSIZ + offset;
      offset = 0;
      start_was_touched = TRUE;
    } else if (offset < 0 && start_was_touched) {
      offset = 0;
      break;
    }
    if (lseek(fd, offset, SEEK_SET) == -1) {
      errExit("lseek3");
    }

    char buf[BUFSIZ];
    ssize_t nread = 0, nread_total = 0;
    while (1) {
      nread = read(fd, buf + nread_total, to_read - nread_total);
      // printf("nread=%ld\n", nread);
      if (nread == -1) {
        if (errno == EINTR) {
          continue;
        }
        errExit("read");
      }

      if (nread == 0) {
        break;
      }

      nread_total += nread;
    }

    ssize_t i;
    for (i = nread_total - 1; i >= 0; i--) {
      if (buf[i] == '\n') {
        // printf("found newline\n");
        lines_processed++;
      }

      if (lines_processed >= n + last_byte_is_newline) {
        return offset + i + 1;
      }
    }
  }
  return offset;
}

static void print_file_from_offset_(int fd, off_t offset) {
  if (lseek(fd, offset, SEEK_SET) == -1) {
    errExit("lseek");
  }

  char buf[BUFSIZ];

  ssize_t nread;
  while ((nread = read(fd, buf, BUFSIZ))) {
    if (nread == -1 && errno != EINTR) {
      errExit("read");
    } else if (nread == 0) {
      break;
    } else if (nread == -1) {
      continue;
    }

    ssize_t nwrite, nwritten = 0;
    while ((nwrite = write(STDOUT_FILENO, buf + nwritten, nread - nwritten)) !=
           nread - nwritten) {
      if (nwrite == -1 && errno != EINTR) {
        errExit("write");
      } else if (nwrite == -1) {
        continue;
      }

      nwritten += nwrite;
    }
  }
}

void tail(int n, char *filename) {
  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    errExit("open");
  }

  off_t offset = find_start_off_(fd, n);
  print_file_from_offset_(fd, offset);
}
