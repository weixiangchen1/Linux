![在这里插入图片描述](https://img-blog.csdnimg.cn/5ee93720470047a8b769945644e0cb03.png)

@[TOC](文章目录)

# 生产者消费者模型
## 函数调用角度理解生产者消费者模型


我们之前学习的函数调用：

![在这里插入图片描述](https://img-blog.csdnimg.cn/9472bf76fc58414fa914286138c1a249.png)
**在调用FunctionA函数内部调用FunctionB，只有在进入FunctionB函数内部后执行FunctionB的业务代码才可以返回到调用FunctionB函数之后的代码继续执行。FunctionA把数据交给FunctionB处理，这在单线程下是一个串行的过程。函数和函数之间的交互的本质上是数据通信。**

**如果将FunctionA交由线程A处理，FunctionB交由线程B处理，并内置一段缓冲区，Function
A往缓冲区写数据，FunctionB往缓冲区取走数据处理，这样用两个执行流和一段缓冲区就可以实现FunctionA和FunctionB并行执行。我们把这种场景就叫做生产者消费者模型。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/5b6a6061cf4d4158ad9cb7b9aaefb4bd.png)

---

## 生活角度理解生产者消费者模型
**生活中的生产者消费者模型最典型的就是超市。站在超市的角度消费者就是普通老百姓，生产者就是各种供货商。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/ca2c610d7bcc4cfcb7ab6e27d6cdffb2.png)

为什么要有超市？

1. **提高效率。消费者和供应商无法直接交易，供应商的生产量动则成千上万，而消费者的需求并没有这么高，效率很低，这就需要超市来做一个中间件。超市的功能是收集需求，超市收集老百姓的需求，向各个供货商进大量的货物，足以养活供应商，也可以满足大部分老百姓的需求，超市的存在就大大减少了交易成本，这就大大提高了效率。**
2. **生产环节和消费环节进行解耦。当某一家矿泉水供应商倒闭了，对于普通老百姓短期来说是没有影响的，因为还可以购买超市的存货，这就实现了生产环节和消费环节的解耦，生产者和消费者互不影响。**



---


## 为什么要使用生产者消费者模型
- **生产者消费者模式就是通过一个容器来解决生产者和消费者的强耦合问题。生产者和消费者彼此之间不直接通讯，而通过阻塞队列来进行通讯，所以生产者生产完数据之后不用等待消费者处理，直接扔给阻塞队列，消费者不找生产者要数据，而是直接从阻塞队列里取，阻塞队列就相当于一个缓冲区，平衡了生产者和消费者的处理能力。这个阻塞队列就是用来给生产者和消费者解耦的。**

---

## 生产者消费者模型优点

 - **解耦**
 - **支持并发**
 - **支持忙闲不均**



---

## 321原则

**3种关系：**

 - 生产者 vs 生产者：竞争关系、互斥关系
 - 消费者 vs 消费者：竞争关系、互斥关系
 - 生产者 vs 消费者：互斥关系、同步关系

**2种角色：**
- 生产者（执行流）
- 消费者（执行流）


**1个消费场所：**
- 缓冲区（内存空间、STL容器等）


