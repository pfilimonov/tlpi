/*
 * Write a program that measures the time required to create and then remove a
 * large number of 1-byte files from a single directory. The program should
 * create files with names of the form xNNNNNN, where NNNNNN is replaced by a
 * random six-digit number. The files should be created in the random order in
 * which their names are generated, and then deleted in increasing numerical
 * order (i.e., an order that is different from that in which they were
 * created). The number of files (NF) and the directory in which they are to be
 * created should be specifiable on the command line. Measure the times required
 * for different values of NF (e.g., in the range from 1000 to 20,000) and for
 * different file systems (e.g., ext2, ext3, and XFS). What patterns do you
 * observe on each file system as NF increases? How do the various file systems
 * compare? Do the results change if the files are created in increasing
 * numerical order (x000000, x000001, x0000002, and so on) and then deleted in
 * the same order? If so, what do you think the reason(s) might be? Again, do
 * the results vary across file-system types?
 */

/* create fs helper:
./create_fs.sh ext2              # size 1G, 300000 inode, mounts to
/tmp/mnt-ext2
./create_fs.sh xfs 2G            # custom size
./create_fs.sh ext3 1G 400000
./create_fs.sh -u ext2           # unmount and remove
 */

/* results:
 * ============ XFS ==============
NF = 100000
  IN ORDER:
    Gen files time: real=6.38 user=0.09, sys=2.58
    Delete files time: real=3.40 user=0.05, sys=1.68
  RANDOM:
    Gen files time: real=6.40 user=0.11, sys=2.61
    Delete files time: real=2.88 user=0.06, sys=1.74

* ============ EXT2 =============
NF = 100000
  IN ORDER:
    Gen files time: real=2.94 user=0.08, sys=2.85
    Delete files time: real=1.13 user=0.03, sys=1.06
  RANDOM:
    Gen files time: real=2.93 user=0.09, sys=2.83
    Delete files time: real=1.34 user=0.04, sys=1.25

* ============ EXT3 =============
NF = 100000
  IN ORDER:
    Gen files time: real=3.03 user=0.10, sys=2.93
    Delete files time: real=1.39 user=0.02, sys=1.22
  RANDOM:
    Gen files time: real=3.06 user=0.10, sys=2.95
    Delete files time: real=1.88 user=0.04, sys=1.51
*/

#include "tlpi_hdr.h"
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/times.h>
#include <time.h>

void gen_files_random(size_t nf, char *dir_name, size_t *numbers);
void delete_files_asc(size_t nf, char *dir_name, const size_t *numbers);
void gen_files_asc(size_t nf, char *dir_name, size_t *numbers);

static int comp(const void *a, const void *b) {
  size_t x = *(const size_t *)a;
  size_t y = *(const size_t *)b;

  if (x < y)
    return -1;
  if (x > y)
    return 1;
  return 0;
}

static void make_path(char *buf, size_t len, const char *dir, size_t num) {
  snprintf(buf, len, "%s/x%06zu", dir, num);
}

size_t *numbers = NULL;

int main(int argc, char *argv[]) {
  srand(time(0));

  int opt;
  Boolean random_order = FALSE;
  ssize_t n_files = -1;
  char *dir_name = NULL;

  if (argc < 3) {
    usageErr("%s [-n num] [-d dir] [-r]", argv[0]);
  }

  while ((opt = getopt(argc, argv, "n:d:r")) != -1) {
    switch (opt) {
    case 'n':
      n_files = atoi(optarg);
      break;
    case 'd':
      dir_name = optarg;
      break;
    case 'r':
      random_order = TRUE;
      break;
    default:
      usageErr(argv[0], NULL);
    }
  }

  if (n_files < 0) {
    usageErr("%s [-n num] [-d dir] [-r]\n", argv[0]);
  }
  if (dir_name == NULL) {
    usageErr("%s [-n num] [-d dir] [-r]\n", argv[0]);
  }

  if (optind != argc) {
    usageErr("%s [-n num] [-d dir] [-r]\n", argv[0]);
  }

  numbers = malloc(n_files * sizeof(size_t));

  double clk = sysconf(_SC_CLK_TCK);
  clock_t ts1, ts2;
  struct tms t1;
  if ((ts1 = times(&t1)) == -1) {
    errExit("times");
  }
  if (random_order) {
    gen_files_random(n_files, dir_name, numbers);
  } else {
    gen_files_asc(n_files, dir_name, numbers);
  }
  struct tms t2;
  if ((ts2 = times(&t2)) == -1) {
    errExit("times");
  }

  printf("Gen files time: real=%.2f user=%.2f, sys=%.2f\n", (ts2 - ts1) / clk,
         (t2.tms_utime - t1.tms_utime) / clk,
         (t2.tms_stime - t1.tms_stime) / clk);

  if ((ts1 = times(&t1)) == -1) {
    errExit("times");
  }
  delete_files_asc(n_files, dir_name, numbers);
  if ((ts2 = times(&t2)) == -1) {
    errExit("times");
  }
  printf("Delete files time: real=%.2f user=%.2f, sys=%.2f\n",
         (ts2 - ts1) / clk, (t2.tms_utime - t1.tms_utime) / clk,
         (t2.tms_stime - t1.tms_stime) / clk);

  free(numbers);

  exit(EXIT_SUCCESS);
}

void gen_files_random(size_t nf, char *dir_name, size_t *numbers) {
  for (size_t i = 0; i < nf; i++) {
    int fd = -1;
    size_t num;
    while (1) {
      num = (size_t)rand() % 1000000;

      char path[BUFSIZ];
      make_path(path, BUFSIZ, dir_name, num);

      fd = open(path, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR);
      if (fd == -1) {
        if (errno == EEXIST) {
          continue;
        } else {
          errExit("open");
        }
      }

      break;
    }
    numbers[i] = num;
    if (write(fd, "a", 1) != 1) {
      errExit("write");
    }

    if (close(fd) == -1) {
      errExit("close");
    }
  }

  qsort(numbers, nf, sizeof(size_t), comp);
}

void delete_files_asc(size_t nf, char *dir_name, const size_t *numbers) {
  for (size_t i = 0; i < nf; i++) {
    size_t num = numbers[i];

    char path[BUFSIZ];
    make_path(path, BUFSIZ, dir_name, num);

    if (remove(path) != 0) {
      errExit("remove");
    }
  }
}

void gen_files_asc(size_t nf, char *dir_name, size_t *numbers) {
  for (size_t i = 0; i < nf; i++) {
    numbers[i] = i;

    char path[BUFSIZ];
    make_path(path, BUFSIZ, dir_name, i);

    int fd = open(path, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd == -1) {
      errExit("open");
    }
    if (write(fd, "a", 1) != 1) {
      errExit("write");
    }
    if (close(fd) == -1) {
      errExit("close");
    }
  }
}
