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
