CPIO=rootfs.cpio
BIN=hack


hack: exploit.c
	$(CC) -o hack exploit.c -static -g

run: hack pack
	qemu-system-x86_64 \
		-kernel $(PWD)/bzImage \
		-cpu qemu64,+smep,+smap,+rdrand \
		-m 4G \
		-smp 4 \
		-initrd $(CPIO) \
		-hda flag.txt \
		-append "console=ttyS0 quiet loglevel=3 oops=panic panic_on_warn=1 panic=-1 pti=on page_alloc.shuffle=1" \
		-monitor /dev/null \
		-nographic \
		-no-reboot

dbg: hack pack
	qemu-system-x86_64 \
		-kernel $(PWD)/bzImage \
		-cpu qemu64,+smep,+smap,+rdrand \
		-m 4G \
		-smp 4 \
		-initrd $(CPIO) \
		-hda flag.txt \
		-append "console=ttyS0 quiet loglevel=3 oops=panic panic_on_warn=1 panic=-1 pti=on page_alloc.shuffle=1" \
		-monitor /dev/null \
		-nographic \
		-no-reboot \
		-s

unpack:
	mkdir root; cd ./root && cpio -id < ../$(CPIO) && cd ..

pack:
	cp $(BIN) ./root/bin/. && \
	cd ./root && \
	find . -print0 | cpio --null --create --format=newc > ../$(CPIO) && \
	cd ..

gdb:
	sudo gdb -ex "target remote :1234" -ex "c"

clean: hack 
	rm hack
