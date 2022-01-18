# linux-0.11
:blue_book: 理论课：https://www.bilibili.com/video/BV1d4411v7u7

:blue_book: 有用的参考资料（*linux-0.11* 注释版）：https://github.com/beride/linux0.11-1

*main branch* 为 *linux-0.11* 源码

## :heavy_check_mark: lab0 操作系统的引导（branch lab0）
1. 改写 bootsect.s 主要完成如下功能：
    - bootsect.s 能在屏幕上打印一段提示信息“XXX is booting...”。

2. 改写 setup.s 主要完成如下功能：
    - bootsect.s 能完成 setup.s 的载入，并跳转到 setup.s 开始地址执行。而 setup.s 向屏幕输出一行"Now we are in SETUP"。
    setup.s 能获取至少一个基本的硬件参数（如内存参数、显卡参数、硬盘参数等），将其存放在内存的特定地址，并输出到屏幕上。
    setup.s 不再加载 Linux 内核，保持上述信息显示在屏幕上即可。
    
:blue_book: 实验指导书：https://www.lanqiao.cn/courses/115/labs/568/document/

## :heavy_check_mark: lab1 实现系统调用（branch lab1）
1. 在 kernel/system_call.s 修改系统调用总数为74。
2. 在 include/unistd.h 中添加宏，指明调用函数表定位。
3. 在 include/linux/sys.h 中写入两个函数（sys_iam, sys_whoami）函数定义。并在其后的函数表增加两个新函数。
4. 新增 kernel/who.c 实现两个系统调用。 
5. 修改makefile
    - OBJS 增加who的依赖（who.o）
    - Dependencies中增加 who.s 和 who.o的依赖产生条件
6. make 执行./run 进入linux子系统，在/usr/include/unistd.h中增加iam 和 whoami 的宏定义（同第2点）。
7. 在用户态编写测试程序测试是否成功。

:blue_book: 实验指导书：https://www.lanqiao.cn/courses/115/labs/569/document/

## :heavy_check_mark: lab2 实现进程运行轨迹的跟踪（branch lab2）
1. process.c 实现了模拟cpu计算以及io计算混合的场景，并采用多进程的方式运行。
2. 将内核进程切换的几个状态进行打印输出（pid status time）到/var/process.log。
3. 在 init/main 中的init()函数 进入用户态语句后，将文件描述符3关联到/var/process.log。
4. 在 kernel/printk 中实现fprintk()函数，以打印输出到process.log日志。
5. 寻找状态切换点，并加入fprintk()写日志。
    - 进程开始 kernel/system_call.s -> sys_fork (call copy_process) 
    - 运行、阻塞态切换 kernel/sched.c -> schedule sys_pause sleep_on interruptible_sleep_on wake_up
    - 退出 kernel/exit.c -> do_exit sys_waitpid
6. 注意：linux-0.11 中 gcc 不可以编译 // 注释。

:blue_book: 实验指导书：https://www.lanqiao.cn/courses/115/labs/570/document/

## :heavy_check_mark: lab3 替换linux0.11原有的tss进程切换为堆栈切换（branch lab3）
原切换方式是通过tss，相当于是寄存器的快照，通过intel提供的指令直接进行现场替换，速度较慢。堆栈切换的效率更高。
### kernel/sched.c
1. 原switch_to方法基于tss切换，我们要在头文件将其注释掉。
2. 新switch_to，需要两个参数（1：下一个pcb的指针 2：下一个任务在数组中的位置 用于切换LDT）。
### kernel/system_call.s
1. 编写switch_to 汇编实现
2. pcb的切换
3. 重写tss内核栈位置（此时保留tss，但全局只有一个tss，不利用它来做进程切换。）
4. 切换内核栈
5. LDT的切换
### kernel/fork.c
1. 将pcb中tss的设置注销掉。
2. 在pcb中加入新的成员变量 —— kernel stack，用于存储内核栈信息。
3. 将栈信息写入此处，并让pcb该成员变量指向该指针。

:blue_book: 实验指导书：https://www.lanqiao.cn/courses/115/labs/571/document/

