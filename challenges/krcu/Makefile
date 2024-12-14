CC=gcc
CCFLAGS=-static -g `pkg-config fuse --cflags --libs`

SOURCES=exploit.c
BIN=exploit
KERNEL_PATH=bzImage
CPIO=initramfs.cpio

all:hack

hack:
	$(CC) $(SOURCES) -o $(BIN) $(CCFLAGS)

run:
	qemu-system-x86_64 -m 1G -nographic -no-reboot \
		-enable-kvm -cpu host,+smep,+smap -smp 2 \
		-kernel ./server/bzImage \
		-nic user,model=virtio-net-pci \
		-drive file=server/rootfs.img,if=virtio,cache=none,aio=native,format=raw \
		-drive file=./exploit,if=virtio,cache=none,aio=native,format=raw \
		-drive file=server/flag.txt,if=virtio,cache=none,aio=native,format=raw \
		-append "kaslr console=ttyS0 root=/dev/vda rw quiet" -monitor /dev/null

# no kvm for debugging
dbg:
	qemu-system-x86_64 -m 1G -nographic -no-reboot \
		-cpu kvm64,+smep,+smap -smp 2 \
		-kernel ./server/bzImage \
		-nic user,model=virtio-net-pci \
		-drive file=server/rootfs.img,if=virtio,cache=none,aio=native,format=raw \
		-drive file=./exploit,if=virtio,cache=none,aio=native,format=raw \
		-drive file=server/flag.txt,if=virtio,cache=none,aio=native,format=raw \
		-append "nokaslr console=ttyS0 root=/dev/vda rw quiet" -monitor /dev/null \
		-s

kdbg:
	qemu-system-x86_64 -m 1G -nographic -no-reboot \
		-enable-kvm -cpu host,+smep,+smap -smp 2 \
		-kernel ./server/bzImage \
		-nic user,model=virtio-net-pci \
		-drive file=server/rootfs.img,if=virtio,cache=none,aio=native,format=raw \
		-drive file=./exploit,if=virtio,cache=none,aio=native,format=raw \
		-drive file=server/flag.txt,if=virtio,cache=none,aio=native,format=raw \
		-append "nokaslr console=ttyS0 root=/dev/vda rw quiet" -monitor /dev/null \
		-s