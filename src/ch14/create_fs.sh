#!/usr/bin/env bash
# Создаёт образ с файловой системой заданного типа и монтирует его.
#
# Использование:
#   ./mkfs_img.sh <fstype> [size] [inodes]   создать и примонтировать
#   ./mkfs_img.sh -u <fstype>                отмонтировать и удалить образ
#
# Примеры:
#   ./mkfs_img.sh ext2
#   ./mkfs_img.sh xfs 2G
#   ./mkfs_img.sh ext3 1G 400000
#   ./mkfs_img.sh -u ext2
#
# Образ:        $IMG_DIR/<fstype>.img   (по умолчанию /tmp)
# Точка монт.:  $MNT_DIR/mnt-<fstype>   (по умолчанию /tmp)

set -euo pipefail

IMG_DIR="${IMG_DIR:-/tmp}"
MNT_DIR="${MNT_DIR:-/tmp}"

usage() {
  sed -n '2,14p' "$0" | sed 's/^# \{0,1\}//'
  exit 1
}

[[ $# -ge 1 ]] || usage

# ---- отмонтирование ----
if [[ $1 == "-u" ]]; then
  [[ $# -eq 2 ]] || usage
  fs=$2
  img="$IMG_DIR/$fs.img"
  mnt="$MNT_DIR/mnt-$fs"
  if mountpoint -q "$mnt"; then
    sudo umount "$mnt"
  fi
  rmdir "$mnt" 2>/dev/null || true
  rm -f "$img"
  echo "removed $fs"
  exit 0
fi

fs=$1
size=${2:-1G}
inodes=${3:-300000}
img="$IMG_DIR/$fs.img"
mnt="$MNT_DIR/mnt-$fs"

if mountpoint -q "$mnt"; then
  echo "error: $mnt already mounted (run: $0 -u $fs)" >&2
  exit 1
fi

mkdir -p "$mnt"

if [[ $fs == "tmpfs" ]]; then
  sudo mount -t tmpfs -o "size=$size" tmpfs "$mnt"
else
  command -v "mkfs.$fs" >/dev/null || {
    echo "error: mkfs.$fs not found" >&2
    exit 1
  }

  rm -f "$img"
  truncate -s "$size" "$img"

  case "$fs" in
  ext2 | ext3 | ext4)
    # -N: число inode фиксируется при создании, берём с запасом
    "mkfs.$fs" -q -F -N "$inodes" "$img"
    ;;
  xfs)
    # inode выделяются динамически, минимальный размер образа ~300M
    mkfs.xfs -q -f "$img"
    ;;
  *)
    "mkfs.$fs" "$img"
    ;;
  esac

  sudo mount -o loop "$img" "$mnt"
fi

sudo chown "$USER": "$mnt"

df -T "$mnt"
echo "mounted at $mnt"
