![在这里插入图片描述](https://img-blog.csdnimg.cn/5ee93720470047a8b769945644e0cb03.png)

@[TOC](文章目录)
# 进程间通信介绍
进程是具有独立性的，进程之间在保持各自独立性时，可能会存在特定的协同工作的场景，比如一个进程要把数据交付给另一个进程进行处理，这种场景下就需要进程间通信。要实现进程间通信，操作系统就要设计适合系统的通信方式，然而进程是具有独立性的，要交互数据，成本非常高，因为一个进程是获取不到另一个进程的资源的，如果要实现进程间通信就必须让不同的进程可以看到同一份资源。
**所以说，进程间通信的本质是由操作系统提供一份通信进程可以看到的公共资源，这份资源可能以文件方式提供，也可能队列的方式，也可能是原始的内存块。**

---


## 进程间通信的目的

 - **数据传输**：一个进程需要将它的数据发送给另一个进程。
 - **资源共享**：多个进程之间共享同样的资源。
 - **通知事件**：一个进程需要向另一个或一组进程发送消息，通知它发生了某种事件（如进程终止时要通知父进程）。
 - **进程控制**：有些进程希望完全控制另一个进程的执行（如Debug进程），此时控制进程希望能够拦截另一个进程的所有陷入和异常，并能够及时知道它的状态改变。

---

## 进程间通信发展和分类
**管道**
- 匿名管道
- 命名管道

**System V进程间通信**
- System V 消息队列
- System V 共享内存
- System V 信号量

**POSIX进程间通信**
- 消息队列 
- 共享内存
- 信号量
- 互斥量
- 条件变量
- 读写锁


---

# 管道
## 什么是管道
- 管道是Unix中最古老的进程间通信的形式。
- 我们把从一个进程连接到另一个进程的一个数据流称为一个“管道”。
- 管道是一个只能单向通信的通信信道。

**管道使用：**

```c
[cwx@VM-20-16-centos pipe]$ cat mytest.c | wc -l
13
[cwx@VM-20-16-centos pipe]$ 
```
![在这里插入图片描述](https://img-blog.csdnimg.cn/a047b73fbe1d42c3b1f66646628a7094.png)

---

## 匿名管道
父进程调用fork()创建子进程，子进程以父进程为模板创建PCB，struct files_struct结构，并将父进程的数据拷贝到自己的数据结构里，父子进程就可以通过struct files_struct指向同一个文件内核缓冲区，看到了同一份资源，就可以实现进程间通信。

**接口介绍：**

```c
#include <unistd.h>
功能:创建一个匿名管道
原型
int pipe(int fd[2]);
参数
fd：文件描述符数组,其中fd[0]表示读端, fd[1]表示写端
返回值:成功返回0，失败返回错误代码
```
---
**通过代码实现父子进程间通信：**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// pipefd[2]：输出型参数
// 创建成功返回0，错误返回-1
// int pipe(int pipefd[2]);

int main()
{   
    int pipe_fd[2] = {0};
    if(pipe(pipe_fd) != 0){
        perror("create pipe");
        return 1;
    }

    // 父进程读取，子进程写入
    if(fork() == 0){
        // 子进程
        close(pipe_fd[0]); // 关闭读端

        const char* msg = "hello world ";
        while(1){
            write(pipe_fd[1], msg, strlen(msg));
        }

        exit(0);
    }

    // 父进程
    close(pipe_fd[1]); // 关闭写端
    while(1){
        char buffer[64] = {0};
        // read返回值为0，表示子进程关闭文件写端
        ssize_t s = read(pipe_fd[0], buffer, sizeof(buffer)-1);
        if(s == 0){
            printf("child process quit...\n");
            break;
        }
        else if(s > 0){
            buffer[s] = 0;
            printf("child process: %s\n", buffer);
        }
        else{
            printf("read error\n");
            break;
        }
    }

    return 0;
}

运行结果：
[cwx@VM-20-16-centos pipe]$ ./pipe_process 
child process: hello world 
child process: hello world 
child process: hello world 
child process: hello world 
child process: hello world 
child process: hello world 
child process: hello world 
^C
[cwx@VM-20-16-centos pipe]$
```



---

**用fork来共享管道原理：**
![在这里插入图片描述](https://img-blog.csdnimg.cn/1eb89b8c23374b389c4d3b99b9758b75.png)
**站在文件描述符角度-深度理解管道：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/304bf345e7cb48e786d12d918f953e1b.png)

---

## 匿名管道通信的四种情况
**1、读端暂停读数据或者读端读数据比较慢，写端等待读端(write调用阻塞)。
2、写端暂停写数据或者写端写数据比较慢，读端等待写端(read调用阻塞)。
3、读端关闭，写端收到操作系统发送的SIGPIPE信号终止进程。
4、写端关闭，读端读完匿名管道内部数据后，读到0，表示读到文件结尾。**

---
**验证读端关闭的情况：**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// pipefd[2]：输出型参数
// 创建成功返回0，错误返回-1
// int pipe(int pipefd[2]);

int main()
{   
    int pipe_fd[2] = {0};
    if(pipe(pipe_fd) != 0){
        perror("create pipe");
        return 1;
    }

    // 父进程读取，子进程写入
    if(fork() == 0){
        // 子进程
        close(pipe_fd[0]); // 关闭读端

        const char* msg = "hello world ";
        while(1){
            write(pipe_fd[1], msg, strlen(msg));
            sleep(3);
        }
        exit(0);
    }

    // 父进程
    close(pipe_fd[1]); // 关闭写端
    while(1){
        char buffer[64] = {0};
        ssize_t s = read(pipe_fd[0], buffer, sizeof(buffer)-1);
        if(s == 0){
            printf("child process quit...\n");
            break;
        }
        else if(s > 0){
            buffer[s] = 0;
            printf("child process: %s\n", buffer);
            break;
        }
        else{
            printf("read error\n");
            break;
        }
    }
    close(pipe_fd[0]); // 关闭读端
    // 进程等待获取退出信号
    int status = 0;
    waitpid(-1, &status, 0);
    printf("exit signal: %d\n", status&0x7F);

    return 0;
}


运行结果：
[cwx@VM-20-16-centos pipe]$ ./pipe_process 
child process: hello world 
exit signal: 13
```

**读端在读取一条信息后，立刻退出循环，关闭读端，父进程waitpid()进程等待获取子进程退出的退出码，exit signal: 13，<kbd>kill -l</kbd>查看13号信号为SIGPIPE。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/c92ab0faadc8458d81e697990ef9605f.png)

---

**验证写端关闭的情况：**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// pipefd[2]：输出型参数
// 创建成功返回0，错误返回-1
// int pipe(int pipefd[2]);

int main()
{   
    int pipe_fd[2] = {0};
    if(pipe(pipe_fd) != 0){
        perror("create pipe");
        return 1;
    }

    // 父进程读取，子进程写入
    if(fork() == 0){
        // 子进程
        close(pipe_fd[0]); // 关闭读端

        const char* msg = "hello world ";
        while(1){
            write(pipe_fd[1], msg, strlen(msg));
            sleep(3);
            break;
        }
        close(pipe_fd[1]);
        exit(0);
    }

    // 父进程
    close(pipe_fd[1]); // 关闭写端
    while(1){
        char buffer[64] = {0};
        // read返回值为0，表示子进程关闭文件写端
        ssize_t s = read(pipe_fd[0], buffer, sizeof(buffer)-1);
        if(s == 0){
            printf("child process quit...\n");
            break;
        }
        else if(s > 0){
            buffer[s] = 0;
            printf("child process: %s\n", buffer);
        }
        else{
            printf("read error\n");
            break;
        }
    }

    return 0;
}

运行结果：
[cwx@VM-20-16-centos pipe]$ ./pipe_process 
child process: hello world 
child process quit...
```
**子进程写入字符串三秒后退出循环，关闭写端。父进程read读取子进程写入的数据，之后read返回值为0，表示子进程关闭文件写端。**

---

## 管道容量
**子进程定义count变量，每次向管道写入一个字符，count++，父进程等待子进程写入，测试管道可以容纳多少bit的字符。**

**测试代码：**

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{   
    int pipe_fd[2] = {0};
    if(pipe(pipe_fd) != 0){
        perror("create pipe");
        return 1;
    }

    // 父进程读取，子进程写入
    if(fork() == 0){
        // 子进程
        close(pipe_fd[0]); // 关闭读端

        int count = 0;
        while(1){
            write(pipe_fd[1], "a", 1);
            count++;
            printf("count: %d\n", count);
        }
        exit(0);
    }

    // 父进程
    close(pipe_fd[1]); // 关闭写端
    while(1);
    return 0;
}

运行结果：
......
count: 65530
count: 65531
count: 65532
count: 65533
count: 65534
count: 65535
count: 65536
^C
[cwx@VM-20-16-centos test_pipe]$ 
```
**当count累加到65536时，子进程写端不再写入数据，通过以上代码可以测试出管道的容量为65536bytes=64kb。**

**通过<kbd>man 7 pipe</kbd>查看管道描述：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/46b5132c83e74c14b25d7777198b62e4.png)

