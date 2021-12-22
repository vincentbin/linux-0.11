/*consumer.c*/
#define   __LIBRARY__
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/sem.h>

_syscall2(sem_t*,sem_open,const char *,name,unsigned int,value);
_syscall1(int,sem_wait,sem_t*,sem);
_syscall1(int,sem_post,sem_t*,sem);
_syscall1(int,sem_unlink,const char *,name);
_syscall1(void*,shmat,int,shmid);
_syscall2(int,shmget,int,key,int,size);

#define NUMBER 5 /*打出数字总数*/
#define BUFSIZE 10 /*缓冲区大小*/

int main()
{
    int  i,shmid,data;
    int *p;
    int  buf_out = 0; /*从缓冲区读取位置*/

    printf("consumer start.\n");
    fflush(stdout);
    shmid = shmget(1234, BUFSIZE);
    printf("shmid:%d\n",shmid);
    if(shmid == -1)
    {
        return -1;
    }
    p = (int *) shmat(shmid);

    for( i = 0; i < NUMBER; i++ ) {
        data = p[buf_out];
        buf_out = (buf_out + 1) % BUFSIZE;
        printf("%d: %d.\n", getpid(), data);
        fflush(stdout);
    }
    printf("consumer end.\n");
    fflush(stdout);
    return 0;
}
