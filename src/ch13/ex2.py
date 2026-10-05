import os
import resource
import subprocess
import time
from dataclasses import dataclass

SRC = "ch13/ex2.c"
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


VARIANTS = {
    "none": [],
    "osync": ["-DUSE_O_SYNC"],
    "fsync": ["-DUSE_FSYNC"],
    "fdatasync": ["-DUSE_FDATASYNC"],
}


def test(buf_size, num_bytes, fs, sync) -> TestRes:
    run(["truncate", "-s", "2G", "fs.img"])
    run([*MKFS[fs], "fs.img"], stdout=subprocess.DEVNULL)
    os.makedirs("mnt", exist_ok=True)
    run(["sudo", "mount", "-o", "loop", "fs.img", "mnt"])
    try:
        run(["sudo", "chown", f"{os.getuid()}:{os.getgid()}", "mnt"])

        cmd = [
            "gcc",
            "-c",
            SRC,
            *INCLUDES,
            "-o",
            "ex2.o",
        ]
        if sync:
            cmd.extend(VARIANTS[sync])

        run(cmd)
        run(["gcc", "ex2.o", *LIB_SRCS, *INCLUDES, "-o", "write_bytes"])

        run(["touch", "mnt/testfile"])
        run(["sync"])

        before = resource.getrusage(resource.RUSAGE_CHILDREN)
        t0 = time.perf_counter()
        run(["./write_bytes", "mnt/testfile", f"{num_bytes}", f"{buf_size}"])
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
        subprocess.run(["rm", "-f", "fs.img", "write_bytes", "ex2.o"])


BUF_SIZES = [1024, 4096, 65536]
NUM_BYTES = [10000000, 100000000, 1000000000]
FSS = ["xfs", "ext4", "btrfs"]


SYNC_VARS = ["none", "osync", "fsync", "fdatasync"]


def main():
    for buf_size in BUF_SIZES:
        for num_bytes in NUM_BYTES:
            for fs in FSS:
                for sync in SYNC_VARS:
                    res = test(buf_size, num_bytes, fs, sync)
                    print(
                        f"buf_size={buf_size} num_bytes={num_bytes} fs={fs} sync_var={sync}:\n"
                        f"\tuser={res.usertime:.2f}s sys={res.systemtime:.2f}s "
                        f"cpu={res.cpu_perc}% total={res.total_time:.3f}s",
                        flush=True,
                    )


if __name__ == "__main__":
    main()
