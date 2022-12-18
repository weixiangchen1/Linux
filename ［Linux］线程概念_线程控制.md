 ![在这里插入图片描述](https://img-blog.csdnimg.cn/5ee93720470047a8b769945644e0cb03.png)

@[TOC](文章目录)

# 线程概念
**线程是在进程内部运行的一个执行分支（执行流），属于进程的一部分，粒度比进程更加轻量级。一个进程可能存在多个线程，进程：线程 = 1：n，操作系统需要管理进程，操作系统管理的方式是先描述，再组织，线程就应该有自己的线程控制块TCB来组织和管理线程，这是常规操作系统的做法，比如Windows。**

那么Linux是怎么管理线程的呢？

**Linux没有单独为线程制定线程控制块，而是用进程的PCB来模拟线程。Linux创建线程，只创建task_struct，共享同一个地址空间，进程的代码和数据被划分成若干份，分配给各个线程去执行。站在CPU的角度，调度的还是一个个的PCB，但是今天CPU看到的PCB比我们之前讲的PCB更加轻量级，一个PCB就是一个需要被调度的执行流。Linux这样管理线程，就不用维护线程和进程的关系，不用单独为线程设计任何算法，直接复用进程的管理方式，操作系统只需要聚焦在线程间的资源分配上。**




![在这里插入图片描述](https://img-blog.csdnimg.cn/7b22d3177ac14347b5e7dc12245d1ac8.png)

---
## 进程和线程的关系
**之前我们学习的进程是内部只有一个执行流的进程，今天我们学习的进程里面可以具有多个执行流。操作系统创建进程的成本是非常高的，成本主要体现在时间和空间上，创建进程需要使用的资源是很多的，这是一个从0到1的过程，而创建线程的代价比进程小得多。**

站在内核的视角：
- **进程是资源分配的基本单位。**
- **线程是CPU调度的基本单位，也是承担一部分进程资源的基本实体。**
- **透过进程虚拟地址空间，可以看到进程的大部分资源，将进程资源合理分配给每个执行流，就形成了线程执行流。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/594feee6dfe9403a9d4476fae6e5cdd4.png)

---
**线程共享进程数据，但也拥有自己的一部分数据:**

 - 线程ID
 - 一组寄存器
 - 栈
 - errno
 - 信号屏蔽字
 - 调度优先级


**进程的多个线程共享 同一地址空间,因此Text Segment、Data Segment都是共享的,如果定义一个函数,在各线程中都可以调用,如果定义一个全局变量,在各线程中都可以访问到,除此之外,各线程还共享以下进程资源和环境:**

 - 文件描述符表
 - 每种信号的处理方式(SIG_ IGN、SIG_ DFL或者自定义的信号处理函数)
 - 当前工作目录
 - 用户id和组id


---
## 线程的优点

 - **创建一个新线程的代价要比创建一个新进程小得多。**
 - **与进程之间的切换相比，线程之间的切换需要操作系统做的工作要少很多。**
 - **线程占用的资源要比进程少很多。**
 - **能充分利用多处理器的可并行数量。**
 - **在等待慢速I/O操作结束的同时，程序可执行其他的计算任务。**
 - **计算密集型应用（比如加密，大数据运算等，主要使用CPU资源），为了能在多处理器系统上运行，将计算分解到多个线程中实现。**
 - **I/O密集型应用（比如网络下载，在线直播等，主要使用内存和外设的IO资源），为了提高性能，将I/O操作重叠。线程可以同时等待不同的I/O操作，因为大部分都是在等待IO就绪的。**

线程是越多越好吗？

**不一定。计算密集型的应用，线程太多会导致线程间被过度调度切换，线程调度是有成本的。IO密集型应用则允许多一点线程，因为大部分都是在等待IO就绪的。**


---


## 线程的缺点
- **性能损失：**
一个很少被外部事件阻塞的计算密集型线程往往无法与共它线程共享同一个处理器。如果计算密集型线程的数量比可用的处理器多，那么可能会有较大的性能损失，这里的性能损失指的是增加了额外的同步和调度开销，而可用的资源不变。
- **健壮性降低：**
编写多线程需要更全面更深入的考虑，在一个多线程程序里，因时间分配上的细微偏差或者因共享了不该共享的变量而造成不良影响的可能性是很大的，换句话说线程之间是缺乏保护的。
- **缺乏访问控制：**
进程是访问控制的基本粒度，在一个线程中调用某些OS函数会对整个进程造成影响。
- **编程难度提高：**
编写与调试一个多线程程序比单线程程序困难得多。


---

# 线程控制
## Linux线程和接口关系的认识
因为Linux线程是用进程复用模拟的，所以Linux没有提供直接操作线程的接口，而是提供了在同一进程地址空间创建PCB的方法，分配资源给指定PCB的接口，但是这些接口为用户不友好，所以一些系统级别的工程师就在用户层对Linux轻量级进程接口进行封装，打包成库，让用户直接使用库接口（线程创建，释放线程，等待线程等等），这个库叫做**原生线程库(POSIX库)**。

 - **与线程有关的函数构成了一个完整的系列，绝大多数函数的名字都是以“pthread_”打头的.**
 - **要使用这些函数库，要通过引入头文<pthread.h>.**
 - **链接这些线程函数库时要使用编译器命令的“-lpthread”选项.**

---

## 线程创建

**<kbd>pthread_create</kbd>接口介绍**

```c
#include <pthread.h>

功能：创建一个新的线程
原型
	int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg);
参数
	thread:返回线程ID
	attr:设置线程的属性，attr为NULL表示使用默认属性
	start_routine:是个函数地址，线程启动后要执行的函数
	arg:传给线程启动函数的参数
返回值：
	成功返回0；失败返回错误码
```

**错误检查：**

 - 传统的一些函数是成功返回0，失败返回-1，并且对全局变量errno赋值以指示错误。
 - pthreads函数出错时不会设置全局变量errno（而大部分其他POSIX函数会这样做）。而是将错误代码通过返回值返回。
 - pthreads同样也提供了线程内的errno变量，以支持其它使用errno的代码。对于pthreads函数的错误，建议通过返回值业判定，因为读取返回值要比读取线程内的errno变量的开销更小。


**测试代码：**

```c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* pthread_run(void* args){
    while(1){
        printf("I am %s pthread\n", (const char*)args);
        sleep(1);
    }
}

int main()
{
    pthread_t pid;
    pthread_create(&pid, NULL, pthread_run, (void*)"new thread"); //创建新线程执行pthread_run

    while(1){
        printf("I am main pthread\n");
        sleep(1);
    }

    return 0;
}

运行结果：
[cwx@VM-20-16-centos pthread]$ make
gcc -o mytest mytest.c -pthread
[cwx@VM-20-16-centos pthread]$ ./mytest 
I am main pthread
I am new thread pthread
I am main pthread
I am new thread pthread
...
```



---

## 线程ID及进程地址空间布局

```c
#include <pthread.h>

功能：
	获取线程自身id
原型:
	pthread_t pthread_self(void);
```
**测试代码，查看主线程和新线程的进程id和线程id：**

```c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* pthread_run(void* args)
{
    while(1)
    {
        printf("I am %s, pid: %d, id: %lu\n", (const char*)args, getpid(), pthread_self());
        sleep(1);
    }
}

int main()
{
    pthread_t pid;
    pthread_create(&pid, NULL, pthread_run, (void*)"new thread"); //创建新线程执行pthread_run

    while(1)
    {
        printf("I am main thread, pid: %d, id: %lu\n", getpid(), pthread_self());
        sleep(1);
    }

    return 0;
}

运行结果：
[cwx@VM-20-16-centos pthread]$ ./mytest 
I am main thread, pid: 31377, id: 139738719135552
I am new thread, pid: 31377, id: 139738710800128
```

**<kbd>ps -aL</kbd>查看线程详情**

 ```c
[cwx@VM-20-16-centos pthread]$ ps -aL
  PID   LWP TTY          TIME CMD
31377 31377 pts/4    00:00:00 mytest
31377 31378 pts/4    00:00:00 mytest
```


- **一个进程内的线程的PID都是一样的。**
 - **pthread_ create函数会产生一个线程ID，存放在第一个参数指向的地址中。**


---
## 线程等待
为什么需要线程等待？
- **已经退出的线程，其空间没有被释放，仍然在进程的地址空间内，类似于"僵尸进程"。**
- **创建新的线程不会复用刚才退出线程的地址空间。**


**<kbd>pthread_join</kbd>接口介绍**

```c
功能：
	等待线程结束
原型：
	int pthread_join(pthread_t thread, void **value_ptr);
参数：
	thread:线程ID
	value_ptr:输出型参数，它指向一个指针，后者指向线程的返回值
返回值：
	成功返回0；失败返回错误码
```




**测试代码：**

```c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* pthread_run(void* args)
{
    printf("%s running...\n", (const char*)args);
        
    return (void*)123; // 返回退出码123
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, pthread_run, (void*)"new thread");

    void* status = NULL; // 获取退出码
    pthread_join(tid, &status);

    printf("exit code: %d\n", (int)status);

    return 0;
}

运行结果：
[cwx@VM-20-16-centos pthread]$ ./mytest 
new thread running...
exit code: 123
```
调用该函数的线程将挂起等待,直到id为thread的线程终止。线程获取退出码，得知代码的运行结果的对错，但是如果程序异常了呢？pthread_join需要或者有能力处理异常吗？

**pthread_join根本不需要处理程序异常，程序异常的处理是进程的任务，线程出现崩溃了，其他线程包括线程都崩溃了，pthread_join也没有意义了，线程出现异常是进程的问题而不是线程的问题。**


---
## 线程终止
如果需要只终止某个线程而不终止整个进程,可以有三种方法：

1. **从线程函数return。这种方法对主线程不适用,从main函数return相当于调用exit。主线程return相当于进程退出。其他线程return只代表当前进程退出。**
2. **线程可以调用pthread_ exit终止自己。**
3. **一个线程可以调用pthread_ cancel终止同一进程中的另一个线程。**

**<kbd>pthread_exit</kbd>接口介绍**

```c
#include <pthread.h>

功能：线程终止
原型：
	void pthread_exit(void *value_ptr);
参数：
	value_ptr:value_ptr不要指向一个局部变量。
返回值：
	无返回值，跟进程一样，线程结束的时候无法返回到它的调用者（自身）
```

**pthread_exit或者return返回的指针所指向的内存单元必须是全局的或者是用malloc分配的，不能在线程函数的栈上分配，因为当其它线程得到这个返回指针时线程函数已经退出了。**

**测试代码：**

```c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* pthread_run(void* args)
{
    printf("%s running...\n", (const char*)args);

    pthread_exit((void*)123); 
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, pthread_run, (void*)"new thread");

    void* status = NULL; // 获取退出码
    pthread_join(tid, &status);

    printf("exit code: %d\n", (int)status);

    return 0;
}

运行结果：
[cwx@VM-20-16-centos pthread]$ ./mytest 
new thread running...
exit code: 123
```

---


**<kbd>pthread_cancel</kbd>接口介绍**

```c
#include <pthread.h>

功能：
	取消一个执行中的线程
原型：
	int pthread_cancel(pthread_t thread);
参数：
	thread:线程ID
返回值：
	成功返回0；失败返回错误码
```

**测试代码（主线程取消新线程）：**

```c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* pthread_run(void* args)
{
    while(1)
    {
        printf("%s runing...\n", (const char*)args);
        sleep(1);
    }
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, pthread_run, (void*)"new thread");
 
    printf("3s cancel new thread...\n");
    sleep(3);
    pthread_cancel(tid);  // 主线程取消新线程

    void *status = NULL;
    pthread_join(tid, &status);
    printf("exit code: %d\n", (int)status);

    return 0;
}

运行结果:
[cwx@VM-20-16-centos pthread]$ ./mytest 
3s cancel new thread...
new thread runing...
new thread runing...
new thread runing...
exit code: -1
```
**如果进程被正常取消，退出码返回-1，这个值代表PTHREAD_CANCELED,
通过<kbd>grep</kbd>命令查找这个宏定义：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/f4c18a20d2ab403aba849877231afbcf.png)



