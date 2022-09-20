![在这里插入图片描述](https://img-blog.csdnimg.cn/5ee93720470047a8b769945644e0cb03.png)

@[TOC](文章目录)



# 信号的三个阶段

学习信号需要学习信号产生前、信号产生中和信号产生后三个阶段。今天我们探讨后两个阶段。
![在这里插入图片描述](https://img-blog.csdnimg.cn/07c5e79b5d324bf3b61245b9fa3ca0e9.png)

# 信号产生中

**信号相关概念：**

- 实际执行信号的处理动作称为**信号递达(Delivery)**，信号递达后有三种处理方式：默认，忽略和自定义。
- 信号从产生到递达之间的状态,称为**信号未决(Pending)。**
- 进程可以选择阻塞 (Block )某个信号。
- 被阻塞的信号产生时将保持在未决状态,直到进程解除对此信号的阻塞,才执行递达的动作.
- 阻塞和忽略是不同的,只要信号被阻塞就不会递达,而忽略是在递达之后可选的一种处理动作。
- 信号可选的处理动作有三种：**默认(SIG_DFL)，忽略(SIG_IGN)和自定义**。

## 阻塞信号

**进程在收到信号后，对于信号的处理并不是立刻做出反应，这与进程的优先级有关，进程有可能在执行优先级更高的工作，信号就需要被暂时保存下来，进程的信号会被暂时保存在进程的task_struct中。**

**信号在内核中的表示图：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/9df4d246e9984946a2570d464f940aca.png)

- **每个信号都有两个标志位分别表示阻塞(block)和未决(pending)，还有一个函数指针表示处理动作。信号产生时，内核在进程控制块中设置该信号的未决标志，直到信号递达才清除该标志。**
- **如果在进程解除对某信号的阻塞之前这种信号产生过多次，Linux会将常规信号在递达之前产生多次只计一次，而实时信号在递达之前产生多次可以依次放在一个队列里。**
- 在上图的例子中,SIGHUP信号未阻塞也未产生过,当它递达时执行默认处理动作。
- SIGINT信号产生过，但正在被阻塞，所以暂时不能递达。虽然它的处理动作是忽略，但在没有解除阻塞之前不能忽略这个信号，因为进程仍有机会改变处理动作之后再解除阻塞。
- SIGQUIT信号未产生过，一旦产生SIGQUIT信号将被阻塞，它的处理动作是用户自定义函数handler。

---
## sigset_t 



- **每个信号只有一个bit的未决标志，非0即1，不记录该信号产生了多少次，阻塞标志也是这样表示的。**
- **因此，未决和阻塞标志可以用相同的数据类型sigset_t来存储，sigset_t称为信号集，这个类型可以表示每个信号的“有效”或“无效”状态，在阻塞信号集中“有效”和“无效”的含义是该信号是否被阻塞，而在未决信号集中“有效”和“无效”的含义是该信号是否处于未决状态。**
- **阻塞信号集也叫做当前进程的信号屏蔽字(Signal Mask)，这里的“屏蔽”应该理解为阻塞而不是忽略。**
---

## 信号集操作函数

sigset_t类型对于每种信号用一个bit表示“有效”或“无效”状态，至于这个类型内部如何存储这些bit则依赖于系统实现，从使用者的角度是不必关心的，使用者只能调用以下函数来操作sigset_ t变量，而不应该对它的内部数据做任何解释，比如用printf直接打印sigset_t变量是没有意义的。

**<kbd>sigemptyset</kbd>函数**

```c
#include <signal.h>

原型：
	int sigemptyset(sigset_t *set);
功能：
	函数sigemptyset初始化set所指向的信号集,使其中所有信号的对应bit清零,
	表示该信号集不包含任何有效信号。
```
**<kbd>sigfillset</kbd>函数**
```c
#include <signal.h>

原型：
	int sigfillset(sigset_t *set);
功能：
	函数sigfillset初始化set所指向的信号集,使其中所有信号的对应bit置位,
	表示该信号集的有效信号包括系统支持的所有信号。
```
**<kbd>sigaddset</kbd>函数** **和<kbd>sigdelset</kbd>函数**
```c
#include <signal.h>

原型：
	int sigaddset (sigset_t *set, int signo);
	int sigdelset(sigset_t *set, int signo);
功能：
	调用sigaddset和sigdelset在该信号集中添加或删除某种有效信号。
```
**<kbd>sigismember</kbd>函数** 
```c
#include <signal.h>

原型：
	int sigismember（const sigset_t *set, int signo);
功能：
	sigismember是一个布尔函数,用于判断一个信号集的有效信号中是否包含某种信号,
	若包含则返回1,不包含则返回0,出错返回-1。 
```
**<kbd>sigprocmask</kbd>函数** 

```c
#include <signal.h>

原型：
	int sigprocmask(int how, const sigset_t *set, sigset_t *oset); 
功能：
	可以读取或更改进程的信号屏蔽字(阻塞信号集)
返回值:
	若成功则为0,若出错则为-1
```
- 如果oset是非空指针,则读取进程的当前信号屏蔽字通过oset参数传出。
- 如果set是非空指针,则 更改进程的信号屏蔽字,参数how指示如何更改。
- 如果oset和set都是非空指针,则先将原来的信号 屏蔽字备份到oset里,然后根据set和how参数更改信号屏蔽字。


假设当前的信号屏蔽字为mask,下表说明了how参数的可选值：

![在这里插入图片描述](https://img-blog.csdnimg.cn/db1fa65e873347e685e0d381d36fa314.png)





**<kbd>sigpending</kbd>函数** 

```c
#include <signal.h>

原型：
	int sigpending(sigset_t *set); 
功能：
	读取当前进程的未决信号集,通过set参数传出。
返回值：
	调用成功则返回0,出错则返回-1。
```
---
下面利用这些信号集操作函数打印信号集，测试代码：

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void show_pending(sigset_t *pending){
   printf("process pending: ");
   int sig = 1;
   for(; sig <= 31; sig++){
       if(sigismember(pending, sig)){
           printf("1");
       }
       else{
           printf("0");
       }
   }
   printf("\n");
}

void handler(int signo){
   printf("%d信号可被递达\n", signo);
}

int main()
{
   sigset_t iset, oset;
   sigemptyset(&iset); // 将信号集全设置为0
   sigemptyset(&oset);

   signal(2, handler); // 将2号信号的处理方式设为自定义
   sigaddset(&iset, 2); // 设置iset中的二号信号
   sigprocmask(SIG_SETMASK, &iset, &oset); // 屏蔽进程的二号信号
   
   int count = 0;
   sigset_t pending;
   while(1){
       sigemptyset(&pending);

       sigpending(&pending); // 获取进程的信号位图

       show_pending(&pending); // 打印进程的信号位图
       sleep(1);
       count++;
       if(count == 5){
           sigprocmask(SIG_SETMASK, &oset, NULL);
       }
   }

   return 0;
}
```
**运行结果：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/efb26074cbbe4bca85275ce3d353a65f.png)


---


# 信号产生后

## 内核态和用户态



- **内核态：执行操作系统OS的代码和数据的时候所处的状态叫做内核态，操作系统的代码全都是在内核态执行的。**
- **用户态：用户代码和数据被访问或者执行的时候所处的状态就叫用户态，我们用户自己写的代码全部都是在用户态被执行的。**
- **主要区别：权限**


**感性认识：**

**当我们调用open系统调用时，除了会进入操作系统的open函数的具体实现，还会发生身份的变化，从用户态转化为内核态。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/5402112e17e848f7b524892d4cbcc2ee.png)

---

**理性认识：**

**每一个进程都有一个4G的进程地址空间，进程地址空间分为用户空间和内核空间，用户的代码和数据通过用户级页表映射到物理内存中，用户的代码要能运行就必须被加载到内存中，内核的代码和数据同理，也要被加载到内存中，开机就是在加载内核的代码和数据到内存中，内核空间和同样拥有一份页表，叫做内核级页表。进程为了维护进程的独立性，每个进程都有自己的进程地址空间，也有自己的用户级页表，但是内核级页表只有一份，被所有的进程共享。**


![在这里插入图片描述](https://img-blog.csdnimg.cn/62b3227d660547d49cf49cd2b8aa7bc2.png)




这样就保证了每个进程都能访问到自己用户的代码和内核的代码，但是能访问到内核的代码就可以执行内核的代码吗？

**所以在进程中就必须要有一种身份认证来辨别自己的工作模式，是访问用户的代码还是访问内核的代码，这种工作模式在进程中有相关的数据去标识，这个数据会被加载到CPU中。CPU中有一个寄存器叫做CR3，CR3为0表示内核态，CR3为3表示用户态就有权限执行内核的代码，进程就可以通过这个寄存器辨别是用户态还是内核态。**


**总结：**

- **用户态使用的是用户级页表，只能访问用户的代码和数据。**
- **内核态使用的是内核级页表，只能访问内核的代码和数据。**
- **CPU内有寄存器保存进程的状态。**
- **进程无论如何切换，都能保证找到同一个操作系统，因为每个进程都有进程地址空间使用同一份内核级页表。**
- **系统调用的本质是进程的身份从用户态变成内核态，通过内核级页表找到系统函数的实现执行。**
---

## 内核如何实现信号的捕捉
信号在被操作系统发送给进程时，进程会先把信号保存在pending位图里，进程有可能在做优先级更高的工作，信号会在合适的时候被延时处理，这取决于操作系统和进程，那么信号被处理的合适的时候是什么时候呢？

**信号被保存在进程的task_struct里的pending位图里，当进程从内核态返回用户态的时候，进行信号的检测和处理工作 -- 递达信号(默认，忽略和自定义)。**

 
 **如果信号的处理动作是用户自定义函数,在信号递达时就调用这个函数,这称为捕捉信号。由于信号处理函数的代码是在用户空间的,处理过程比较复杂,举例如下: 用户程序注册了SIGQUIT信号的处理函数sighandler。 当前正在执行main函数,这时发生中断或异常切换到内核态。 在中断处理完毕后要返回用户态的main函数之前检查到有信号SIGQUIT递达。 内核决定返回用户态后不是恢复main函数的上下文继续执行,而是执行sighandler函 数,sighandler和main函数使用不同的堆栈空间,它们之间不存在调用和被调用的关系,是 两个独立的控制流程。 sighandler函数返回后自动执行特殊的系统调用sigreturn再次进入内核态。 如果没有新的信号要递达,这次再返回用户态就是恢复main函数的上下文继续执行了。**

**信号捕捉示意图：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/92387f5a95d0404e867301f31c1995a3.png)
**精简图：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/df7fbeeca4ab4bc1a463d07c0ac675f3.png)

---

为什么一定要切换成用户态才能执行信号的自定义捕捉方法呢？

**信号的自定义捕捉方法是用户的代码和数据，操作系统（内核态）理论上是可以直接执行用户的代码的，但是因为操作系统的权限很大，不相信任何用户，不会去执行用户的代码，比如说如果在信号的自定义捕捉函数有<kbd>rm -rf /</kbd>，强制删除根目录，后果将不堪设想。操作系统为了防止执行恶意代码，不以内核态执行信号的捕捉函数，而是切换到用户态执行。**

---
## sigaction函数

```c
#include <signal.h>

原型：
	int sigaction(int signo, const struct sigaction *act, struct sigaction *ocat);
功能：
	信号自定义捕捉
返回值：
	调用成功返回0.出错返回-1.
```

- **sigaction函数可以读取和修改与指定信号相关联的处理动作。调用成功则返回0,出错则返回- 1。signo是指定信号的编号。若act指针非空,则根据act修改该信号的处理动作。若oact指针非空,则通过oact传出该信号原来的处理动作。act和oact指向sigaction结构体。**

**sigaction结构体：**

```c
struct sigaction {

	void     (*sa_handler)(int);
	void     (*sa_sigaction)(int, siginfo_t *, void *);
	sigset_t   sa_mask;
	int        sa_flags;
	void     (*sa_restorer)(void);
};
```
- **将sa_handler赋值为常数SIG_IGN传给sigaction表示忽略信号，赋值为常数SIG_DFL表示执行系统默认动作，赋值为一个函数指针表示用自定义函数捕捉信号，或者说向内核注册了一个信号处理函数，该函数返回值为void，可以带一个int参数，通过参数可以得知当前信号的编号，这样就可以用同一个函数处理多种信号。显然,这也是一个回调函数，不是被main函数调用,而是被系统所调用。**
- **如果在调用信号处理函数时,除了当前信号被自动屏蔽之外,还希望自动屏蔽另外一些信号,则用sa_mask字段说明这些需要额外屏蔽的信号,当信号处理函数返回时自动恢复原来的信号屏蔽字。**
- **sa_flags字段包含一些选项,本章的代码都把sa_flags设为0,sa_sigaction是实时信号的处理函数。**

**测试代码：**

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

void handler(int signo)
{
    printf("get a %d signal\n", signo);
}

int main()
{
    struct sigaction act;
    memset(&act, 0, sizeof(act));

    act.sa_handler = handler;

    sigaction(2, &act, NULL);
    while(1)
    {
        printf("hello world\n");
        sleep(1);
    }

    return 0;
}

运行结果：
[cwx@VM-20-16-centos signal]$ ./mytest 
hello world
hello world
^Cget a 2 signal
hello world
```

---
# 可重入函数
![在这里插入图片描述](https://img-blog.csdnimg.cn/68cea4e490714304adf39e0605a8c055.png)
- main函数调用insert函数向一个链表head中插入节点node1，插入操作分为两步，刚做完第一步的时候，因为硬件中断使进程切换到内核，再次回用户态之前检查到有信号待处理，于是切换到sighandler函数，sighandler也调用insert函数向同一个链表head中插入节点node2，插入操作的两步都做完之后从sighandler返回内核态，再次回到用户态就从main函数调用的insert函数中继续往下执行，先前做第一步之后被打断，现在继续做完第二步。结果是main函数和sighandler先后向链表中插入两个节点，而最只有一个节点真正插入链表中。
- **insert函数被不同的控制流程调用，有可能在第一次调用还没返回时就再次进入该函数，这称
为重入，insert函数访问一个全局链表，有可能因为重入而造成错乱，像这样的函数称为不可重入函数，反之，如果一个函数只访问自己的局部变量或参数，则称为可重入(Reentrant) 函数。**

如果一个函数符合以下条件之一则是不可重入的:

 - **调用了malloc或free，因为malloc也是用全局链表来管理堆的。**
 - **调用了标准I/O库函数。标准I/O库的很多实现都以不可重入的方式使用全局数据结构。**


---

# volatile关键字
请看以下这个程序：

```c
#include <stdio.h>
#include <signal.h>

int flag = 0; 

void handler(int signo){
    flag = 1;
    printf("get a signo: %d, flag 0 to 1\n", signo);
}

int main()
{
    signal(2, handler);
    while(!flag);
    printf("process normal quit..\n");

    return 0;
}

运行结果：
[cwx@VM-20-16-centos volatile]$ ./test_volatile 
^Cget a signo: 2, flag 0 to 1
process normal quit..
```
程序对2号信号进行捕捉，键入 CTRL-C ，2号信号被捕捉，执行自定义动作，修改 flag＝1，while 条件不满足，退出循环，进程退出。


---

## GCC优化

 - O1优化会消耗少多的编译时间，它主要对代码的分支，常量以及表达式等进行优化。
 - O2会尝试更多的寄存器级的优化以及指令级的优化，它会在编译期间占用更多的内存和编译时间。
 - O3在O2的基础上进行更多的优化，例如使用伪寄存器网络，普通函数的内联，以及针对循环的更多优化。


---

我们对之前的程序进行优化：

**Makefile文件：**

```c
test_volatile:test_volatile.c
	gcc -o $@ $^ -O3
	
.PHONY:clean 
clean:
	rm -rf test_volatile
```

```c
运行结果：
[cwx@VM-20-16-centos volatile]$ make
gcc -o test_volatile test_volatile.c -O3
[cwx@VM-20-16-centos volatile]$ ./test_volatile 
^Cget a signo: 2, flag 0 to 1
^Cget a signo: 2, flag 0 to 1
^Cget a signo: 2, flag 0 to 1
^Cget a signo: 2, flag 0 to 1
```

我们会发现键入 CTRL-C进程并不会退出，但是理论上flag变量已经被由0置1了，这是为什么？

 while 循环检查的flag，并不是内存中最新的flag，这就存在了数据二异性的问题。 while 检测的flag其实已经因为优化，被放在了CPU寄存器当中。这就需要volatile关键字解决这个问题：


```c
#include <stdio.h>
#include <signal.h>

volatile int flag = 0; // 保持内存的可见性

void handler(int signo){
    flag = 1;
    printf("get a signo: %d, flag 0 to 1\n", signo);
}

int main()
{
    signal(2, handler);
    while(!flag);
    printf("process normal quit..\n");

    return 0;
}

运行结果：
[cwx@VM-20-16-centos volatile]$ ./test_volatile 
^Cget a signo: 2, flag 0 to 1
process normal quit..
```

**<kbd>volatile</kbd>作用：保持内存的可见性，告知编译器，被该关键字修饰的变量，不允许被优化，对该变量的任何操作，都必须在真实的内存中进行操作。**

---

