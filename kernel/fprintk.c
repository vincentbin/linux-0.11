//
// Created by 52750 on 2021/12/3.
//

#include "linux/sched.h"
#include "sys/stat.h"

static char logbuf[1024];
int fprintk(int fd, const char *fmt, ...)
{
    va_list args;
    int count;
    struct file * file;
    struct m_inode * inode;

    va_start(args, fmt);
    count=vsprintf(logbuf, fmt, args);
    va_end(args);
/* 如果输出到stdout或stderr，直接调用sys_write即可 */
    if (fd < 3)
    {
        __asm__("push %%fs\n\t"
                "push %%ds\n\t"
                "pop %%fs\n\t"
                "pushl %0\n\t"
                /* 注意对于Windows环境来说，是_logbuf,下同 */
                "pushl $logbuf\n\t"
                "pushl %1\n\t"
                /* 注意对于Windows环境来说，是_sys_write,下同 */
                "call sys_write\n\t"
                "addl $8,%%esp\n\t"
                "popl %0\n\t"
                "pop %%fs"
        ::"r" (count),"r" (fd):"ax","cx","dx");
    }
    else
/* 假定>=3的描述符都与文件关联。事实上，还存在很多其它情况，这里并没有考虑。*/
    {
        /* 从进程0的文件描述符表中得到文件句柄 */
        if (!(file=task[0]->filp[fd]))
            return 0;
        inode=file->f_inode;

        __asm__("push %%fs\n\t"
                "push %%ds\n\t"
                "pop %%fs\n\t"
                "pushl %0\n\t"
                "pushl $logbuf\n\t"
                "pushl %1\n\t"
                "pushl %2\n\t"
                "call file_write\n\t"
                "addl $12,%%esp\n\t"
                "popl %0\n\t"
                "pop %%fs"
        ::"r" (count),"r" (file),"r" (inode):"ax","cx","dx");
    }
    return count;
}
