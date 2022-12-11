![在这里插入图片描述](https://img-blog.csdnimg.cn/5ee93720470047a8b769945644e0cb03.png)

@[TOC](文章目录)

# 进程创建
## fork()函数初识
在linux中，fork函数是非常重要的函数，它从已存在进程中创建一个新进程。新进程为子进程，而原进程为父进程。**fork()函数给父进程返回子进程的PID，给子进程返回0，出错返回-1。**

```c
#include <unistd.h>
pid_t fork(void);
返回值：子进程中返回0，父进程返回子进程id，出错返回-1
```

**当进程在调用fork()函数时，内核会做以下的事：**

- **给子进程分配新的内存块和内核数据结构**
- **子进程以父进程为模板，拷贝父进程部分代码和数据**
- **将子进程添加到系统进程列表中**


当一个进程调用fork之后，就有两个二进制代码相同的进程。利用if分流，每个进程都将可以开始执行自己的代码，看如下程序：
```c
#include <stdio.h>      
#include <unistd.h>      
      
int main()      
{      
    pid_t id = fork();      
      
    if(id == 0){      
        // 子进程      
        printf("I am child process, pid:%d\n", getpid());      
    }      
    else{      
        // 父进程      
        printf("I am parent process, pid:%d\n", getpid());                                                                                                     
    }                                                                                                                                         
                                                                                                                                              
                                                                                                                                              
    return 0;                                                                                                                                 
}
```
运行结果：

![在这里插入图片描述](https://img-blog.csdnimg.cn/82067fa42a624ac3ac2e02d62404d574.png)

---

## 写时拷贝
**在我们创建子进程时，子进程会拷贝父进程的部分代码和数据。父子进程的代码一般是共享的，所以，所有只读的数据一般来说只有一份，父子进程指向同一块空间，因为操作系统维护一份代码的成本是最低的。此时，如果父子进程有一方要修改代码或数据，就会发生写时拷贝，操作系统就会再开辟一块空间，让父子进程各自拥有一份副本。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/ef5a35a74b4e48ebabc5fb88097391e2.png)

---

## fork()用法
- 一个父进程希望复制自己，使父子进程同时执行不同的代码段。例如，父进程等待客户端请求，生成子进程来处理请求。
- 一个进程要执行一个不同的程序。例如子进程从fork返回后，调用exec函数。


---

# 进程终止
## 进程退出场景

 - 代码运行完毕，结果正确
 - 代码运行完毕，结果不正确
 - 代码异常终止

**代码运行完毕，main函数的return返回值返回0为代码执行正确，非0为代码执行错误，代码异常终止指程序崩溃，退出码就没有了意义。**

程序退出码：

![在这里插入图片描述](https://img-blog.csdnimg.cn/f0f2f434f77845c584e2b57db5d20b5d.png)

---
## 进程常见退出方法
**正常终止**（通过<kbd>echo $?</kbd>查看退出码）

 1. **通过main函数return退出码**
 2. **调用exit()**
 3. **调用_exit()**

**异常终止**

- **ctrl+c，ctrl+\\  信号终止**



---



### 通过main函数return退出码

```c
[cwx@VM-20-16-centos process_control]$ cat mytest.c 
#include <stdio.h>
int main()
{
    printf("hello world\n");

    return 2;
}

运行结果：
[cwx@VM-20-16-centos process_control]$ ./mytest 
hello world
[cwx@VM-20-16-centos process_control]$ echo $?
2

```
**main函数中return返回退出码，可以通过echo $?查看退出码。
return n退出相当于执行exit(n)，因为调用main的运行时函数会将main的返
回值当做 exit的参数**

---

## 调用exit()

 - **函数用法：**
 - **#include <unistd.h>
void exit(int status);**

```c
[cwx@VM-20-16-centos process_control]$ cat mytest.c 
#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("hello world\n");

    exit(5);
}

运行结果：
[cwx@VM-20-16-centos process_control]$ ./mytest 
hello world
[cwx@VM-20-16-centos process_control]$ echo $?
5

```

---

## 调用_exit()
 - **函数用法：**
 - **#include <unistd.h>
void _exit(int status);
参数：status 定义了进程的终止状态，父进程通过wait来获取该值**
```c
[cwx@VM-20-16-centos process_control]$ cat mytest.c 
#include <stdio.h>
#include <unistd.h>

int main()
{
    printf("hello world");

    _exit(6);
}

运行结果：
[cwx@VM-20-16-centos process_control]$ ./mytest 
[cwx@VM-20-16-centos process_control]$ echo $?
6

```

**我们可以看出调用_exit()函数并不会在终端上打印出"hello world"，这是因为调用printf()函数，如果打印的内容不加\n，内容不会立即被打印到显示器上，会先暂存在用户级缓冲区中，而_exit()函数调用后，不会进行刷新用户级缓冲区等后续进程结束的后续收尾工作，直接进程终止了，所以看不到打印的内容。**

---

**说明：**

 1. main函数return退出码，表示进程退出，非main函数使用exit退出进程并返回退出码
 2. exit可以在任意地方调用，都代表进程退出，参数时退出码
 3. _exit退出进程，强制终止进程，不进行进程刷新用户级缓冲区等后续进程结束的后续收尾工作
 4. return返回值或exit会要求系统进行缓冲区刷新
![在这里插入图片描述](https://img-blog.csdnimg.cn/093041ffb9b74de99a158634b05ab4ec.png)
---

# 进程等待
## 进程等待的必要性
- 当子进程退出时，父进程如果不管不顾，该子进程就会变成僵尸进程，一旦成为僵尸进程，不仅会产生内存泄漏问题，而且我们无法用"kill -9"杀死僵尸进程。
- 父子进程运行时，父进程需要知道子进程运行的任务完成情况，比如子进程运行的结果对或不对，是否正常退出，父进程可以通过进程等待的方式，回收子进程资源，获得子进程的退出信息。

---

## 进程等待的方法
### wait方法

```c
#include<sys/types.h>
#include<sys/wait.h>
pid_t wait(int*status);

返回值：
成功返回被等待进程pid，失败返回-1。
参数：
输出型参数，获取子进程退出状态,不关心则可以设置成为NULL
```
**测试代码：**

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    pid_t id = fork();
    if(id == 0){
        //child
        int cnt = 3;
        while(cnt){
            printf("I am a child process\n");
            cnt--;
            sleep(1);
        }
        exit(1);
    }
    //parent
    sleep(5);
    int status = 0;
    pid_t res = wait(&status);
    if(res > 0){
        //父进程等待成功，获取子进程退出信息
        printf("parent wait success, status exit code: %d, status exit single: %d, core dump: %d\n", (status>>8)&0xFF, status&0x7F, status&0x80);
    }
    else{
        //父进程等待失败
        printf("parent wait failed\n");
    }
    sleep(3);
    
    return 0;
}

```
问题：为什么我们在命令行上的指令可以通过echo $?查看退出码呢，操作系统是如何获取的呢？
![在这里插入图片描述](https://img-blog.csdnimg.cn/cf37c005cf214dfa8608dcc22cfaf6f7.png)

答：bash是命令行启动所有进程的父进程，bash通过wait方法得到子进程的退出结果，所以echo$?可以查到子进程的退出码。

---



### waitpid方法

```c
#include<sys/types.h>
#include<sys/wait.h>
pid_ t waitpid(pid_t pid, int *status, int options);

返回值：
当正常返回的时候waitpid返回收集到的子进程的进程ID；
如果设置了选项WNOHANG,而调用中waitpid发现没有已退出的子进程可收集,则返回0；
如果调用中出错,则返回-1,这时errno会被设置成相应的值以指示错误所在；

参数：
pid：Pid=-1,等待任一个子进程。与wait等效。
Pid>0.等待其进程ID与pid相等的子进程。
status:
WIFEXITED(status): 若为正常终止子进程返回的状态，则为真。（查看进程是否是正常退出）
WEXITSTATUS(status): 若WIFEXITED非零，提取子进程退出码。（查看进程的退出码）
options:
WNOHANG: 若pid指定的子进程没有结束，则waitpid()函数返回0，不予以等待。若正常结束，则返回该子进
程的ID。
```
测试代码（阻塞等待）：

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{   
    pid_t id = fork();
    if(id == 0)
    {
        int cnt = 10;
        while(cnt--)
        {
            printf("child process running, pid: %d, ppid: %d\n", getpid(), getppid());
            sleep(1);
        }
        exit(2);
    }
    //父进程阻塞等待
    // pid_ t waitpid(pid_t pid, int *status, int options);
    int status = 0;
    //pid_t ret = waitpid(-1, &status, 0); // pid=-1相当于wait
    pid_t ret = waitpid(id, &status, 0); // 等待指定id的进程，options为0为阻塞等待
    if(ret > 0)
    {
        if(WIFEXITED(status))
        {
            printf("exit code: %d\n", WEXITSTATUS(status));
        }
        else
        {
            printf("get a signal\n");
        }
    }
    else
    {
        printf("parent wait failed\n");
    }


    return 0;
}
```
测试代码（非阻塞等待）：

```c
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

int main()
{   
    pid_t id = fork();
    if(id == 0)
    {
        int cnt = 10;
        while(cnt--)
        {
            printf("child process running, pid: %d, ppid: %d\n", getpid(), getppid());
            sleep(1);
        }
        exit(2);
    }

    // 父进程非阻塞等待
    int status = 0;
    while(1)
    {
        //WNOHANG: 若pid指定的子进程没有结束，则waitpid()函数返回0，不予以等待。若正常结束，则返回该子进程的ID
        pid_t ret = waitpid(id, &status, WNOHANG);
        if(ret == 0)
        {
            // 子进程未退出，父进程waitpid成功，父进程重复等待子进程退出
            printf("parent do himself\n");
            sleep(1);
        }
        else if(ret > 0)
        {
            // 子进程退出，父进程waitpid成功
            if(WIFEXITED(status))
            {
                printf("exit code: %d\n", WEXITSTATUS(status));
                break;
            }
            else
            {
                printf("get a signal\n");
                break;
            }
        }
        else
        {
            // 父进程等待失败
            printf("parent wait failed\n");
        }
    }

    return 0;
}
```

 - 如果子进程已经退出，调用wait/waitpid时，wait/waitpid会立即返回，并且释放资源，获得子进程退出信息。
 - 如果在任意时刻调用wait/waitpid，子进程存在且正常运行，则进程可能阻塞。
 - 如果不存在该子进程，则立即出错返回。

---

## 阻塞等待和非阻塞等待

- 阻塞等待:父进程在等待子进程退出时，父进程不可做其他操作，直到子进程退出，这叫做阻塞等待。
- 非阻塞等待:父进程在等待子进程退出时，父进程可以做自己的事情，不会被阻塞，直到子进程退出，这叫做非阻塞等待。
-  阻塞等待的本质是进程的PCB被插入等待队列，并将该进程的状态设为S状态(睡眠状态)。
---

## 获取子进程status

 - wait和waitpid，都有一个status参数，该参数是一个输出型参数，由操作系统填充。
 - 如果传递NULL，表示不关心子进程的退出状态信息。
 - 如果传递一个int变量地址，操作系统会根据该参数，将子进程的退出信息反馈给父进程。
 - status不能简单的当作整形来看待，可以当作位图来看待，具体细节如下图（只研究status低16比特位）：


![在这里插入图片描述](https://img-blog.csdnimg.cn/9cda528eb9d24f1bb74473d4e7f65f22.png)

```c
获取退出状态退出码: (status>>8)&0xFF
获取终止信号: status&0x7F
获取core dump标志位: (status>>7)&1
```
---

# 进程替换
## 替换原理
- 用fork创建子进程后执行的是和父进程相同的程序(但有可能执行不同的代码分支),子进程往往要调用一种exec函数以执行另一个程序。当进程调用一种exec函数时,该进程的用户空间代码和数据完全被新程序替换,从新程序的启动
例程开始执行。调用exec并不创建新进程,所以调用exec前后该进程的id并未改变。
- 进程内容不发生改变，替换进程的代码和数据的操作，叫做进程替换。



![在这里插入图片描述](https://img-blog.csdnimg.cn/79fac02c999c4967928f4e3ba76c753f.png)

---
## 替换函数
总共有六种以exec开头的函数，统称exec函数:

```c
#include <unistd.h>`

int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *path, const char *arg, ...,char *const envp[]);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execve(const char *path, char *const argv[], char *const envp[]);
```

 - 这些函数如果调用成功则加载新的程序从启动代码开始执行,不再返回。
 - 如果调用出错则返回-1，所以exec函数只有出错的返回值而没有成功的返回值。

---

## 命名理解

 - l(list) : 表示参数采用列表 
 - v(vector) : 参数用数组
 - p(path) : 有p自动搜索环境变量PATH
 - e(env) : 表示自己维护环境变量

![在这里插入图片描述](https://img-blog.csdnimg.cn/af22061876534af2b9646cadff1f2c82.png)
测试代码：

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    if(fork() == 0){
         // child
       
         //测试execvp
         char *argv[] = {
               "ps", "ajx", NULL 
         };
         execvp("ps", argv);
 
         // 测试execv
         //char *argv[] = {
         //    "ls", "-a", "-l", "-d", NULL
         //};
         //execv("/usr/bin/ls", argv);
  
         // 测试execlp
         //execlp("ls", "ls", "-l", "-a", "-i", NULL);
         
         // 测试execl
         //execl("/usr/bin/ls", "ls", "-l", "-a", NULL);
         exit(1); // 替换失败返回信号1
    }
     
    // parent
    waitpid(-1, NULL, 0);
    printf("wait success\n");


    return 0;
}

```
事实上,只有execve是真正的系统调用,其它五个函数最终都调用 execve,所以execve在man手册 第2节,其它函数在man手册第3节。这些函数之间的关系如下图所示。

![在这里插入图片描述](https://img-blog.csdnimg.cn/107ae106a85d4e45ab49fa49249bdbbf.png)

