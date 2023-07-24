# target remote localhost:1234
gdb -ex "set architecture i386:x86-64" -ex "target remote :1234" -ex "add-symbol-file ./root/chall.ko 0xffffffffc0000000" -ex "b device_write"
# gdb -ex "set architecture i386:x86-64" -ex "target remote :1234"
