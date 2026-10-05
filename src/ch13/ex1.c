/*
 * Using the time built-in command of the shell, try timing the operation of the
program in Listing 4-1 (copy.c) on your system.

a) Experiment with different file and buffer sizes. You can set the buffer size
using the –DBUF_SIZE=nbytes option when compiling the program.

b) Modify the open() system call to include the O_SYNC flag. How much difference
does this make to the speed for various buffer sizes?

c) Try performing these timing tests on a range of file systems (e.g., ext3,
XFS, Btrfs, and JFS). Are the results similar? Are the trends the same when
going from small to large buffer sizes?
 */

/* SOLUTION
 * RUN ex.py from src/ directory to obtain results
 */

/* A
buf_size=1024 file_size=10M fs=xfs osync=0:
        user=0.00s sys=0.03s cpu=99% total=0.032s
buf_size=1024 file_size=10M fs=xfs osync=1:
        user=0.01s sys=0.27s cpu=4% total=6.263s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=1024 file_size=10M fs=ext4 osync=0:
        user=0.01s sys=0.02s cpu=99% total=0.031s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=1024 file_size=10M fs=ext4 osync=1:
        user=0.01s sys=0.24s cpu=3% total=7.192s
buf_size=1024 file_size=10M fs=btrfs osync=0:
        user=0.00s sys=0.05s cpu=99% total=0.051s
buf_size=1024 file_size=10M fs=btrfs osync=1:
        user=0.00s sys=1.83s cpu=20% total=8.741s
buf_size=1024 file_size=100M fs=xfs osync=0:
        user=0.05s sys=0.25s cpu=99% total=0.304s
buf_size=1024 file_size=100M fs=xfs osync=1:
        user=0.17s sys=2.73s cpu=4% total=60.139s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=1024 file_size=100M fs=ext4 osync=0:
        user=0.03s sys=0.25s cpu=99% total=0.287s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=1024 file_size=100M fs=ext4 osync=1:
        user=0.09s sys=2.56s cpu=4% total=60.301s
buf_size=1024 file_size=100M fs=btrfs osync=0:
        user=0.03s sys=0.46s cpu=99% total=0.494s
buf_size=1024 file_size=100M fs=btrfs osync=1:
        user=0.06s sys=18.57s cpu=22% total=83.559s
buf_size=1024 file_size=1G fs=xfs osync=0:
        user=0.45s sys=2.64s cpu=99% total=3.089s
buf_size=1024 file_size=1G fs=xfs osync=1:
        user=1.91s sys=26.90s cpu=4% total=577.457s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=1024 file_size=1G fs=ext4 osync=0:
        user=0.45s sys=2.44s cpu=91% total=3.149s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=1024 file_size=1G fs=ext4 osync=1:
        user=1.03s sys=24.14s cpu=4% total=606.487s
buf_size=1024 file_size=1G fs=btrfs osync=0:
        user=0.42s sys=4.57s cpu=99% total=4.990s
buf_size=1024 file_size=1G fs=btrfs osync=1:
        user=0.63s sys=191.78s cpu=21% total=876.951s
buf_size=4096 file_size=10M fs=xfs osync=0:
        user=0.00s sys=0.01s cpu=99% total=0.012s
buf_size=4096 file_size=10M fs=xfs osync=1:
        user=0.00s sys=0.08s cpu=3% total=1.973s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=4096 file_size=10M fs=ext4 osync=0:
        user=0.00s sys=0.01s cpu=99% total=0.014s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=4096 file_size=10M fs=ext4 osync=1:
        user=0.00s sys=0.09s cpu=3% total=2.294s
buf_size=4096 file_size=10M fs=btrfs osync=0:
        user=0.00s sys=0.01s cpu=99% total=0.016s
buf_size=4096 file_size=10M fs=btrfs osync=1:
        user=0.00s sys=0.54s cpu=21% total=2.563s
buf_size=4096 file_size=100M fs=xfs osync=0:
        user=0.01s sys=0.10s cpu=99% total=0.107s
buf_size=4096 file_size=100M fs=xfs osync=1:
        user=0.03s sys=0.74s cpu=3% total=20.309s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=4096 file_size=100M fs=ext4 osync=0:
        user=0.01s sys=0.11s cpu=99% total=0.126s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=4096 file_size=100M fs=ext4 osync=1:
        user=0.04s sys=0.98s cpu=4% total=20.927s
buf_size=4096 file_size=100M fs=btrfs osync=0:
        user=0.01s sys=0.13s cpu=99% total=0.144s
buf_size=4096 file_size=100M fs=btrfs osync=1:
        user=0.00s sys=5.62s cpu=21% total=25.572s
buf_size=4096 file_size=1G fs=xfs osync=0:
        user=0.11s sys=0.95s cpu=99% total=1.061s
buf_size=4096 file_size=1G fs=xfs osync=1:
        user=0.27s sys=7.69s cpu=4% total=197.127s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=4096 file_size=1G fs=ext4 osync=0:
        user=0.12s sys=1.11s cpu=83% total=1.479s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=4096 file_size=1G fs=ext4 osync=1:
        user=0.15s sys=8.64s cpu=4% total=205.371s
buf_size=4096 file_size=1G fs=btrfs osync=0:
        user=0.11s sys=1.29s cpu=99% total=1.396s
buf_size=4096 file_size=1G fs=btrfs osync=1:
        user=0.02s sys=57.86s cpu=22% total=261.005s
buf_size=65536 file_size=10M fs=xfs osync=0:
        user=0.00s sys=0.00s cpu=97% total=0.003s
buf_size=65536 file_size=10M fs=xfs osync=1:
        user=0.00s sys=0.00s cpu=5% total=0.132s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=65536 file_size=10M fs=ext4 osync=0:
        user=0.00s sys=0.00s cpu=98% total=0.005s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=65536 file_size=10M fs=ext4 osync=1:
        user=0.00s sys=0.01s cpu=6% total=0.152s
buf_size=65536 file_size=10M fs=btrfs osync=0:
        user=0.00s sys=0.01s cpu=98% total=0.011s
buf_size=65536 file_size=10M fs=btrfs osync=1:
        user=0.00s sys=0.03s cpu=19% total=0.165s
buf_size=65536 file_size=100M fs=xfs osync=0:
        user=0.00s sys=0.03s cpu=99% total=0.028s
buf_size=65536 file_size=100M fs=xfs osync=1:
        user=0.00s sys=0.07s cpu=5% total=1.318s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=65536 file_size=100M fs=ext4 osync=0:
        user=0.00s sys=0.04s cpu=99% total=0.039s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=65536 file_size=100M fs=ext4 osync=1:
        user=0.00s sys=0.09s cpu=6% total=1.531s
buf_size=65536 file_size=100M fs=btrfs osync=0:
        user=0.00s sys=0.09s cpu=99% total=0.095s
buf_size=65536 file_size=100M fs=btrfs osync=1:
        user=0.00s sys=0.44s cpu=25% total=1.744s
buf_size=65536 file_size=1G fs=xfs osync=0:
        user=0.01s sys=0.26s cpu=99% total=0.274s
buf_size=65536 file_size=1G fs=xfs osync=1:
        user=0.02s sys=0.72s cpu=5% total=13.682s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=65536 file_size=1G fs=ext4 osync=0:
        user=0.01s sys=0.36s cpu=62% total=0.598s
mke2fs 1.47.2 (1-Jan-2025)
buf_size=65536 file_size=1G fs=ext4 osync=1:
        user=0.04s sys=0.90s cpu=6% total=14.345s
buf_size=65536 file_size=1G fs=btrfs osync=0:
        user=0.01s sys=0.89s cpu=99% total=0.892s
buf_size=65536 file_size=1G fs=btrfs osync=1:
        user=0.01s sys=4.68s cpu=27% total=17.149s
➜  src git:(main) ✗
 */

