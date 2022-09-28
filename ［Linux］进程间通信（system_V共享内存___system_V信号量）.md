![在这里插入图片描述](https://img-blog.csdnimg.cn/5ee93720470047a8b769945644e0cb03.png)

@[TOC](文章目录)

# system V共享内存
共享内存区是最快的IPC形式。一旦这样的内存映射到共享它的进程的地址空间，这些进程间数据传递不再涉及到内核，换句话说是进程不再通过执行进入内核的系统调用来传递彼此的数据，大大提高了效率。

---

## 共享内存示意图
![在这里插入图片描述](https://img-blog.csdnimg.cn/da2f05a791c7465983aeb20980ce8f7c.png)

---

## 共享内存数据结构

```c
struct shmid_ds {
	 struct ipc_perm shm_perm; /* operation perms */
	 int shm_segsz; /* size of segment (bytes) */
	 __kernel_time_t shm_atime; /* last attach time */
	 __kernel_time_t shm_dtime; /* last detach time */
	 __kernel_time_t shm_ctime; /* last change time */
	 __kernel_ipc_pid_t shm_cpid; /* pid of creator */
	 __kernel_ipc_pid_t shm_lpid; /* pid of last operator */
	 unsigned short shm_nattch; /* no. of current attaches */
	 unsigned short shm_unused; /* compatibility */
	 void *shm_unused2; /* ditto - used by DIPC */
	 void *shm_unused3; /* unused */
};
```


---

## 共享内存函数

**<kbd>shmget</kbd>函数**

```c
#include <sys/ipc.h>
#include <sys/shm.h>
功能：用来创建共享内存
原型
	 int shmget(key_t key, size_t size, int shmflg);
参数
	 key:这个共享内存段名字
	 size:共享内存大小
	 shmflg:由九个权限标志构成，它们的用法和创建文件时使用的mode模式标志是一样的
返回值：成功返回一个非负整数，即该共享内存段的标识码；失败返回-1
```
**size：**
共享内存的大小尽量保持在4kb的整数倍

**shmflg标识标志：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/1f47da94ba3e45128f6ff39ba01402d0.png)
**如果shmflg的值为IPC_CREAT或0，如果key对应的共享内存段不存在，则创建，如果已经存在，则直接返回已经存在的共享内存。
如果shmflg的值为IPC_CREAT|IPC_EXCL，如果key对应的共享内存段不存在，则创建，如果已经存在，则返回错误，这样就保证了调用sheget()申请的共享内存一定是未使用过的。**

---

如果获取key值呢？**调用ftok()接口生成**

**<kbd>ftok</kbd>函数**
```c
#include <sys/types.h>
#include <sys/ipc.h>
功能：用来获取key值
原型
	key_t ftok(const char *pathname, int proj_id);
参数
	pathname:自定义路径名
	proj_id:自定义项目id
```

如何保证不同进程访问的是同一块共享内存呢？

**通过自定义路径名+自定义项目id生成key的算法是一样的，就会形成同一个key值，访问的就是同一块共享内存块。key值会设置进内核关于共享内存的数据结构中。**


---
**<kbd>shmat</kbd>函数**

```c
#include <sys/types.h>
#include <sys/shm.h>

功能：将共享内存段连接到进程地址空间
原型
	 void *shmat(int shmid, const void *shmaddr, int shmflg);
参数
	 shmid: 共享内存标识
	 shmaddr:指定连接的地址
	 shmflg:它的两个可能取值是SHM_RND和SHM_RDONLY
返回值：成功返回一个指针，指向共享内存第一个字节；失败返回-1
```
**说明：**

```c
shmaddr为NULL，核心自动选择一个地址。
shmaddr不为NULL且shmflg无SHM_RND标记，则以shmaddr为连接地址。
shmaddr不为NULL且shmflg设置了SHM_RND标记，则连接的地址会自动向下调整为SHMLBA的整数倍。
公式：shmaddr - (shmaddr % SHMLBA)。
shmflg = SHM_RDONLY，表示连接操作用来只读共享内存。
```

- **用户在访问共享内存时，不需要调用read() or write()系统调用，因为共享内存一旦创建好，就会映射到进程的进程地址空间中，该进程就可以直接访问共享内存，就如同malloc()一样，不需要系统调用接口访问，所以共享内存是所有进程间通信速度最快的。**
- **在一个进程没有向共享内存写数据时，另一个进程并不会等待该进程写入之后在读数据，共享内存不提供同步和互斥机制，需要程序员自行维护数据安全。**
---


**<kbd>shmdt</kbd>函数**

```c
#include <sys/types.h>
#include <sys/shm.h>

功能：将共享内存段与当前进程脱离
原型
 	int shmdt(const void *shmaddr);
参数
 	shmaddr: 由shmat所返回的指针
返回值：成功返回0；失败返回-1
注意：将共享内存段与当前进程脱离不等于删除共享内存段
```


---

**<kbd>shmctl</kbd>函数**

```c
#include <sys/ipc.h>
#include <sys/shm.h>

功能：用于控制共享内存
原型
	 int shmctl(int shmid, int cmd, struct shmid_ds *buf);
参数
	 shmid:由shmget返回的共享内存标识码
	 cmd:将要采取的动作（有三个可取值）
	 buf:指向一个保存着共享内存的模式状态和访问权限的数据结构
返回值：成功返回0；失败返回-1
```
![在这里插入图片描述](https://img-blog.csdnimg.cn/d65da165859f4147a735a4e486f3c332.png)


---

## 创建共享内存
**comm.h：**

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define PATH_NAME "./"
#define PROJ_ID 0x6666
#define SIZE 4096
```

**server.c：**

```c
#include "comm.h"

// 创建共享内存

int main()
{
    key_t key = ftok(PATH_NAME, PROJ_ID);
    if(key < 0){
        perror("ftok");
        return 1;
    }

    int shmid = shmget(key, SIZE, IPC_CREAT | IPC_EXCL);
    if(shmid < 0){
        perror("shmget");
        return 1;
    }

    printf("key: %u, shmid: %d\n", key, shmid);

    return 0;
}

运行结果：
[cwx@VM-20-16-centos shared_memory]$ ./server 
key: 1711343725, shmid: 12
[cwx@VM-20-16-centos shared_memory]$ ipcs -m

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status      
0x6601086d 12         cwx        0          4096       0    
```

**我们可以发现运行server进程后，进程运行结束，但是进程创建的共享内存并没有被释放，systemV的IPC资源生命周期是随内核的，需要程序员显式的释放或者操作系统重启。**

**可以通过<kbd>ipcs -m</kbd>查看共享内存详情，用<kbd>ipcrm -m shmid</kbd>释放共享内存：**

```c
[cwx@VM-20-16-centos shared_memory]$ ipcs -m

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status      
0x6601086d 12         cwx        0          4096       0                       

[cwx@VM-20-16-centos shared_memory]$ ipcrm -m 12
[cwx@VM-20-16-centos shared_memory]$ ipcs -m

------ Shared Memory Segments --------
key        shmid      owner      perms      bytes      nattch     status   
```
**或者通过<kbd>shmctl</kbd>系统调用删除共享内存：**

```c
shmctl(shmid, IPC_RMID, NULL);
```





**key VS shmid:**

**key：key是在系统层面用来标识共享内存唯一性的值，不能用来管理共享内存。
shmid：shmid是操作系统给用户返回的id，在用户层用于管理共享内存。**


---
## 共享内存底层细节
### 共享内存大小
**<kbd>shmget</kbd>函数的size**

```c
int shmget(key_t key, size_t size, int shmflg);
参数
	 size:共享内存大小
```
**创建共享内存时，建议size设置为4096字节的整数倍，共享内存在内核中申请的基本单位是内存页(4KB)，操作系统为了提高内存和硬盘之间交换数据的速度，交换数据是以4KB为单位的，如果申请4097字节的共享内存，内核会给你4096*2字节的空间，但是用户可见的也只是4097个字节。**


---

### 共享内存属性的数据结构
<kbd>man shmctl</kbd>查看存储共享内存属性的数据结构：

![在这里插入图片描述](https://img-blog.csdnimg.cn/a1f9541bd17c49609c0a7b92c23439e2.png)


<kbd>man msgctl</kbd>查看存储消息队列属性的数据结构：

![在这里插入图片描述](https://img-blog.csdnimg.cn/4d8efc6a9a494b86a7a21667abf5dea9.png)


<kbd>man semctl</kbd>查看存储信号量属性的数据结构：

![在这里插入图片描述](https://img-blog.csdnimg.cn/1d4b01c8fe91440c958e48b9ae047379.png)


**总结：**
- **共享内存、消息队列和信号量的接口类似** 
- **共享内存、消息队列和信号量的数据结构的第一个数据的数据类型是一样的：struct ipc_perm**

---

在内核中，所有的IPC资源都是由数组组织起来的，但是共享内存、消息队列和信号量的数据结构类型完全不一样，操作系统是怎么组织的呢？

**所有的System V标准的IPC资源，XXXid_ds结构体的第一个数据类型都是ipc_prem，在内核中，有存放IPC资源的数组ipc_id_ary，ipc_id_ary数组的类型为(struct ipc_prem\*)，由于XXXid_ds结构体的第一个数据类型都是ipc_prem，存储时只需把结构体指针类型强转成（ipc_prem\*）就可以以相同的视角看待不同类型的IPC资源，如若需要使用IPC资源的其他成员，只需要把数组成员强转回原来的类型，解引用就可以得到所需成员，这与C++的切片原理类似，是用C语言实现的切片技术。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/c4e01c59e59e4998b8b70b6a55caf47b.png)





---

## 共享内存实现server&client通信
**comm.h：**

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define PATH_NAME "./"
#define PROJ_ID 0x6666
#define SIZE 4096
```

**Makefile文件：**

```c
.PHONY:all
all:client server

client:client.c
	gcc -o $@ $^

server:server.c
	gcc -o $@ $^

.PHONY:clean
clean:
	rm -f client server
```
**server.c：**

```c
#include "comm.h"

// 创建共享内存

int main()
{
    key_t key = ftok(PATH_NAME, PROJ_ID);
    if(key < 0){
        perror("ftok");
        return 1;
    }

    int shmid = shmget(key, SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if(shmid < 0){
        perror("shmget");
        return 1;
    }

    char* mem = shmat(shmid, NULL, 0);
    printf("server process attaches shared memory success\n");

    while(1)
    {
        sleep(1);
        printf("%s\n", mem);
    }

    shmdt(mem);
    printf("server process detaches shared memory success\n");

    shmctl(shmid, IPC_RMID, NULL);


    printf("key: %u shmid: %d delete success\n", key, shmid);


    return 0;
}
```

**client.c：**

```c
#include "comm.h"

int main()
{
    key_t key = ftok(PATH_NAME, PROJ_ID);
    if(key < 0){
        perror("ftok");
        return 1;
    }

    int shmid = shmget(key, SIZE, IPC_CREAT);
    if(shmid < 0){
        perror("shmget");
        return 1;
    }

    char* mem = shmat(shmid, NULL, 0);
    printf("client process attaches success\n");

    char ch = 'A';
    while(ch <= 'Z'){
        mem[ch-'A'] = ch;
        ch++;
        mem[ch-'A'] = 0;
        sleep(1);
    }

    shmdt(mem);
    printf("client process detaches success\n");

    return 0;
}
```

运行结果：
![在这里插入图片描述](https://img-blog.csdnimg.cn/6fc2954068554918a524c8cca5442ff3.png)


---


# system V信号量
信号量主要用于同步和互斥的。先了解概念，多线程会深入探讨。
## 临界资源
系统中被多个执行流同时访问的资源就是临界资源。比如同时向显示器打印消息，显示器就是一个临界资源，管道、共享内存、消息队列等都是临界资源。
## 临界区
进程的代码中用来访问临界资源的代码就是临界区。比如命名管道的代码：

```c
#include "comm.h"

int main()
{
    int fd = open(MY_FIFO, O_WRONLY);

    while(1){
        printf("请输入# ");
        fflush(stdout);
        char buffer[64] = {0};
        ssize_t s = read(0, buffer, sizeof(buffer)-1);

        // 临界区
        if(s > 0){
            buffer[s-1] = 0; // 去掉回车

            write(fd, buffer, strlen(buffer));
        }
    }
    close(fd);

    return 0;
}
```


## 原子性
原子性是指 一个操作是不可中断的，要么全部执行成功要么全部执行失败，有着"同生共死"的感觉。

## 互斥
是指散布在不同进程之间的若干程序片段，当某个进程执行其中的一个程序片段时，其他进程就不能运行它们之中的任一程序片段，只能等到该进程运行完之后才可以继续运行。

## 信号量
管道、共享内存和消息队列等进程间通信的方式都是以传输数据为目的的，信号量不是以传输数据为目的，而是通过资源"共享"的方式，来达到多个进程同步和互斥的目的，信号量是用来衡量临界资源数目的。

---