- **当要写入的数据量不大于PIPE_BUF时，linux将保证写入的原子性。**
- **当要写入的数据量大于PIPE_BUF时，linux将不再保证写入的原子性。**

也就是说当管道满的时候，写端停止写入，当读端一次性读取的数据少于PIPE_BUF时，写端仍然暂停写入，当读端一次性读取的数据大于PIPE_BUF时，写端才会解除阻塞，继续写入。

---
## 管道通信的五个特点

 1. **管道是半双工的，数据只能向一个方向流动，管道是一个只能单向通信的信道，如需双向通信，需要建立两个管道。**
 2. **管道提供流式服务，管道式面向字节流的。**
 3. **只能用于具有共同祖先的进程（具有亲缘关系的进程）之间进行通信；通常，一个管道由一个进程创建，然后该进程调用fork，此后父、子进程之间就可应用该管道。**
 4. **管道自带同步互斥机制和原子性写入。**
 5. **进程退出，管道释放，所以管道的生命周期随进程。**


---


# 命名管道

 - **管道应用的一个限制就是只能在具有共同祖先的进程间通信。**
 - **如果我们想在不相关的进程之间交换数据，可以使用FIFO文件来做这项工作，它经常被称为命名管道。命名管道是一种特殊类型的文件**

**创建命名管道：**

