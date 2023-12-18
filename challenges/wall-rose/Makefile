IMAGEDIR=${PWD}
CPIO=initramfs.cpio
# GZCPIO=initramfs.cpio.gz
BIN=hack

hack: exploit.c exploit.h
	$(CC) -o hack *.c -static -g

run: hack
	qemu-system-x86_64 \
		-L $(IMAGEDIR)/bios \
		-kernel $(IMAGEDIR)/bzImage \
		-initrd $(IMAGEDIR)/$(CPIO) \
		-cpu kvm64,+smep,+smap \
		-monitor none \
		-m 1024M \
		-append "console=ttyS0 oops=panic panic=1 quiet" \
		-monitor /dev/null \
		-nographic \
		-no-reboot \
		-net user -net nic -device e1000

dbg: hack
	qemu-system-x86_64 \
		-L $(IMAGEDIR)/bios \
		-kernel $(IMAGEDIR)/bzImage \
		-initrd $(IMAGEDIR)/$(CPIO) \
		-cpu kvm64,+smep,+smap \
		-monitor none \
		-m 1024M \
		-append "console=ttyS0 oops=panic panic=1 quiet" \
		-monitor /dev/null \
		-nographic \
		-no-reboot \
		-net user -net nic -device e1000 \
		-s

gdb:
	sudo gdb ./vmlinux -ex "target remote :1234" -ex "c" -ex "kmalloc-tracer"

unpack:
	mkdir root; cd root && cpio -id < ../$(CPIO) && cd ..

pack:
	cp $(BIN) ./root/bin/. && \
	cd root && \
	find . -print0 | cpio --null --create --format=newc > ../$(CPIO) && \
	cd .. 

clean:
	rm -rf hack
