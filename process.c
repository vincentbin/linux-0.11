//
// Created by 52750 on 2021/12/3. (注 linux 0.11 无法编译// 注释)
//
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

#define HZ    100

void cpuio_bound(int last, int cpu_time, int io_time);

int main(int argc, char *argv[]) {
    pid_t n_proc[10];
    int i;
    for (i = 0; i < 10; i++) {
        n_proc[i] = fork();
        /*子进程*/
        if (n_proc[i] == 0) {
            cpuio_bound(20, 2 * i, 20 - 2 * i); /* 每个子进程都占用20s */
            return 0; /* 执行完cpuio_bound 以后，结束该子进程 */
        } else if (n_proc[i] < 0) {
            printf("Failed to fork child process %d!\n", i + 1);
            return -1;
        }
        /*父进程继续fork*/
    }
    /*打印所有子进程PID*/
    for (i = 0; i < 10; i++) {
        printf("Child PID: %d\n", n_proc[i]);
    }
    /*等待所有子进程完成*/
    wait(&i);  /*Linux 0.11 上 gcc要求必须有一个参数, gcc3.4+则不需要*/
    return 0;
}

/*
 * 此函数按照参数占用CPU和I/O时间
 * last: 函数实际占用CPU和I/O的总时间，不含在就绪队列中的时间，>=0是必须的
 * cpu_time: 一次连续占用CPU的时间，>=0是必须的
 * io_time: 一次I/O消耗的时间，>=0是必须的
 * 如果last > cpu_time + io_time，则往复多次占用CPU和I/O
 * 所有时间的单位为秒
 */
void cpuio_bound(int last, int cpu_time, int io_time) {
    struct tms start_time, current_time;
    clock_t utime, stime;
    int sleep_time;

    while (last > 0) {
        /* CPU Burst */
        times(&start_time);
        /* 其实只有t.tms_utime才是真正的CPU时间。但我们是在模拟一个
         * 只在用户状态运行的CPU大户，就像“for(;;);”。所以把t.tms_stime
         * 加上很合理。*/
        do {
            times(&current_time);
            utime = current_time.tms_utime - start_time.tms_utime;
            stime = current_time.tms_stime - start_time.tms_stime;
        } while (((utime + stime) / HZ) < cpu_time);
        last -= cpu_time;

        if (last <= 0)
            break;

        /* IO Burst */
        /* 用sleep(1)模拟1秒钟的I/O操作 */
        sleep_time = 0;
        while (sleep_time < io_time) {
            sleep(1);
            sleep_time++;
        }
        last -= sleep_time;
    }
}
