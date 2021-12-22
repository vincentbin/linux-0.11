/*producer.c*/
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
/*_syscall1(int,shmget,char*,name);*/

#define NUMBER 5 /*打出数字总数*/
#define BUFSIZE 10 /*缓冲区大小*/

int main()
{
    int  i, shmid;
    int *p;
    int  buf_in = 0; /*写入缓冲区位置*/
    /*shmid = shmget("buffer");*/
    shmid = shmget(1234, BUFSIZE);
    printf("shmid: %d.\n", shmid);
    if(shmid == -1)
    {
        return -1;
    }
    p = (int*) shmat(shmid);
    /*生产者进程*/
    printf("producer start.\n");
    fflush(stdout);
    for(i = 0 ; i < NUMBER; i++) {
        printf("%d.\n", i);
        p[buf_in] = i;
        buf_in = (buf_in + 1) % BUFSIZE;
    }
    printf("producer end.\n");
    fflush(stdout);
    return 0;
}