#include "tlpi_hdr.h"
#include <fcntl.h>
#include <sys/stat.h>

#ifndef BUF_SIZE /* Allow "cc -D" to override definition */
#define BUF_SIZE 1024
#endif

#ifndef OSYNCON
#define OSYNCON 0
#endif

int main(int argc, char *argv[]) {
  int inputFd, outputFd, openFlags;
  mode_t filePerms;
  ssize_t numRead;
  char buf[BUF_SIZE];

  if (argc != 3 || strcmp(argv[1], "--help") == 0)
    usageErr("%s old-file new-file\n", argv[0]);

  /* Open input and output files */

  inputFd = open(argv[1], O_RDONLY);
  if (inputFd == -1)
    errExit("opening file %s", argv[1]);

  openFlags = O_CREAT | O_WRONLY | O_TRUNC;
  if (OSYNCON == 1) {
    openFlags |= O_SYNC;
  }
  filePerms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
  /* rw-rw-rw- */
  outputFd = open(argv[2], openFlags, filePerms);
  if (outputFd == -1)
    errExit("opening file %s", argv[2]);

  /* Transfer data until we encounter end of input or an error */

  while ((numRead = read(inputFd, buf, BUF_SIZE)) > 0)
    if (write(outputFd, buf, numRead) != numRead)
      fatal("write() returned error or partial write occurred");
  if (numRead == -1)
    errExit("read");

  if (close(inputFd) == -1)
    errExit("close input");
  if (close(outputFd) == -1)
    errExit("close output");

  exit(EXIT_SUCCESS);
}
