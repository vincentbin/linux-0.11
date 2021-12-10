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
int j;

sem* s1;
sem* s2;

int main()
{
    char s_name1[20];
    char s_name2[20];
    strcpy(s_name1, "semaphore a1");
    strcpy(s_name2, "semaphore a2");

    s1 = (sem*) sem_open(s_name1, 1);
    s2 = (sem*) sem_open(s_name2, 0);

    i = 0;
    while (i < 15) {
        if (!fork()) {
            sem_wait(s1);
            printf("%d: %d\n", getpid(), i % 2);
            fflush(stdout);
            sem_post(s2);
            exit(0);
        }
        i = i + 2;
    }

    j = 1;
    while (j < 15) {
        if (!fork()) {
            sem_wait(s2);
            printf("%d: %d\n", getpid(), j % 2);
            fflush(stdout);
            sem_post(s1);
            exit(0);
        }
        j = j + 2;
    }

    while (waitpid(-1, NULL, 0) > 0);
    sem_unlink(s_name1);
    sem_unlink(s_name2);
    return 0;
}
