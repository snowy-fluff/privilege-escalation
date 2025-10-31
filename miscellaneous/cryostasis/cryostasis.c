#define _GNU_SOURCE 1
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <signal.h>

int main(int argc, char **argv, char **envp)
{
    setvbuf(stdin, NULL, _IONBF, 0);

    for (int i = 3; i < 10000; i++)
    {
        close(i);
    }

    for (char **a = argv; *a; a++)
    {
        memset(*a, 0, strlen(*a));
    }

    for (char **e = envp; *e; e++)
    {
        memset(*e, 0, strlen(*e));
    }

    int flag_fd = open("/flag", 0);
    if (flag_fd < 0)
    {
        printf("\n  ERROR: Failed to open the flag -- %s!\n", strerror(errno));
        if (geteuid() != 0)
        {
            printf("  Your effective user id is not 0!\n");
            printf("  You must directly run the suid binary in order to have the correct permissions!\n");
        }
        exit(-1);
    }

    void *flag = mmap(NULL, 0x1000, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    assert(flag != MAP_FAILED);

    ssize_t flag_sz = read(flag_fd, flag, 64);
    if (flag_sz <= 0)
    {
        printf("\n  ERROR: Failed to read the flag -- %s!\n", strerror(errno));
        exit(-1);
    }

    close(flag_fd);
    unlink("/flag");

    assert(mprotect(flag, 0x1000, PROT_NONE) != -1);
    flag = (char *)0xDEADBEEF;

    void *sc = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    assert(sc != MAP_FAILED);

    ssize_t sc_sz = read(0, sc, 0x1000);
    assert(sc_sz > 0);

    assert(mprotect(sc, 0x1000, PROT_READ | PROT_EXEC) != -1);

    ((void (*)(void))sc)();
    return 0;
}