![在这里插入图片描述](https://img-blog.csdnimg.cn/ac51124d34e24e8da2215916229c11f1.png)


---

## 基于BlockingQueue的生产者消费者模型

- *在多线程编程中阻塞队列(Blocking Queue)是一种常用于实现生产者和消费者模型的数据结构。其与普通的队列区别在于，当队列为空时，从队列获取元素的操作将会被阻塞，直到队列中被放入了元素；当队列满时，往队列里存放元素的操作也会被阻塞，直到有元素被从队列中取出(以上的操作都是基于不同的线程来说的，线程在对阻塞队列进程操作时会被阻塞)。*

![在这里插入图片描述](https://img-blog.csdnimg.cn/960aafecbcf64fd1916d52ac6f6e7720.png)
**Makefile文件：**

```c
CpTest:CpTest.cc
	g++ -o $@ $^ -std=c++11 -lpthread
.PHONY:clean
clean:
	rm -f CpTest
```

**BlockQueue.hpp文件：**

```c
#pragma once
#include <iostream>
#include <queue>
#include <pthread.h>

namespace ns_blockqueue
{
    const int default_capacity = 5;
    template <class T>
    class BlockQueue
    {
    private:
        std::queue<T> _bq;       // 阻塞队列
        int _capacity;           // 队列的容量上限
        pthread_mutex_t _mtx;    // 保护临界资源的互斥锁
        pthread_cond_t is_full;  // 队列满，消费者条件变量等待
        pthread_cond_t is_empty; // 队列空，生产者条件变量等待


        bool IsFull()
        {
            return _bq.size() == _capacity;
        }

        bool IsEmpty()
        {
            return _bq.size() == 0;
        }

        void LockQueue()
        {
            pthread_mutex_lock(&_mtx);
        }

        void UnlockQueue()
        {
            pthread_mutex_unlock(&_mtx);
        }

        void ProducterWait()
        {
            pthread_cond_wait(&is_empty, &_mtx);
        }

        void ConsumerWait()
        {
            pthread_cond_wait(&is_full, &_mtx);
        }

        void WakeupConsumer()
        {
            pthread_cond_signal(&is_full);
        }

        void WakeupProducter()
        {
            pthread_cond_signal(&is_empty);
        }

    public:
        BlockQueue(int capacity = default_capacity) : _capacity(capacity)
        {
            pthread_mutex_init(&_mtx, nullptr);
            pthread_cond_init(&is_full, nullptr);
            pthread_cond_init(&is_empty, nullptr);
        }

        void Push(const T &in)
        {
            LockQueue();
            // if (IsFull())
            while (IsFull())
            {
                ProducterWait();
            }

            _bq.push(in);
            if (_bq.size() > _capacity / 2)
                WakeupConsumer();
            UnlockQueue();
        }

        void Pop(T *out)
        {
            LockQueue();
            while (IsEmpty())
            {
                ConsumerWait();
            }
            
            *out = _bq.front();
            _bq.pop();
            if (_bq.size() < _capacity / 2)
                WakeupProducter();
            UnlockQueue();
        }

        ~BlockQueue()
        {
            pthread_mutex_destroy(&_mtx);
            pthread_cond_destroy(&is_full);
            pthread_cond_destroy(&is_empty);
        }
    };
}

```

**Task.hpp：**

```c
#pragma once
#include <iostream>
#include <string>
#include <pthread.h>

namespace ns_task
{
    class Task
    {
    private:
        int _x;
        int _y;
        char _op;
    public:
        Task() {}
        Task(int x, int y, char op):_x(x), _y(y), _op(op) {}
        int Run()
        {
            int res = 0;
            switch(_op)
            {
            case '+':
                res = _x + _y;
                break;
            case '-':
                res = _x - _y;
                break;
            case '*':
                res = _x * _y;
                break;
            case '/':
                res = _x / _y;
                break;
            case '%':
                res = _x % _y;
                break;
            default:
                break;
            }
            std::cout << "当前任务正在被: " << pthread_self() << " 处理: " << _x << _op << _y << "=" << res << std::endl;
            return res;
         }
        
        int operator()()
        {
            return Run();
        }

        ~Task(){}
    };
}
```

**CpTest.cc文件：**

```c
#include "BlockQueue.hpp"
#include "Task.hpp"
#include <time.h>
#include <cstdlib>
#include <unistd.h>

using namespace ns_blockqueue;
using namespace ns_task;
void *consumer(void *args)
{
    BlockQueue<Task> *bq = (BlockQueue<Task> *)args;

    while(true)
    {
        Task t;
        bq->Pop(&t);
        t();

        // int data = 0;
        // bq->Pop(&data);
        // std::cout << "消费者消费数据：" << data << std::endl;
    }

}

void *producter(void *args)
{
    BlockQueue<Task> *bq = (BlockQueue<Task> *)args;
    std::string ops = "+-*/%";
    while(true)
    {
        int x = rand()%10 + 1;
        int y = rand()%20 + 1;
        char op = ops[rand()%5];
        Task t(x, y, op);
        std::cout << "线程: " << pthread_self() <<  " 分发任务 " << x << op << y << "=?" << std::endl;
        bq->Push(t);
        sleep(1);

        // sleep(1);
        // int data = rand()%20 + 1;
        // bq->Push(data);   
        // std::cout << "生产者成产数据：" << data << std::endl;

    }
}

int main()
{
    srand((long long)time(nullptr));
    BlockQueue<Task> *bq = new BlockQueue<Task>();

    pthread_t c, p;
    pthread_t c1, c2, c3;
    pthread_t p1, p2, p3;
    pthread_create(&c, nullptr, consumer, (void*)bq);
    // pthread_create(&c1, nullptr, consumer, (void*)bq);
    // pthread_create(&c2, nullptr, consumer, (void*)bq);
    // pthread_create(&c3, nullptr, consumer, (void*)bq);

    pthread_create(&p, nullptr, producter, (void*)bq);
    // pthread_create(&p1, nullptr, producter, (void*)bq);
    // pthread_create(&p2, nullptr, producter, (void*)bq);
    // pthread_create(&p3, nullptr, producter, (void*)bq);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);

    return 0;
}
```
![在这里插入图片描述](https://img-blog.csdnimg.cn/658c27f66496499589e8656e9ca7317b.png)
**Push函数中生产者判断是否为满时，用if进行条件判断是不太完善的，因为当满足为满条件后，生产者线程要进行挂起等待，但是如果挂起等待失败，如果生产者线程被伪唤醒，都会造成生产条件不具备，导致程序出错。所以用while循环判断是比较合理的，多次判断能避免以上的情况。**


**运行结果：**

```c
[cwx@VM-20-16-centos consumer_producter]$ make
g++ -o CpTest CpTest.cc -std=c++11 -lpthread
[cwx@VM-20-16-centos consumer_producter]$ ./CpTest 
线程: 140317604919040 分发任务 7*11=?
当前任务正在被: 140317613311744 处理: 7*11=77
线程: 140317604919040 分发任务 2-20=?
线程: 140317604919040 分发任务 10+7=?
线程: 140317604919040 分发任务 5%2=?
当前任务正在被: 140317613311744 处理: 2-20=-18
当前任务正在被: 140317613311744 处理: 10+7=17
当前任务正在被: 140317613311744 处理: 5%2=1
```


---

# 	POSIX信号量
POSIX信号量和SystemV信号量作用相同，都是用于同步操作，达到无冲突的访问共享资源目的。 但POSIX可以用于线程间同步。

## 回顾信号量概念
- **信号量的本质是一个计数器，用来描述临界资源中资源的数目(最多可以有多少资源分配给线程)。**
- **使用信号量就使得每个线程要访问临界资源就必须申请信号量，申请信号量的本质是预定资源，临界资源被拆分成一份一份的小资源，可以让多个线程同时访问临界资源，从而实现线程并发。**




---

## 信号量操作函数
**初始化信号量：**

```c
#include <semaphore.h>

原型：
	int sem_init(sem_t *sem, int pshared, unsigned int value);
参数：
	pshared：0表示线程间共享，非零表示进程间共享
	value：信号量初始值
```
**销毁信号量：**

```c
#include <semaphore.h>

原型：
	int sem_destroy(sem_t *sem);
```
**等待信号量：**

```c
#include <semaphore.h>

功能：	
	等待信号量，会将信号量的值减1
原型：
	int sem_wait(sem_t *sem); //P()
```
**发布信号量：**

```c
#include <semaphore.h>

功能：
	发布信号量，表示资源使用完毕，可以归还资源了。将信号量值加1。
原型：
	int sem_post(sem_t *sem); //V()
```

![在这里插入图片描述](https://img-blog.csdnimg.cn/4222cfb79b97430fa772941fda0a7da7.png)

---
## 环形队列

![在这里插入图片描述](https://img-blog.csdnimg.cn/2b0bd8f8170a4a238b95796905875bc6.png)

- **环形队列实际上是数组的线性空间来实现的。**

- **当队列到了尾部，运用取模操作使之指向头部。**


**环形队列的判空判满条件：**

- **front：指向环形队列的第一个元素，初始值为0**
- **rear：指向环形队列的最后一个元素的后一个位置，初始值为0**
- **队列为空：front == rear**
- **队列为满：(rear+1) % size == front**


---

## 基于环形队列的生产者消费者模型

![在这里插入图片描述](https://img-blog.csdnimg.cn/104e8e9fa21145baa565930c4be9947f.png)

**基本实现思想：**

- 生产者最关心的是环形队列中的空的位置。
- 消费者最关心的是环形队列中的数据。
- 根据空满状态，决定生产者消费者谁先访问。
- 生产者不能循环一圈超过消费者。
- 消费者不能超过生产者。
- 生产者和消费者并发执行。


**伪代码：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/c5d9adce58ec4fe89fd3578009b89f18.png)


---

**实现代码：**

**Makefile文件：**

```c
ring_queue_test:ring_queue_test.cc
	g++ -o $@ $^ -std=c++11 -lpthread

.PHONY:clean
clean:
	rm -rf ring_test
```

**ring_queue.hpp文件：**

```c
#pragma once
#include <iostream>
#include <vector>
#include <semaphore.h>

// 基于环形队列的多生产者多消费者模型
namespace ns_ring_queue
{
    const int cap_default = 10;
    template <class T>
    class RingQueue
    {
    private:
        std::vector<T> _ring_queue; // 环形队列
        int _cap;                   // 环形队列的容量
        sem_t _sem_blank;           // 空位置信号量
        sem_t _sem_data;            // 数据信号量
        int c_step;                 // 消费者下标
        int p_step;                 // 生产者下标

        pthread_mutex_t c_mtx;      // 保护消费者的临界资源
        pthread_mutex_t p_mtx;      // 保护生产者的临界资源

    public:
        RingQueue(int cap = cap_default) : _ring_queue(cap), _cap(cap)
        {
            sem_init(&_sem_blank, 0, _cap);
            sem_init(&_sem_data, 0, 0);
            c_step = p_step = 0;
            pthread_mutex_init(&c_mtx, nullptr);
            pthread_mutex_init(&p_mtx, nullptr);
        }

        void Push(const T &in)
        {
            sem_wait(&_sem_blank); //P(blank)
            pthread_mutex_lock(&p_mtx);
            _ring_queue[p_step] = in;

            p_step++; // 临界资源
            p_step %= _cap;

            pthread_mutex_unlock(&p_mtx);
            sem_post(&_sem_data); //V(data)
        }

        void Pop(T* out)
        {
            sem_wait(&_sem_data); //P(data)
            pthread_mutex_lock(&c_mtx);
            *out = _ring_queue[c_step];

            c_step++; // 临界资源
            c_step %= _cap;

            pthread_mutex_unlock(&c_mtx);
            sem_post(&_sem_blank); //V(blank)
        }

        ~RingQueue()
        {
            sem_destroy(&_sem_blank);
            sem_destroy(&_sem_data);
            pthread_mutex_destroy(&p_mtx);
            pthread_mutex_destroy(&c_mtx);
        }
    };
}
```
**ring_queue_test.cc文件：**

```c
#include "ring_queue.hpp"
#include "Task.hpp"
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

using namespace ns_task;
using namespace ns_ring_queue;

void* consumer(void* args)
{
    RingQueue<Task>* rq = (RingQueue<Task>*)args;

    while(true)
    {
        Task t;
        rq->Pop(&t);
        t();
        // sleep(1);
        // int data;
        // rq->Pop(&data);
        // std::cout << "Thread:" << pthread_self() << "消费者消费数据：" << data << std::endl;
        // sleep(1);
    }
}

void* producter(void* args)
{
    RingQueue<Task>* rq = (RingQueue<Task>*)args;
    while(true)
    {
        int x = rand()%20 + 1;
        int y = rand()%10 + 1;
        char op = "+-*/%"[rand()%5];
        Task t(x, y, op);
        rq->Push(t);

        std::cout << "Producter Thread: " << pthread_self() << " Task: " << t.message() << std::endl; 

        // int data = rand()%30 + 1;
        // rq->Push(data);
        // std::cout << "Thread:" << pthread_self() << "生产者生产数据：" << data << std::endl;
    }
}

int main()
{
    srand((long long)time(nullptr));
    RingQueue<Task>* rq = new RingQueue<Task>();
    
    pthread_t c0,c1,c2,c3,p0,p1,p2;
    pthread_create(&c0, nullptr, consumer, (void*)rq);
    pthread_create(&c1, nullptr, consumer, (void*)rq);
    pthread_create(&c2, nullptr, consumer, (void*)rq);
    pthread_create(&c3, nullptr, consumer, (void*)rq);
    pthread_create(&p0, nullptr, producter, (void*)rq);
    pthread_create(&p1, nullptr, producter, (void*)rq);
    pthread_create(&p2, nullptr, producter, (void*)rq);

    pthread_join(c0, nullptr);
    pthread_join(c1, nullptr);
    pthread_join(c2, nullptr);
    pthread_join(c3, nullptr);
    pthread_join(p0, nullptr);
    pthread_join(p1, nullptr);
    pthread_join(p2, nullptr);

    return 0;
}
```

**Task.hpp文件：**

```c
#pragma once
#include <iostream>
#include <string>
#include <pthread.h>

namespace ns_task
{
    class Task
    {
    private:
        int _x;
        int _y;
        char _op;
    public:
        Task() {}
        Task(int x, int y, char op):_x(x), _y(y), _op(op) {}
        std::string message()
        {
            std::string msg = std::to_string(_x);
            msg += _op;
            msg += std::to_string(_y);
            msg += "=?";

            return msg;
        }
        int Run()
        {
            int res = 0;
            switch(_op)
            {
            case '+':
                res = _x + _y;
                break;
            case '-':
                res = _x - _y;
                break;
            case '*':
                res = _x * _y;
                break;
            case '/':
                res = _x / _y;
                break;
            case '%':
                res = _x % _y;
                break;
            default:
                break;
            }
            std::cout << "Consumer Thread: " << pthread_self() << " Task: " << _x << _op << _y << "=" << res << std::endl;
            return res;
         }
        
        int operator()()
        {
            return Run();
        }

        ~Task(){}
    };
}
```

**运行结果：**

```c
[cwx@VM-20-16-centos ring_queue]$ ./ring_queue_test 
Producter Thread: 139742694962944 Task: 12+4=?
Consumer Thread: 139742736926464 Task: 12+4=16
Producter Thread: 139742711748352 Task: 20%2=?
Producter Thread: 139742703355648 Task: 17/10=?
Consumer Thread: 139742745319168 Task: 20%2=0
Consumer Thread: 139742745319168 Task: 17/10=1
```

---
