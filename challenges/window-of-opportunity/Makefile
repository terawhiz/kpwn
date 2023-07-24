CC=gcc
CCFLAGS="-static"
BIN=hack
KERNEL_PATH=bzImage
CPIO=initramfs.cpio

all:hack

pack:
	cp $(BIN) ./root/bin/. && \
	cd root && \
	find . -print0 | cpio --null --create --format=newc > ../$(CPIO) && \
	cd ..

unpack:
	mkdir root; cd root && cpio -id < ../$(CPIO) && cd ..

hack: $(BIN).c
	$(CC) $(BIN).c -o $(BIN) $(CCFLAGS)

run:
	qemu-system-x86_64 \
        -m 256M\
        -kernel $(KERNEL_PATH) \
        -initrd $(CPIO)  \
        -cpu kvm64,+smep,+smap \
        -append "console=ttyS0 oops=panic panic=1 kpti=1 nokaslr quiet" \
        -drive file=$(BIN),format=raw \
        -monitor /dev/null \
        -serial mon:stdio \
        -nographic -s \
        -no-reboot
