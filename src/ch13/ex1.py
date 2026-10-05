import os
import resource
import subprocess
import time
from dataclasses import dataclass

SRC = "ch13/ex1.c"
LIB_SRCS = ["../lib/error_functions.c", "../lib/get_num.c"]
INCLUDES = ["-I../lib", "-I."]

MKFS = {
    "ext4": ["mkfs.ext4", "-F"],
    "xfs": ["mkfs.xfs", "-f"],
    "btrfs": ["mkfs.btrfs", "-f"],
}


@dataclass
class TestRes:
    usertime: float
    systemtime: float
    cpu_perc: int
    total_time: float


def run(cmd, **kw):
    return subprocess.run(cmd, check=True, **kw)


def test(buf_size, file_size, fs, sync) -> TestRes:
    run(["truncate", "-s", "2G", "fs.img"])
    run([*MKFS[fs], "fs.img"], stdout=subprocess.DEVNULL)
    os.makedirs("mnt", exist_ok=True)
    run(["sudo", "mount", "-o", "loop", "fs.img", "mnt"])
    try:
        run(["sudo", "chown", f"{os.getuid()}:{os.getgid()}", "mnt"])

        run(
            [
                "gcc",
                "-c",
                SRC,
                *INCLUDES,
                f"-DBUF_SIZE={buf_size}",
                f"-DOSYNCON={sync}",
                "-o",
                "ex1.o",
            ]
        )
        run(["gcc", "ex1.o", *LIB_SRCS, *INCLUDES, "-o", "copy"])

        run(["truncate", "-s", file_size, "mnt/testfile"])
        run(["sync"])

        before = resource.getrusage(resource.RUSAGE_CHILDREN)
        t0 = time.perf_counter()
        run(["./copy", "mnt/testfile", "mnt/copyfile"])
        total = time.perf_counter() - t0
        after = resource.getrusage(resource.RUSAGE_CHILDREN)

        user = after.ru_utime - before.ru_utime
        system = after.ru_stime - before.ru_stime
        cpu = int(100 * (user + system) / total) if total > 0 else 0
        return TestRes(user, system, cpu, total)
    finally:
        subprocess.run(["sync"])
        subprocess.run(["sudo", "umount", "mnt"])
        subprocess.run(["rmdir", "mnt"])
        subprocess.run(["rm", "-f", "fs.img", "copy", "ex1.o"])


BUF_SIZES = [1024, 4096, 65536]
FILE_SIZES = ["10M", "100M", "1G"]
FSS = ["xfs", "ext4", "btrfs"]


def main():
    for buf_size in BUF_SIZES:
        for file_size in FILE_SIZES:
            for fs in FSS:
                for sync in (0, 1):
                    res = test(buf_size, file_size, fs, sync)
                    print(
                        f"buf_size={buf_size} file_size={file_size} fs={fs} osync={sync}:\n"
                        f"\tuser={res.usertime:.2f}s sys={res.systemtime:.2f}s "
                        f"cpu={res.cpu_perc}% total={res.total_time:.3f}s",
                        flush=True,
                    )


if __name__ == "__main__":
    main()

