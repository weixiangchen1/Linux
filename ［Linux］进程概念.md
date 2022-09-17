![在这里插入图片描述](https://img-blog.csdnimg.cn/5ee93720470047a8b769945644e0cb03.png)

@[TOC](文章目录)

# 一、冯诺依曼体系结构

> **冯·诺依曼结构也称普林斯顿结构，是一种将程序指令存储器和数据存储器合并在一起的存储器结构。程序指令存储地址和数据存储地址指向同一个存储器的不同物理位置，因此程序指令和数据的宽度相同，数学家冯·诺依曼提出了计算机制造的三个基本原则，即采用二进制逻辑、程序存储执行以及计算机由五个部分组成（运算器、控制器、存储器、输入设备、输出设备），这套理论被称为冯·诺依曼体系结构。**
> 
![在这里插入图片描述](https://img-blog.csdnimg.cn/426d39c140d846768b0961dde81f52c3.png)
我们常见的计算机，如笔记本。我们不常见的计算机，如服务器，大部分都遵守冯诺依曼体系。截至目前，我们所认识的计算机，都是有一个个的硬件组件组成。

 - **输入设备**：键盘，鼠标，磁盘，网卡，显卡，话筒和摄像头等等。
 - **输出设备**：显示器，磁盘，网卡，显卡和音响等等。
 - **中央处理器(CPU)**：含有运算器和控制器等等。

**中央处理器的速度是非常快的**，主要用于处理一些加减乘除模运算、循环、逻辑判断等，而**输入和输出设备的速度是非常慢的**。

需要注意的是：
+ **冯诺依曼体系中的存储器指的是内存。**
+ **不考虑缓存情况，CPU只能对内存进行读写，不能访问输入或输出设备。**
+ **输入或输出设备也只能写入内存或者从内存中读取。**
+ <font color=red>**总而言之，所有设备都只能和内存打交道。**</font>
---

# 二、操作系统(OS)

**操作系统是一个任何计算机系统都包含一个基本的程序集合。**
操作系统包括：

 - **内核（进程管理，内存管理，文件管理，驱动管理）**
 - **其他程序（例如函数库，shell程序等等）**

设计操作系统的目的：
+ **与硬件交互，管理所有的软硬件资源**
+ **为用户程序（应用程序）提供一个良好的执行环境**

操作系统是如何进行管理的呢？

学校里各个角色的组织就和操作系统的管理很像，校长是如何管理整个学校成千上万的学生呢？
学生是被管理者，校长是管理者。校长并不会直接关注学生各种信息，也就是说管理者和被管理者并不直接打交道，但是我们的信息又是怎么被校长知道的，这时候就产生出辅导员这个角色，辅导员把我们的信息汇总成表，提交给校长，让校长做决策，校长就把学生的信息聚合起来，产生关联，并做决策，比如开除同学，分发奖学金等，**这本质上就是对数据结构的增删查改。**
**所以总而言之，校长管理学生的方式就是先描述学生的信息，然后再将学生的信息组织起来，并进行决策。这种方式被称为<font color=red>**先描述，再组织。**</font>**

同样的，操作系统管理软硬件也是这样，**先描述，再组织**。
1. **描述起来，用struct结构体**
2. **组织起来，用链表或其他高效的数据结构**
![在这里插入图片描述](https://img-blog.csdnimg.cn/e687dce6651a4e3aa9fa882830db796e.png)


**系统调用和库函数概念**：

 - 在开发角度，操作系统对外会表现为一个整体，但是会**暴露自己的部分接口**，供上层开发使用，这部分由操作系统提供的接口，叫做**系统调用**。
 - 系统调用在使用上，功能比较基础，对用户的要求相对也比较高，所以，有心的开发者可以**对部分系统调用进行适度封装**，从而**形成库**，有了库，就很有利于更上层用户或者开发者进行二次开发。
 - ---

# 三、进程
首先我们要明确，操作系统是如何管理进程的？**先描述，再组织。**
## 基本概念

 - 课本概念：程序的一个执行实例，正在执行的程序等。
 - 内核观点：担当分配系统资源（CPU时间，内存）的实体。

<br>

## 描述进程--PCB（process control block）
- 进程信息被放在一个叫做**进程控制块**的数据结构中，可以理解为进程属性的集合，称为**PCB（process control block）**，**Linux操作系统下的PCB是: task_struct。**
- **在Linux中描述进程的结构体叫做task_struct**。task_struct是Linux内核的一种数据结构，它会被**装载到RAM(内存)里并且包含着进程的信息**。
- 所有运行在系统里的进程都**以task_struct链表**的形式存在内核里。
<br>


### task_struct内容分类
- **标示符**: 描述本进程的唯一标示符，用来区别其他进程。
- **状态**: 任务状态，退出代码，退出信号等。
- **优先级**: 相对于其他进程的优先级。
- **程序计数器**: 程序中即将被执行的下一条指令的地址。
- **内存指针**: 包括程序代码和进程相关数据的指针，还有和其他进程共享的内存块的指针。
- **上下文数据**: 进程执行时处理器的寄存器中的数据。
-  **I／O状态信息**: 包括显示的I/O请求,分配给进程的I／O设备和被进程使用的文件列表。
- **记账信息**: 可能包括处理器时间总和，使用的时钟数总和，时间限制，记账号等。
- **其他信息**。

## 查看进程

进程的信息可以通过 <kbd> /proc </kbd> 系统文件夹查看。
![在这里插入图片描述](https://img-blog.csdnimg.cn/293ab0c192cd4fe497968c7eef4bc187.png)

大多数进程信息同样可以使用top和ps这些用户级工具来获取
![在这里插入图片描述](https://img-blog.csdnimg.cn/498239661707467daebc5aa7a538fc2c.png)
<br>
## 通过系统调用获取进程标示符
- 进程id（PID），getpid()用于获取进程id。
- 父进程id（PPID），getppid()用于获取父进程id。

![在这里插入图片描述](https://img-blog.csdnimg.cn/28ea06bfe3ed4f3e9200b613ee18e667.png)

```c
#include <stdio.h>
#include <unistd.h>                                                                                          

int main()
{
     printf("pid: %d\n", getpid());
     printf("ppid: %d\n", getppid());
 
     return 0;
}

```
![在这里插入图片描述](https://img-blog.csdnimg.cn/f0b4a73de1df464f951d20665b8fb209.png)
<br>
## 通过系统调用创建进程-fork初识
- 运行 <kbd> man fork  </kbd>认识fork
- fork有两个返回值
- 父子进程代码共享，数据各自开辟空间，私有一份（采用写时拷贝）

![在这里插入图片描述](https://img-blog.csdnimg.cn/e6535cd7d3694df588092c7933ee961b.png)

- fork 是创建子进程，也就意味着 fork 之后，这个子进程才能被创建成功，父进程和子进程都要运行相同的代码，fork 之后，父进程和子进程谁先运行，不是由 fork 决定的，而是由**系统的调度优先级**决定的。

- fork 创建子进程之后，父子进程共享一份代码，当子进程或父进程要修改代码和数据时，**操作系统会采用写时拷贝，生成两份相同的代码和数据**，再由子进程或父进程来修改代码和数据，这样就能达到互不干扰的效果，所以在操作系统中，**进程是具有独立性的**。
- fork 在创建子进程之后，操作系统就多了一个进程，这个子进程是由父进程为模板创建的，会**继承程序的代码和数据**，**内核数据结构task_struct也会以父进程为模板，初始化子进程的task_struct**。



fork有两个返回值，通过一个程序来验证一下:

```cpp
#include <stdio.h>
#include <unistd.h>

int main()
{
    int result = fork();
    printf("pid: %d, result: %d\n", getpid(), result);
    sleep(1);                                                                                                
                                                                                                      
    return 0;                    
} 
```
![在这里插入图片描述](https://img-blog.csdnimg.cn/b98a60d0ac0449bfbddb76e17b2fdd4c.png)

 - 父进程的fork会返回**子进程的PID值**，子进程的fork会**返回0**.所以通常情况下，**fork可以使用if来进行分流**。

![在这里插入图片描述](https://img-blog.csdnimg.cn/48f95dec716542a3a9ab373e9aa68776.png)
## 进程状态
### Linux内核源代码描述进程状态

```c
/*
 * The task state array is a strange "bitmap" of
 * reasons to sleep. Thus "running" is zero, and
 * you can test for combinations of others with
 * simple bit tests.
*/
static const char * const task_state_array[] = {
"R (running)", /* 0 */
"S (sleeping)", /* 1 */
"D (disk sleep)", /* 2 */
"T (stopped)", /* 4 */
"t (tracing stop)", /* 8 */
"X (dead)", /* 16 */
"Z (zombie)", /* 32 */
};
```

 * **R运行状态（running）**: 并不意味着进程一定在运行中，它表明进程要么是在运行中要么在运行队列里。
 ![在这里插入图片描述](https://img-blog.csdnimg.cn/8c86defda15d49bb838f410ea4b48dd0.png)
 * **S睡眠状态（sleeping)**: 意味着进程在等待事件完成（这里的睡眠有时候也叫做可中断睡眠（interruptible sleep））。
![在这里插入图片描述](https://img-blog.csdnimg.cn/8489b2d0bc63464c97a962118f047c33.png)
![在这里插入图片描述](https://img-blog.csdnimg.cn/33ea165cf77e4be9a7b9b0d10fedbdf4.png)


 * **D磁盘休眠状态**（Disk sleep）：也叫不可中断睡眠状态（uninterruptible sleep），在这个状态的进程通常会等待IO的结束。
 * **T停止状态（stopped）**： 可以通过发送 SIGSTOP 信号给进程来停止（T）进程。这个被暂停的进程可以通过发送 SIGCONT 信号让进程继续运行。
![在这里插入图片描述](https://img-blog.csdnimg.cn/ab3c26d34aaf4c2f8c5711ed57e499ab.png)
![在这里插入图片描述](https://img-blog.csdnimg.cn/5ce5bf30054b46da858a17a8506089a8.png)


 * **X死亡状态（dead）**：这个状态只是一个返回状态，你不会在任务列表里看到这个状态。

## 进程状态查看

```bash
ps aux / ps axj 命令
```

![在这里插入图片描述](https://img-blog.csdnimg.cn/28f1ea4c31c24f5299b749947ef8ca2b.png)
![在这里插入图片描述](https://img-blog.csdnimg.cn/41175629f94a467a87ed1607849c3e33.png)
## Z(zombie)-僵尸进程

 - 僵死状态（Zombies）是一个比较特殊的状态。当进程退出并且父进程
没有读取到子进程退出的返回代码时就会产生僵死(尸)进程
- 僵死进程会以终止状态保持在进程表中，并且会一直在等待父进程读取退出状态代码。
- 所以，只要子进程退出，父进程还在运行，但父进程没有读取子进程状态，子进程进入Z状态

下面我们用一个例子来验证一下僵尸进程：
```c
#include <iostream>
#include <cstdlib>
#include <unistd.h>
using namespace std;
// 验证僵尸进程
int main()
{
    pid_t id = fork();
    if(id == 0){
        //child
       while(true){
         cout << "child is running!" << endl;
         sleep(2);
       }
    }
    else{
        //parent
        cout << "parent do nothing!" << endl;
        sleep(50);
    }
                                                                                                                 
    return 0;
}

```
要验证该程序，需要启动三个终端：

 - 第一个终端
编写监控命令行脚本：
![在这里插入图片描述](https://img-blog.csdnimg.cn/f20ea9cb4b4b42d9ae279f0904182e8b.png)

- 第二个终端
启动对应的进程：
![在这里插入图片描述](https://img-blog.csdnimg.cn/f61c99ccb65d4488bb42832706266285.png)
- 第三个终端
用信号杀死子进程
![在这里插入图片描述](https://img-blog.csdnimg.cn/40c3097a26b14357b95f177bdc03ad63.png)
结果：
![在这里插入图片描述](https://img-blog.csdnimg.cn/83fb8fb2a6d9400a9be2ee4ced1029ea.png)


## 僵尸进程的危害
- 维护退出状态本身就是要用数据维护，也属于进程基本信息，所以保存在task_struct(PCB)中，如果进程一直处于僵尸状态，PCB就要一直被维护。
- 一个父进程创建了很多子进程，就是不回收，就会造成内存资源的浪费，而且会造成资源泄露。

## 孤儿进程

**在操作系统领域中，孤儿进程指的是在其父进程执行完成或被终止后仍继续运行的一类进程**。这些孤儿进程将被init进程(进程号为1)所收养，并由init进程对它们完成状态收集工作。

验证孤儿进程：

```c
#include <iostream>                                                                                           
#include <cstdlib>    
#include <unistd.h>    
using namespace std;    
    
// 验证孤儿进程    
int main()    
{    
    pid_t id = fork();    
    if(id == 0){    
        // child    
      while(true){    
          cout << "child is running!" << endl;    
          sleep(2);    
      }    
    }    
    else{    
        //parent    
      cout << "parent is running!" << endl;    
      sleep(10);// 10s后父进程终止    
      exit(1);    
    }    
    
    
  return 0;    
}    
```
结果：
![在这里插入图片描述](https://img-blog.csdnimg.cn/b8aa295c5e0a4100b9161bde4b2c144c.png)