**测试代码（新线程取消主线程）:**

```c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_t g_tid;

void* thread_run(void* args)
{
    while(1)
    {
        printf("5s after cancel main thread, %s running...\n", (const char*)args);
        sleep(5);
        pthread_cancel(g_tid); // 新线程取消主线程
    }
}

int main()
{
    g_tid = pthread_self();
    pthread_t tid;
    pthread_create(&tid, NULL, thread_run, (void*)"new thread");

    sleep(20);
    pthread_join(tid, NULL);

    return 0;
}
```
**启动监控脚本查看线程详情：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/3cbe5c81f176401f887b81b157303c60.png)

![在这里插入图片描述](https://img-blog.csdnimg.cn/dd83ba1bc0224c9f8bd578a6ffb407b3.png)

**我们可以看到，新线程把主线程取消，主线程的状态就变成\<defunct\>状态，类似于"僵尸进程"，但是新线程取消主线程，并没有取消进程，新线程仍然在运行，通过<kbd> ps ajx | grep mytest</kbd>查看进程详情：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/7dae026332d84fc0aacc2fc306b9f30d.png)

**该进程已经变成了僵尸进程。**


---


## 线程终止状态
线程以不同的方法终止,通过pthread_join得到的终止状态是不同的，总结如下:
1. **如果thread线程通过return返回,value_ ptr所指向的单元里存放的是thread线程函数的返回值。**
2. **如果thread线程被别的线程调用pthread_ cancel异常终止掉,value_ ptr所指向的单元里存放的是常数PTHREAD_ CANCELED。**
3. **如果thread线程是自己调用pthread_exit终止的,value_ptr所指向的单元存放的是传给pthread_exit的参数。**
4. **如果对thread线程的终止状态不感兴趣,可以传NULL给value_ ptr参数。**


