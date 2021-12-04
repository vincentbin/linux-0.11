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
