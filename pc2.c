#define __LIBRARY__

#include <stdio.h>
#include <linux/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>

_syscall2(sem*, sem_open, const char*, name, unsigned int, value);
_syscall1(int, sem_wait, sem*, s);
_syscall1(int, sem_post, sem*, s);
_syscall1(int, sem_unlink, const char*, name);

int i;

int main()
{
    i = 0;
    while (i < 15) {
        if (i % 2 == 0) {
            if (!fork()) {
                printf("%d: %d\n", getpid(), i % 2);
                fflush(stdout);
                exit(0);
            }
        } else {
            if (!fork()) {
                printf("%d: %d\n", getpid(), i % 2);
                fflush(stdout);
                exit(0);
            }
        }
        i++;
    }
    while (waitpid(-1, NULL, 0) > 0);
    return 0;
}