---

## 线程分离
- **默认情况下，新创建的线程是joinable的，线程退出后，需要对其进行pthread_join操作，否则无法释放资源，从而造成系统泄漏。**
- **如果不关心线程的返回值，join是一种负担，这个时候，我们可以使用线程分离，当线程退出时，自动释放线程资源。**

```c
#include <pthread.h>

原型：
	int pthread_detach(pthread_t thread);
功能：
	进行线程分离。
参数：
	thread:要分离的线程id
```
**可以是线程组内其他线程对目标线程进行分离，也可以是线程自己分离:**

```c
pthread_detach(pthread_self());
```

**线程等待和线程分离是矛盾的，一个线程不能既等待又分离。**

**测试代码：**

```c
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void* thread_run(void *args)
{
    pthread_detach(pthread_self()); // 新线程分离
    printf("%s running...\n", (const char*)args);
}

int main()
{
    pthread_t tid;
    pthread_create(&tid, NULL, thread_run, (void*)"new thread");
    sleep(1);

    if(pthread_join(tid, NULL) == 0){
        printf("pthread join success\n");
    }
    else{
        printf("pthread wait fail\n");
    }

    return 0;
}


运行结果：
[cwx@VM-20-16-centos pthread]$ ./mytest 
new thread running...
pthread wait fail
```


