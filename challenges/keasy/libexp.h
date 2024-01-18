#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sched.h>
#include <pthread.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/xattr.h>
#include <sys/types.h>
#include <sys/sendfile.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#include <linux/if_packet.h>

#define PAGE 0x1000

typedef struct {
    long mtype;
    char mtext[];
} msg_t;

int create_temp_file(char* template) {
    int fd = mkstemp(template);
    if (fd < 0) error("Error creating temporary files");

    if (unlink(template) < 0) error("Error deleting temp file");


    return fd;
}

void error(const char* format, ...)
{
    if (!format) {
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "%s", "[-] ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, ": %s\n", strerror(errno));

    sleep(5);
    exit(EXIT_FAILURE);
}

void info(const char* format, ...)
{
    if (!format) {
        return;
    }
    
    fprintf(stderr, "%s", "[+] ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fprintf(stderr, "%s", "\n");
}

// Pin process to single cpu
// @param cpu: cpu id
void pin_to_cpu(int cpu){
    cpu_set_t cs;

    CPU_ZERO(&cs);
    CPU_SET(cpu, &cs);
    if (sched_setaffinity(getpid(), sizeof(cs), &cs))
        error("sched_setaffinity");
}

// Set fd limit of a process to the maximum amount
void set_fd_limit_max(){
    struct rlimit rlim;
    if (getrlimit(RLIMIT_NOFILE, &rlim)) error("getrlimit");

    rlim.rlim_cur = rlim.rlim_max;
    if (setrlimit(RLIMIT_NOFILE, &rlim)) error("setrlimit");

    info("FD limit set to %ld", rlim.rlim_cur);
}

void examine(uint64_t* buffer, int n){
    fprintf(stderr, "========================= EXAMINE =========================\n");
    for (int i = 0; i < n; i++){
        fprintf(stderr, "[%04x] 0x%016lx\n", i, buffer[i]);
    }
}

void breakpoint(){
    info("debug");
    getchar();
}

void noleave(){
    while (1);
}