![在这里插入图片描述](https://img-blog.csdnimg.cn/5ee93720470047a8b769945644e0cb03.png)

@[TOC](文章目录)
# Linux线程互斥
## 进程线程间的互斥相关背景概念
**线程共享地址空间，线程可以看到大部分的资源。这样的的优点是方便通信，但是缺点是缺乏访问控制。如果因为其中一个线程的操作不当，引起崩溃、程序异常或逻辑不正确等现象，这就会造成线程安全。如果要让一个线程没有线程安全问题，就应尽量避免使用stl、malloc和new等会在全局内有效的数据，或者应进行访问控制：同步、互斥。**

 - **临界资源**：多线程执行流共享的资源就叫做临界资源。
 - **临界区**：每个线程内部，访问临界资源的代码，就叫做临界区。
 - **互斥**：任何时刻，互斥保证有且只有一个执行流进入临界区，访问临界资源，通常对临界资源起保护作用。
 - **原子性**：不会被任何调度机制打断的操作，该操作只有两态，要么完成，要么未完成。


---




## 互斥量mutex

 - **大部分情况，线程使用的数据都是局部变量，变量的地址空间在线程栈空间内，这种情况，变量归属单个线程，其他线程无法获得这种变量。**
 - **但有时候，很多变量都需要在线程间共享，这样的变量称为共享变量，可以通过数据的共享，完成线程之间的交互。多个线程并发的操作共享变量，会带来一些问题。**

下面我们模拟一个抢票程序，创建5个线程同时抢100张票：

```c
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int tickets = 100;

void* route(void* args)
{
    int num = *(int*)args;
    while(1){
        if(tickets > 0){
            usleep(1000);
            printf("thread %d bug the ticket: %d\n", num, tickets);
            tickets--;
        }
        else{
            printf("have no ticket\n");
            break;
        }
    }
}

int main()
{
    pthread_t tid[5];
    for(int i=0; i<5; i++){
        pthread_create(tid+i, NULL, route, (void*)&i);
    }

    for(int i=0; i<5; i++){
        pthread_join(tid[i], NULL);
    }

    return 0;
}

运行结果：
...
thread 3 bug the ticket: 1
thread 4 bug the ticket: 0
thread 1 bug the ticket: -1
thread 2 bug the ticket: -2
thread 3 bug the ticket: -3
```
为什么程序运行无法得到预期结果呢？

![在这里插入图片描述](https://img-blog.csdnimg.cn/29ef192a540f49a2962b91fa2c84272a.png)

 - **if 语句判断条件为真以后，代码可以并发的切换到其他线程。**
 - **usleep 这个模拟漫长业务的过程，在这个漫长的业务过程中，可能有很多个线程会进入该代码段。**
 - **tickets\-\-的汇编代码有三条：**

```
40064b: 	8b 05 e3 04 20 00	 mov 0x2004e3(%rip),%eax	 # 600b34 <ticket>
400651:		83 e8 01			 sub $0x1,%eax
400654: 	89 05 da 04 20 00 	 mov %eax,0x2004da(%rip)	 # 600b34 <ticket>
```

**tickets\-\- 操作并不是原子操作，而是对应三条汇编指令：**

 - **load ：将共享变量ticket从内存加载到寄存器中**
 - **update : 更新寄存器里面的值，执行-1操作**
 - **store ：将新值，从寄存器写回共享变量ticket的内存地址**

要解决以上问题，需要做到三点：

 - **代码必须要有互斥行为：当代码进入临界区执行时，不允许其他线程进入该临界区。**
 - **如果多个线程同时要求执行临界区的代码，并且临界区没有线程在执行，那么只能允许一个线程进入该临界区。**
 - **如果线程不在临界区中执行，那么该线程不能阻止其他线程进入临界区。**

要做到这三点，本质上就是需要一把锁。Linux上提供的这把锁叫**互斥锁**。
![在这里插入图片描述](https://img-blog.csdnimg.cn/8334711aa518487fb886a9ea7c6cb074.png)

---

##  互斥量的接口

### 初始化互斥量

 1. **静态分配**

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER
```

 2. **动态分配**

```c
int pthread_mutex_init(pthread_mutex_t *restrict mutex, pthread_mutexattr_t *restrict attr);
参数：
	mutex：要初始化的互斥量
	attr：NULL
```

### 销毁互斥量

 - **使用 <kbd>PTHREAD_ MUTEX_ INITIALIZER </kbd>初始化的互斥量不需要销毁。**
 - **不要销毁一个已经加锁的互斥量。**
 - **已经销毁的互斥量，要确保后面不会有线程再尝试加锁。**

```c
int pthread_mutex_destroy(pthread_mutex_t *mutex);
参数：
	mutex：要销毁的互斥量
```
### 互斥量的加锁和解锁

```c
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
返回值:成功返回0,失败返回错误码
```

调用<kbd> pthread_ lock </kbd>时，可能会遇到以下情况:

 - **互斥量处于未锁状态，该函数会将互斥量锁定，同时返回成功。**
 - **发起函数调用时，其他线程已经锁定互斥量，或者存在其他线程同时申请互斥量，但没有竞争到互斥量，那么pthread_ lock调用会陷入阻塞(执行流被挂起)，等待互斥量解锁。**


改进上面的抢票程序：

```c
#include <iostream>
#include <unistd.h>
#include <cstdio>
#include <stdlib.h>
#include <string>
#include <pthread.h>

class Ticket
{
private:
    int tickets;
    pthread_mutex_t mtx; 

public:
    Ticket()
        :tickets(100000)
    {
        pthread_mutex_init(&mtx, nullptr); //初始化锁
    }

    bool GetTicket()
    {
        bool res = true;

        pthread_mutex_lock(&mtx); //申请锁,临界区加锁,访问临界资源
        if(tickets > 0)
        {
            usleep(100);
            std::cout <<  pthread_self() << " thread buy " << "No." << tickets << " ticket" << std::endl;
            tickets--; 
            printf("");
        }
        else
        {
            std::cout << "have no ticket" << std::endl;
            res = false;
        }
        pthread_mutex_unlock(&mtx); // 解锁

        return res;
    }

    ~Ticket()
    {
        pthread_mutex_destroy(&mtx);
    }
};

void *ThreadRun(void *args)
{
    Ticket *t = (Ticket*)args;

    while(true)
    {
        if(!t->GetTicket())
             break;
        //printf("%lu\n", pthread_self());
    }

}

int main()
{
    Ticket *t = new Ticket();
    pthread_t tid[5];
    for(int i = 0; i < 5; i++)
    {
        //创建5个线程,每个线程执行ThreadRun函数
        pthread_create(tid+i, nullptr, ThreadRun, (void*)t);
    }

    for(int i = 0; i < 5; i++)
    {
        //线程等待
        pthread_join(tid[i], nullptr);
    }
    return 0;
}

运行结果：
...
140662086293248 thread buy No.5 ticket
140662086293248 thread buy No.4 ticket
140662086293248 thread buy No.3 ticket
140662086293248 thread buy No.2 ticket
140662086293248 thread buy No.1 ticket
have no ticket
have no ticket
have no ticket
have no ticket
have no ticket
```

---

## 互斥量实现原理

 - **经过上面的例子，大家已经意识到单纯的 i++ 或者 ++i 都不是原子的，有可能会有数据一致性问题。**
 - **为了实现互斥锁操作，大多数体系结构都提供了swap或exchange指令，该指令的作用是把寄存器和内存单元的数据相交换，由于只有一条指令，保证了原子性，即使是多处理器平台，访问内存的 总线周期也有先后，一个处理器上的交换指令执行时另一个处理器的交换指令只能等待总线周期。**


![在这里插入图片描述](https://img-blog.csdnimg.cn/61c87f6741144473922e8f3d9cc827b7.png)
- **线程是有可能在临界区被切换的，线程再被切走的时候会进行上下文保护，锁数据是被保存在上下文当中的，就算线程被切换，线程也是拿着锁被切走的，在此期间别的线程无法申请锁成功，也就进入不了临界区。**
- **为了保证临界区的线程安全，程序员必须保证每个线程都遵循相同的规范，比如申请锁的顺序。**

---

## 可重入vs线程安全
**概念:**

 - **线程安全**：多个线程并发同一段代码时，不会出现不同的结果。常见对全局变量或者静态变量进行操作，并且没有锁保护的情况下，会出现该问题。
 - **重入**：同一个函数被不同的执行流调用，当前一个流程还没有执行完，就有其他的执行流再次进入，我们称之为重入。一个函数在重入的情况下，运行结果不会出现任何不同或者任何问题，则该函数被称为可重入函数，否则，是不可重入函数。

---

### 常见线程不安全情况

 - **不保护共享变量的函数。**
 - **函数状态随着被调用，状态发生变化的函数。**
 - **返回指向静态变量指针的函数。**
 - **调用线程不安全函数的函数。**




### 常见线程安全的情况

 - **每个线程对全局变量或者静态变量只有读取的权限，而没有写入的权限，一般来说这些线程是安全的。**
 - **类或者接口对于线程来说都是原子操作。**
 - **多个线程之间的切换不会导致该接口的执行结果存在二义性。**


---

### 常见不可重入的情况

 - **调用了malloc/free函数，因为malloc函数是用全局链表来管理堆的。**
 - **调用了标准I/O库函数，标准I/O库的很多实现都以不可重入的方式使用全局数据结构。**
 - **可重入函数体内使用了静态的数据结构。**

### 常见可重入的情况

 - **不使用全局变量或静态变量。**
 - **不使用用malloc或者new开辟出的空间。**
 - **不调用不可重入函数。**
 - **不返回静态或全局数据，所有数据都有函数的调用者提供。**
 - **使用本地数据，或者通过制作全局数据的本地拷贝来保护全局数据。**

---

### 可重入与线程安全的联系

 - **函数是可重入的，那就是线程安全的。**
 - **函数是不可重入的，那就不能由多个线程使用，有可能引发线程安全问题。**
 - **如果一个函数中有全局变量，那么这个函数既不是线程安全也不是可重入的。**


### 可重入与线程安全的区别

 - **可重入函数是线程安全函数的一种。**
 - **线程安全不一定是可重入的，而可重入函数则一定是线程安全的。**
 - **如果将对临界资源的访问加上锁，则这个函数是线程安全的，但如果这个重入函数若锁还未释放则会产生死锁，因此是不可重入的。**

---

# 常见锁概念
## 死锁
- **指在一组进程中的各个进程均占有不会释放的资源，但因互相申请被其他进程所站用不会释放的资源而处于的一种永久等待状态。**


## 死锁的四个必要条件

 - **互斥条件**：一个资源每次只能被一个执行流使用
 - **请求与保持条件**：一个执行流因请求资源而阻塞时，对已获得的资源保持不放
 - **不剥夺条件**：一个执行流已获得的资源，在末使用完之前，不能强行剥夺
 - **循环等待条件**：若干执行流之间形成一种头尾相接的循环等待资源的关系

## 避免死锁

 - **破坏死锁的四个必要条件**
 - **加锁顺序一致**
 - **避免锁未释放的场景**
 - **资源一次性分配**


---

# Linux线程同步
## 条件变量
 - 当一个线程互斥地访问某个变量时，它可能发现在其它线程改变状态之前，它、该线程什么也做不了。
 - 例如一个线程访问队列时，发现队列为空，它只能等待，只到其它线程将一个节点添加到队列中。这种情况就需要用到条件变量。

## 同步概念与竞态条件

 - **同步**：在保证数据安全的前提下，让线程能够按照某种特定的顺序访问临界资源，从而有效避免饥饿问题，叫做同步。
 - **竞态条件**：因为时序问题，而导致程序异常，我们称之为竞态条件。


---

## 条件变量函数

**条件变量初始化：**

```c
int pthread_cond_init(pthread_cond_t *restrict cond,const pthread_condattr_t *restrict
attr);
参数：
	cond：要初始化的条件变量
	attr：NULL
```

**条件变量销毁：**

```c
int pthread_cond_destroy(pthread_cond_t *cond);
参数：
	cond：要销毁的条件变量
```

**等待条件满足：**

```c
int pthread_cond_wait(pthread_cond_t *restrict cond,pthread_mutex_t *restrict mutex);
参数：
	cond：要在这个条件变量上等待
	mutex：互斥量，后面详细解释
```

**唤醒等待：**

```c
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_cond_signal(pthread_cond_t *cond);
```


---

创建一个线程为master线程，三个线程为worker线程，每过3s master线程唤醒一个worker线程工作。
测试代码：

```c
#include <iostream>
#include <unistd.h>
#include <string>
#include <pthread.h>

pthread_mutex_t mtx;
pthread_cond_t cond;

void* Worker(void* args)
{
    int id = *(int*)args;
    delete (int*)args;

    while(true)
    {
        pthread_cond_wait(&cond, &mtx);
        std::cout << "No." << id << " Worker thread working..." << std::endl;
    }

}

void* Master(void* args)
{
    std::string str = (char*)args;
    while(true){
        std::cout << str << " wake up worker thread" << std::endl;
        pthread_cond_signal(&cond);
        sleep(3);
    }
}

int main()
{
    // 初始化互斥量和条件变量
    pthread_mutex_init(&mtx, nullptr);
    pthread_cond_init(&cond, nullptr);

    pthread_t master;
    pthread_t workers[3];

    // 创建master线程和worker线程
    pthread_create(&master, nullptr, Master, (void*)"master thread");
    for(int i=0; i<3; i++)
    {
        int* num = new int(i);
        pthread_create(workers+i, nullptr, Worker, (void*)num);
    }

    // 线程等待
    pthread_join(master, nullptr);
    for(int i=0; i<3; i++)
    {
        pthread_join(workers[i], nullptr);
    }

    // 销毁互斥锁和条件变量
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond);

    return 0;
}

运行结果：
[cwx@VM-20-16-centos cond]$ ./mytest 
master thread wake up worker thread
No.0 Worker thread working...
master thread wake up worker thread
No.1 Worker thread working...
master thread wake up worker thread
No.2 Worker thread working...
```


## 条件变量使用规范

- **等待条件代码**

```c
pthread_mutex_lock(&mutex);
while (条件为假）
	pthread_cond_wait(cond, mutex);
修改条件
pthread_mutex_unlock(&mutex);
```

- **给条件发送信号代码**

```c
pthread_mutex_lock(&mutex);
设置条件为真
pthread_cond_signal(cond);
pthread_mutex_unlock(&mutex);
```

