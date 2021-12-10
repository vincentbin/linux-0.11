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


int main()
{
    char s_name[20];
    pid_t p_pid;

    char s_name1[20];
    char s_name2[20];
    s_name1 = "semaphore a1";
    s_name2 = "semaphore a2";

    s1 = sem_open(s_name1, 1);
    s2 = sem_open(s_name2, 0);

    int i = 0;
    while (i < 30) {
        if (i % 2 == 0) {
            if (!fork()) {
                sem_wait(s1);
                printf("%d: %d\n", getpid(), i % 2);
                fflush(stdout);
                sem_post(s2);
            }
        } else {
            if (!fork()) {
                sem_wait(s2);
                printf("%d: %d\n", getpid(), i % 2);
                fflush(stdout);
                sem_post(s1);
            }
        }
    }
    sem_unlink(s_name1);
    sem_unlink(s_name2);
    return 0;
}
