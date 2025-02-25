BIN=exploit
KERNEL_PATH=bzImage
CPIO=rootfs.cpio

# CC=musl-gcc
CC=gcc
CCFLAGS=-static -lbpf

all:hack

pack:
	mv $(BIN) ./root/bin/hack && \
	cd root && \
	find . -print0 | cpio --null --create --format=newc > ../$(CPIO) && \
	cd ..

unpack:
	mkdir root; cd root && cpio -id < ../$(CPIO) && cd ..

hack:
	@{ \
		echo "#include <linux/bpf.h>"; \
		echo ""; \
		echo "char instrs[] = {"; \
	} > bpf_program.h && \
    (clang -target bpf -c prog.S -o - | llvm-objcopy-19 -O binary /proc/self/fd/0 - | xxd -i) >> bpf_program.h && \
    echo "};" >> bpf_program.h && \
	$(CC) $(BIN).c -o $(BIN) $(CCFLAGS)

run:
	qemu-system-x86_64  \
    -m 512M  \
    -smp 2 \
    -kernel bzImage    \
    -append "console=ttyS0 quiet panic=-1 nokaslr sysctl.kernel.io_uring_disabled=1 sysctl.kernel.dmesg_restrict=1 sysctl.kernel.kptr_restrict=2 sysctl.kernel.unprivileged_bpf_disabled=0"     \
    -initrd rootfs.cpio \
    -drive file=./flag,if=virtio,format=raw,readonly=on \
    -nographic  \
    -net nic,model=e1000 \
    -no-reboot \
    -monitor /dev/null

dbg:
	qemu-system-x86_64  \
    -m 512M  \
    -smp 2 \
    -kernel bzImage_dbg   \
    -append "console=ttyS0 panic=-1 nokaslr init=/init quiet sysctl.kernel.dmesg_restrict=1 sysctl.kernel.io_uring_disabled=1 sysctl.kernel.kptr_restrict=2 sysctl.kernel.unprivileged_bpf_disabled=0"     \
    -initrd rootfs.cpio \
    -drive file=./flag,if=virtio,format=raw,readonly=on \
    -nographic  \
    -net nic,model=e1000 \
    -no-reboot \
    -monitor /dev/null \
    -s