---

## LWP和pthread_t


 - **线程ID属于进程调度的范畴。因为线程是轻量级进程，是操作系统调度器的最小单位，所以需要一个数值来唯一表示该线程。**
 - **pthread_ create函数第一个参数指向一个虚拟内存单元，该内存单元的地址即为新创建线程的线程ID，属于NPTL线程库的范畴。线程库的后续操作，就是根据该线程ID来操作线程的。**

	
**创建五个线程，打印线程id，并用<kbd>ps -aL</kbd>查看详情：**
![在这里插入图片描述](https://img-blog.csdnimg.cn/b475ee7245084637aa43dd15bc3ca868.png)
**LWP是内核的线程id，而我们打印的是pthread库的线程id，这两个的值不一样。**

那么pthread_t到底代表的是什么呢？

**我们查看的线程id是pthread库的id，不是Linux内核中的LWP。pthread库的id是一个内存地址。**

**<kbd>ldd mytest</kbd>查看进程的链接详情：**
![在这里插入图片描述](https://img-blog.csdnimg.cn/1216e1a24f1c44fd90890e7807c3b62e.png)
**pthread库要被进程访问，libpthread-2.17.so就必须从磁盘上被加载到内存里，并通过页表映射到进程地址空间中。创建线程后，每个线程都要有运行时的临时数据，就要求每个线程都要有自己私有的栈结构，而且还要有描述线程的用户级控制块，但是进程地址空间中只有一个主线程栈，其他线程的栈结构实际上是在pthread共享库里被库维护的，pthread库的id就是描述每个线程的各种结构、线程局部存储和线程栈的起始内存地址，只要拿到了pthread库的id就可以找到线程运行的用户级数据。只要在库创建了用户层的描述线程的属性和数据结构，对应的在内核PCB就会创建LWP，它们是1:1的关系，相当于用户级线程和内核级线程1:1对应，struct pthread包含对应的LWP，就如同struct FILE中包含文件描述符fd一般。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/16d5aafb3844436e88a1785fefd5c34d.png)


---

**轻量级进程ID与进程ID之间的区别：**
- 因为Linux下的轻量级进程是一个PCB，每个轻量级进程都有一个自己的轻量级进程ID（PCB中的PID），而同一个程序中的轻量级进程组成线程组，拥有一个共同的线程组ID。


**LWP与pthread_create创建的线程之间的关系：**
- pthread_create是一个库函数，功能是在用户态创建一个用户线程，而这个线程的运行调度是基于一个轻量级进程实现的。


**简述什么是LWP：**
- LWP是轻量级进程，在Linux下进程是资源分配的基本单位，线程是CPU调度的基本单位，而线程是使用进程PCB描述实现的，并且同一个进程的所有PCB共享一个虚拟地址空间，因此相较于传统进程更加轻量级。
---