:blue_book: 参考：https://blog.csdn.net/qq_42518941/article/details/119182097

## :heavy_check_mark: lab4 在linux0.11实现信号量系统调用（branch lab4）
### 前置重点知识点
1. kernal/sched.c sleep_on 传入等待队列队首，将current置为阻塞态，主动执行调度schedule()。tmp存储原阻塞队列，当被唤醒时，将阻塞队列全部置为Runnable。
2. kernal/sched.c wake_up 唤醒所有被阻塞的pcb，程序中只能看到唤醒队首，但要结合sleep_on读，一旦结合就会发现，头被唤醒会使后续全部唤醒。
### 编写应用程序“pc.c”，解决经典的生产者—消费者问题，完成下面的功能：
- 建立一个生产者进程，N 个消费者进程（N>1）
- 用文件建立一个共享缓冲区
- 生产者进程依次向缓冲区写入整数 0,1,2,…,M，M>=500
- 消费者进程从缓冲区读数，每次读一个，并将读出的数字从缓冲区删除，然后将本进程 ID 和 + 数字输出到标准输出
- 缓冲区同时最多只能保存 10 个数
### 实现信号量
```c
sem_t* sem_open(const char *name, unsigned int value);
int sem_wait(sem_t *sem);
int sem_post(sem_t *sem);
int sem_unlink(const char *name);
```
- sem_open打开一个信号量
- 信号量减一 如果当前等于零则阻塞进程
- 信号量加一
- 关闭一个信号量
### wait 与 post 的重点
- wait
    - 调用kernal/sched.c sleep_on 进行等待阻塞
    - 利用关中断（linux0.11 单核）做到保护临界区
    - 对 sem 的 value 减一
- post
    - 对 sem 的 value 加一，如果 value 大于0则调用 kernal/sched.c wake_up 唤醒被此信号量阻塞的进程
    - 利用关中断（linux0.11 单核）做到保护临界区

:blue_book: 实验指导书：https://www.lanqiao.cn/courses/115/labs/572/document/

## :heavy_check_mark: lab5 地址的映射与共享（branch lab5）
逻辑地址 -> GDT -> LDT -> 页表 -> 物理地址
### 访存理论知识
### 段选择子

由 GDTR 访问全局描述符表是通过“段选择子”（实模式下的段寄存器）完成的

15&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&emsp;&ensp;3 &thinsp;  2 &ensp;  1 0 

|&ensp;&emsp;&emsp;&emsp;index&emsp;&emsp;&emsp;| &emsp;  | RPL |

- 3-15 为描述符索引, 表示所需要的段的描述符在描述符表的位置。
- 2 为指示选择子在 GDT 选择还是在LDT选择 (0 代表GDT 1 代表LDT)。
- 0-1 为选择特权级。

段选择子包括三部分：描述符索引（index）、TI、请求特权级（RPL）。它的index（描述符索引）部分表示所需要的段的描述符在描述符表的位置，由这个位置再根据在GDTR中存储的描述符表基址就可以找到相应的描述符。然后用描述符表中的段基址加上逻辑地址（SEL:OFFSET）的OFFSET就可以转换成线性地址，段选择子中的TI值只有一位0或1，0代表选择子是在GDT选择，1代表选择子是在LDT选择。请求特权级（RPL）则代表选择子的特权级，共有4个特权级（0级、1级、2级、3级）。

关于特权级的说明：任务中的每一个段都有一个特定的级别。每当一个程序试图访问某一个段时，就将该程序所拥有的特权级与要访问的特权级进行比较，以决定能否访问该段。系统约定，CPU 只能访问同一特权级或级别较低特权级的段。

例如给出逻辑地址：21h:12345678h转换为线性地址

a. 选择子 SEL = 21h = 0000000000100 0 01(b) 代表的意思是：选择子的index=4即0100，选择 GDT 中的第4个描述符；TI=0 代表选择子是在 GDT 选择；最后的01代表特权级 RPL=1。

b. OFFSET=12345678h若此时 GDT 第四个描述符中描述的段基址为11111111h，则线性地址 = 11111111h + 12345678h = 23456789h。

