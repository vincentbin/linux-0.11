#define __LIBRARY__

#include <stdio.h>
#include <linux/sem.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>

int i;
int j;

int main()
{
    i = 0;
    while (i < 15) {
        if (!fork()) {
            printf("%d: %d\n", getpid(), i % 2);
            fflush(stdout);
            exit(0);
        }
        i = i + 2;
    }

    j = 1;
    while (j < 15) {
        if (!fork()) {
            printf("%d: %d\n", getpid(), j % 2);
            fflush(stdout);
            exit(0);
        }
        j = j + 2;
    }

    return 0;
}
