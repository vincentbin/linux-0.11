#define __LIBRARY__
#include <unistd.h>
#include <linux/sem.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/sched.h>

_syscall2(sem_t *,sem_open,const char *,name,unsigned int,value)
_syscall1(int,sem_wait,sem_t *,sem)
_syscall1(int,sem_post,sem_t *,sem)
_syscall1(int,sem_unlink,const char *,name)

        const char *FILENAME = "/usr/root/buffer_file";    /* 消费生产的产品存放的缓冲文件的路径 */
        const int NR_CONSUMERS = 5;                        /* 消费者的数量 */
        const int NR_ITEMS = 50;                        /* 产品的最大量 */
        const int BUFFER_SIZE = 10;                        /* 缓冲区大小，表示可同时存在的产品数量 */
        sem_t *metux, *full, *empty;                    /* 3个信号量 */
        unsigned int item_pro, item_used;                /* 刚生产的产品号；刚消费的产品号 */
        int fi, fo;                                        /* 供生产者写入或消费者读取的缓冲文件的句柄 */


                int main(int argc, char *argv[])
{
    char *filename;
    int pid;
    int i;

    filename = argc > 1 ? argv[1] : FILENAME;
    /* O_TRUNC 表示：当文件以只读或只写打开时，若文件存在，则将其长度截为0（即清空文件）
     * 0222 和 0444 分别表示文件只写和只读（前面的0是八进制标识）
     */
    fi = open(filename, O_CREAT| O_TRUNC| O_WRONLY, 0222);    /* 以只写方式打开文件给生产者写入产品编号 */
    fo = open(filename, O_TRUNC| O_RDONLY, 0444);            /* 以只读方式打开文件给消费者读出产品编号 */

    metux = sem_open("METUX", 1);    /* 互斥信号量，防止生产消费同时进行 */
    full = sem_open("FULL", 0);        /* 产品剩余信号量，大于0则可消费 */
    empty = sem_open("EMPTY", BUFFER_SIZE);    /* 空信号量，它与产品剩余信号量此消彼长，大于0时生产者才能继续生产 */

    item_pro = 0;

    if ((pid = fork()))    /* 父进程用来执行消费者动作 */
    {
        printf("pid %d:\tproducer created....\n", pid);
        /* printf()输出的信息会先保存到输出缓冲区，并没有马上输出到标准输出（通常为终端控制台）。
         * 为避免偶然因素的影响，我们每次printf()都调用一下stdio.h中的fflush(stdout)
         * 来确保将输出立刻输出到标准输出。
         */
        fflush(stdout);

        while (item_pro <= NR_ITEMS)    /* 生产完所需产品 */
        {
            sem_wait(empty);
            sem_wait(metux);

            /* 生产完一轮产品（文件缓冲区只能容纳BUFFER_SIZE个产品编号）后
             * 将缓冲文件的位置指针重新定位到文件首部。
             */
            if(!(item_pro % BUFFER_SIZE))
                lseek(fi, 0, 0);

            write(fi, (char *) &item_pro, sizeof(item_pro));        /* 写入产品编号 */
            printf("pid %d:\tproduces item %d\n", pid, item_pro);
            fflush(stdout);
            item_pro++;

            sem_post(full);        /* 唤醒消费者进程 */
            sem_post(metux);
        }
    }
    else    /* 子进程来创建消费者 */
    {
        i = NR_CONSUMERS;
        while(i--)
        {
            if(!(pid=fork()))    /* 创建i个消费者进程 */
            {
                pid = getpid();
                printf("pid %d:\tconsumer %d created....\n", pid, NR_CONSUMERS-i);
                fflush(stdout);

                while(1)
                {
                    sem_wait(full);
                    sem_wait(metux);

                    /* read()读到文件末尾时返回0，将文件的位置指针重新定位到文件首部 */
                    if(!read(fo, (char *)&item_used, sizeof(item_used)))
                    {
                        lseek(fo, 0, 0);
                        read(fo, (char *)&item_used, sizeof(item_used));
                    }

                    printf("pid %d:\tconsumer %d consumes item %d\n", pid, NR_CONSUMERS-i+1, item_used);
                    fflush(stdout);

                    sem_post(empty);    /* 唤醒生产者进程 */
                    sem_post(metux);

                    if(item_used == NR_ITEMS)    /* 如果已经消费完最后一个商品，则结束 */
                        goto OK;
                }
            }
        }
    }
    OK:
    close(fi);
    close(fo);
    return 0;
}