### 访问GDT
1. 先从 GDTR 寄存器中获得 GDT 基址。
2. 然后在 GDT 中以段选择器高13位位置索引值得到段描述符。
3. 获取基址，加上偏移量获得线性地址。

### 访问LDT
1. 先从 GDTR 寄存器中获得 GDT 基址。
2. 从 LDTR 寄存器中获取 LDT 所在段的位置索引 (LDTR 高13位)。
3. 以这个位置索引在 GDT 中得到 LDT 段描述符从而得到 LDT 段基址。
4. 用段选择器高13位位置索引值从 LDT 段中得到段描述符。
5. 获取基址，加上偏移量获得线性地址。

:blue_book: 实验指导书：https://www.lanqiao.cn/courses/115/labs/573/document/

## :heavy_check_mark: lab6 终端设备的控制（branch lab6）
1. 键盘中断发生时，取出键盘扫描码根据 key_table 表进行扫描码处理。
2. 完成 F12 键对应的函数编写。
3. 处理完毕后将有对应扫描码的字符放入 put_queue。
4. 调用 do_tty_interrupt 进行最后的处理，其中 copy_to_cooked 做最后的预处理，然后调用 con_write 输出到显卡。
5. write -> sys_write -> tty_write -> con_write。

:blue_book: 实验指导书：https://www.lanqiao.cn/courses/115/labs/574/document/

## :heavy_check_mark: lab7 proc文件系统的实现（branch lab7）
### 理论知识
### 磁盘
对机械磁盘的读写需要三个参数进行定位 
1. 柱面（C）
2. 磁头（H）
3. 扇区（S）
```c
block = C * (H * S) + H * S + S;
```
将几个扇区划分为一个block来提升磁盘io效率（linux0.11 将2个扇区划分为一个block）对于更上层的角度而言，只需要输入读写的block号即可进行磁盘io。

划分：
引导块 | 超级块 | inode位图 | 数据位图 | inode块 | 数据块

### 文件
用 FCB（linux0.11 中的 inode）来存储文件信息，其中包括不同种类的文件（例：设备文件，目录文件...）。
```c
struct m_inode 
 { 
 unsigned short i_mode;      // 文件类型和属性(rwx 位)。 
 unsigned short i_uid;       // 用户id（文件拥有者标识符）。 
 unsigned long i_size;       // 文件大小（字节数）。 
 unsigned long i_mtime;      // 修改时间（自1970.1.1:0 算起，秒）。 
 unsigned char i_gid;        // 组id(文件拥有者所在的组)。 
 unsigned char i_nlinks;     // 文件目录项链接数。 
 unsigned short i_zone[9];   // 直接(0-6)、间接(7)或双重间接(8)逻辑块号。 
 /* these are in memory also */ 
 struct task_struct *i_wait; // 等待该i 节点的进程。 
 unsigned long i_atime;      // 最后访问时间。 
 unsigned long i_ctime;      // i 节点自身修改时间。 
 unsigned short i_dev;       // i 节点所在的设备号。 
 unsigned short i_num;       // i 节点号。 
 unsigned short i_count;     // i 节点被使用的次数，0 表示该i 节点空闲。 
 unsigned char i_lock;       // 锁定标志。 
 unsigned char i_dirt;       // 已修改(脏)标志。 
 unsigned char i_pipe;       // 管道标志。 
 unsigned char i_mount;      // 安装标志。 
 unsigned char i_seek;       // 搜寻标志(lseek 时)。 
 unsigned char i_update;     // 更新标志。 
 }; 
```
inode里存放着文件在磁盘中的 block 号，以及其他的一些文件描述信息。可以存在多级的盘块位置引导，分为直接索引、间接索引获得 block 位置。

### 目录
根据 目录 inode 找到对应数据盘块号中的数据，其中有该目录下存在子目录的 inode 盘块号。一层一层查找下去就可以找到最终目标目录的位置。

:blue_book: 实验指导书：https://www.lanqiao.cn/courses/115/labs/575/document/
