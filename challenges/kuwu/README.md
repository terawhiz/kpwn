# kuwu - BackdoorCTF 2024

Very similar to HITCON 2023 [wall rose](../wall-rose/exploit.c).

Running the chall:
```sh
make unpack
sudo chown -R root:root ./root
sudo chmod +s ./root/bin/busybox
make hack
sudo make pack
make run
```

[Compiling with musl-gcc](https://mem2019.github.io/jekyll/update/2021/07/19/GCTF2021-eBPF.html):
```sh
gcc -E exploit.c -o preprocessed_exploit.c
musl-gcc -static -Os -s preprocessed_exploit.c -o exp
```