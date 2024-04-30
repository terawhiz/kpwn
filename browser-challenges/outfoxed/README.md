# Outfoxed - corCTF 2021

The patch adds a `oob` method to the JavaScript array. The `oob` method can be used to read/write out-of-bound from the array.

This was my first time doing browser so I had `0rganizers` writeup as a reference.

[Stage 1 shellcode](./shellcode.s) mprotects the memory pointed by `rax` register and jumps to it.

There are two exploit in this folder, [one](./jsShell_exploit.js) works only on JS shell where I overwrote `ShellPrincipals::securityCallbacks+8` ptr with a one gadget and [the other](./exploit.js) (probably) works on firefox following traditional spidermonkey exploitation method by duplicating classOps vtable.


**References**:
1) https://org.anize.rs/corCTF-2021/pwn/outfoxed
2) https://vigneshsrao.github.io/posts/writeup/
3) https://doar-e.github.io/blog/2018/11/19/introduction-to-spidermonkey-exploitation/
4) http://phrack.org/issues/69/14.html
