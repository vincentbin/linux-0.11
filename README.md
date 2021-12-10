# linux-0.11

## lab1 实现系统调用（branch lab1）
1. 在 kernel/system_call.s 修改系统调用总数为74。
2. 在 include/unistd.h 中添加宏，指明调用函数表定位。
3. 在 include/linux/sys.h 中写入两个函数（sys_iam, sys_whoami）函数定义。并在其后的函数表增加两个新函数。
4. 新增 kernel/who.c 实现两个系统调用。 
5. 修改makefile 
    - OBJS 增加who的依赖（who.o）
    - Dependencies中增加 who.s 和 who.o的依赖产生条件。
6. make 执行./run 进入linux子系统，在/usr/include/unistd.h中增加iam 和 whoami 的宏定义（同第2点）。
7. 在用户态编写测试程序测试是否成功。

## lab2 实现进程运行轨迹的跟踪（branch lab2）
1. process.c 实现了模拟cpu计算以及io计算混合的场景，并采用多进程的方式运行。
2. 将内核进程切换的几个状态进行打印输出（pid status time）到/var/process.log。
3. 在 init/main 中的init()函数 进入用户态语句后，将文件描述符3关联到/var/process.log。
4. 在 kernel/printk 中实现fprintk()函数，以打印输出到process.log日志。
5. 寻找状态切换点，并加入fprintk()写日志。
    - 进程开始 kernel/system_call.s -> sys_fork (call copy_process) 
    - 运行、阻塞态切换 kernel/sched.c -> schedule sys_pause sleep_on interruptible_sleep_on wake_up
    - 退出 kernel/exit.c -> do_exit sys_waitpid
6. 注意：linux-0.11 中gcc 不可以编译 //注释

## lab3 替换linux0.11原有的tss进程切换为堆栈切换（branch lab3）
原切换方式是通过tss，相当于是寄存器的快照，通过intel提供的指令直接进行现场替换，速度较慢。堆栈切换的效率更高。
### kernel/sched.c
1. 原switch_to方法基于tss切换，我们要在头文件将其注释掉。
2. 新switch_to，需要两个参数（1：下一个pcb的指针 2：下一个任务在数组中的位置 用于切换LDT）。
### kernel/system_call.s
1. 编写switch_to 汇编实现。
2. pcb的切换。
3. 重写tss内核栈位置。（此时保留tss，但全局只有一个tss，不利用它来做进程切换。）
4. 切换内核栈
5. LDT的切换
### kernel/fork.c
1. 将pcb中tss的设置注销掉。
2. 在pcb中加入新的成员变量 —— kernel stack，用于存储内核栈信息。
3. 将栈信息写入此处，并让pcb该成员变量指向该指针。

参考：https://blog.csdn.net/qq_42518941/article/details/119182097

## lab4 在linux0.11实现信号量系统调用（branch lab4）
### 前置重点知识点
1. kernal/sched.c sleep_on 传入等待队列队首，将current置为阻塞态，主动执行调度schedule()。tmp存储原阻塞队列，当被唤醒时，将阻塞队列全部置为Runnable。
2. kernal/sched.c wake_up 唤醒所有被阻塞的pcb，程序中只能看到唤醒队首，但要结合sleep_on读，一旦结合就会发现，头被唤醒会使后续全部唤醒。