- **命名管道可以从命令行上创建，命令行方法是使用下面这个命令：**

```c
$ mkfifo filename
```

- **命名管道也可以从程序里创建，相关函数有：**

```c
#include <sys/types.h>
#include <sys/stat.h>

int mkfifo(const char *filename, mode_t mode);

mkfifo() 创建一个名为 pathname 的 FIFO 特殊文件。 mode 指定 FIFO 的权限。 
它由进程的umask以通常的方式修改：创建文件的权限为（mode & ~umask）。
```
**测试代码：**

```c
umask(0); 
if(mkfifo("./fifo", 0666) < 0){
    perror("mkfifo");
}

运行结果：
[cwx@VM-20-16-centos fifo]$ ll
total 1
prw-rw-rw- 1 cwx cwx    0 Aug  7 17:26 fifo
```
- **命名管道的大小是0，这是因为为了效率，命名管道的数据不是刷新到磁盘上。**
- **创建出命名管道之后，通信双方可以按照文件操作进行通信。**
- **命名管道也是面向字节流的，所以实际上信息传输的时候，是需要通信双方制定协议的。**



---


## 匿名管道和命名管道的区别

 - **匿名管道由pipe函数创建并打开。**
 - **命名管道由mkfifo函数创建，打开用open。**
 - **FIFO（命名管道）与pipe（匿名管道）之间唯一的区别在它们创建与打开的方式不同，一但这些工作完成之后，它们具有相同的语义。**
 - **pipe（匿名管道）文件没有名字，通过父子进程继承的方式，访问到同一份资源，不需要名字来标识同一份资源，FIFO（命名管道）文件需要有名字，为了保证让不同进程访问同一份资源，需要用名字来标识。**


