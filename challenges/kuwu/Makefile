# CC=musl-gcc
CC=gcc
CCFLAGS=-static
BIN=exploit
KERNEL_PATH=bzImage
CPIO=initramfs.cpio

all:hack

pack:
	mv $(BIN) ./root/bin/. && \
	cd root && \
	find . -print0 | cpio --null --create --format=newc > ../$(CPIO) && \
	cd ..

unpack:
	mkdir root; cd root && cpio -id < ../$(CPIO) && cd ..

hack:
	$(CC) $(BIN).c -o $(BIN) $(CCFLAGS)

run:
	qemu-system-x86_64 \
    -m 128M \
    -kernel bzImage \
    -initrd initramfs.cpio \
    -append "console=ttyS0 loglevel=3 oops=panic panic=1 pti=off kaslr quiet" \
    -cpu qemu64,+smep \
    -monitor /dev/null \
    -nographic \
    -no-reboot

dbg:
	qemu-system-x86_64 \
    -m 128M \
    -kernel bzImage \
    -initrd initramfs.cpio \
    -append "console=ttyS0 loglevel=3 oops=panic panic=1 pti=off kaslr quiet" \
    -cpu qemu64,+smep \
    -monitor /dev/null \
    -nographic \
    -no-reboot -s
