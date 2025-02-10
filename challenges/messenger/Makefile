CC=gcc
CCFLAGS=-static -g -masm=intel
SOURCES=exploit.c
BIN=hack
KERNEL_PATH=bzImage
CPIO=initramfs.cpio

# do this before using pack or unpack commands
# gzip -d ./initramfs.cpio.gz

all:hack

hack:
	$(CC) $(SOURCES) -o $(BIN) $(CCFLAGS)

pack:
	mv $(BIN) ./root/bin/. && \
	cd root && \
	find . -print0 | cpio --null --create --format=newc > ../$(CPIO) && \
	cd ..

unpack:
	mkdir root; cd root && cpio -id < ../$(CPIO) && cd ..

run: exploit.c
	qemu-system-x86_64 \
		-m 128M \
		-smp 2 \
		-kernel bzImage \
		-initrd initramfs.cpio \
		-append "console=ttyS0 kaslr kpti=1 panic=1 slab_nomerge panic_on_oops=1" \
		-snapshot -monitor /dev/null -nographic -no-reboot \
		-cpu qemu64,+smep,+smap

dbg:
	qemu-system-x86_64 \
		-m 128M \
		-smp 2 \
		-kernel bzImage \
		-initrd initramfs.cpio \
		-append "console=ttyS0 kaslr kpti=1 panic=1 slab_nomerge panic_on_oops=1" \
		-snapshot -monitor /dev/null -nographic -no-reboot \
		-cpu qemu64,+smep,+smap \
		-s