---

## 命名管道的打开规则
 - **如果当前打开操作是为读而打开FIFO时
O_NONBLOCK disable：阻塞直到有相应进程为写而打开该FIFO
O_NONBLOCK enable：立刻返回成功**
- **如果当前打开操作是为写而打开FIFO时
O_NONBLOCK disable：阻塞直到有相应进程为读而打开该FIFO
O_NONBLOCK enable：立刻返回失败，错误码为ENXIO**


---


## 命名管道实现server&client通信
**通过命名管道实现server进程和client进程间通信，client进程给server进程传输数据，并结合进程替换exec函数，实现打印目录文件等操作。**

```c
[cwx@VM-20-16-centos fifo]$ ll
total 16
-rw-rw-r-- 1 cwx cwx  555 Jul 25 16:19 client.c
-rw-rw-r-- 1 cwx cwx  206 Jul 25 16:18 comm.h
-rw-rw-r-- 1 cwx cwx  139 Jul 25 15:50 Makefile
-rw-rw-r-- 1 cwx cwx 1452 Aug  7 17:23 server.c
```

**Makefile文件：**

```c
.PHONY:all
all: client server

client:client.c
	gcc -o $@ $^
server:server.c
	gcc -o $@ $^

.PHONY:clean
clean:
	rm -rf client server fifo
```
**comm.h头文件：**

```c
#pragma once

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#define MY_FIFO "./fifo"
```

**server.c：**

```c
#include "comm.h"

int main()
{
    // 创建命名管道
    umask(0); 
    if(mkfifo(MY_FIFO, 0666) < 0){
        perror("mkfifo");
    }
    
    // 文件操作
    int fd = open(MY_FIFO, O_RDONLY);
    if(fd < 0){
        perror("open");
        return 1;
    }
    
    // 读写操作
    while(1){
        char buffer[64] = {0};
        ssize_t s = read(fd, buffer, sizeof(buffer)-1);

        if(s > 0){
            // 读取成功
            buffer[s] = 0;
            if(strcmp(buffer, "show") == 0){
                if(fork() == 0){
                   execl("/usr/bin/ls", "ls", "-a", "-l", NULL);
                   exit(1);
                }
                waitpid(-1, NULL, 0);
            }
            else if(strcmp(buffer, "run") == 0){
                if(fork() == 0){
                  execl("/usr/bin/sl", "sl", NULL);
                  exit(1);
                }
                waitpid(-1, NULL, 0);
            }
            else{
                printf("cilent# %s\n", buffer);
            }
        }
        else if(s == 0){
            // 读取退出
            printf("client quit...\n");
            break;
        }
        else{
            // 读取错误
            perror("read");
            break;
        }
    }

    close(fd);
    return 0;
}
```

**client.c：**

```c
#include "comm.h"

int main()
{
    int fd = open(MY_FIFO, O_WRONLY);
    if(fd < 0){
        perror("open");
        return 1;
    }

    // 读写操作
    while(1){
        printf("请输入# ");
        fflush(stdout);
        char buffer[64] = {0};
        // 将数据标准输入到client内部
        ssize_t s = read(0, buffer, sizeof(buffer)-1);
        if(s > 0){
            buffer[s-1] = 0; // 去掉回车

            write(fd, buffer, strlen(buffer));
        }
    }

    close(fd);

    return 0;
}
```


**运行结果：**
![在这里插入图片描述](https://img-blog.csdnimg.cn/550962e593304eeabc16e1b5ea788014.png)

---
