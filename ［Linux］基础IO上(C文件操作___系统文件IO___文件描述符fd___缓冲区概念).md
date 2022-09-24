![在这里插入图片描述](https://img-blog.csdnimg.cn/5ee93720470047a8b769945644e0cb03.png)

@[TOC](文章目录)

# C文件接口

```c
#include <stdio.h>

FILE *fopen(const char *path, const char *mode);
函数 fopen 打开文件名为 path 指向的字符串的文件，将一个流与它关联。
参数 mode 指向一个字符串，"w","w+","r","r+","a".


size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
函数 fread（） 从流指向的流中读取数据的 nmemb 元素，
每个大小的字节长度，并将它们存储在ptr.

size_t fwrite(const void *ptr, size_t size, size_t nmemb,FILE *stream);
函数 fwrite（） 将数据的 nmemb 元素（每个大小的字节长度）
写入流所指向的流，并从该位置获取它们由 ptr 给出。

int fclose(FILE *fp);
fclose（） 函数刷新 fp 所指向的流（使用 fflush（3） 写入
任何缓冲的输出数据）并关闭基础文件描述符。
```

---
## 测试C文件操作接口

测试代码(写文件):

```c
#include <stdio.h>
#include <string.h>

int main()
{   
    FILE* fp = fopen("myfile", "w");
    if(fp == NULL){
        printf("open file error\n");
    }

    // 打开文件成功，开始写文件
    const char* str = "hello world\n";
    int cnt = 5;
    while(cnt--)
    {
        fwrite(str, strlen(str), 1, fp);
    }

    fclose(fp);

    return 0;
}

运行结果:
[cwx@VM-20-16-centos fd]$ ./mytest 
[cwx@VM-20-16-centos fd]$ cat myfile 
hello world
hello world
hello world
hello world
hello world
```

测试代码(读文件):

```c
#include <stdio.h>
#include <string.h>

int main()
{   
    FILE* fp = fopen("myfile", "r");
    if(fp == NULL){
        printf("open file error\n");
    }
    ssize_t s = 0;
    char* buffer[1024];
    while(s = fread(buffer, 1, sizeof(buffer), fp))
    {
        buffer[s] = 0;
        printf("%s", buffer);
    }

    fclose(fp);

    return 0;
}
运行结果：
[cwx@VM-20-16-centos fd]$ cat myfile 
hello world
hello world
hello world
hello world
hello world
[cwx@VM-20-16-centos fd]$ ./mytest 
hello world
hello world
hello world
hello world
hello world
```
---

## stdin && stdout && stderr
默认情况下，当我们的代码运行起来形成进程，操作系统就会默认帮我们打开三个标准输入输出流 -- stdin(标准输入)，stdout(标准输出) ，stderr(标准错误)。这三个流的类型都是FILE*, fopen返回值类型，都是文件指针。
![在这里插入图片描述](https://img-blog.csdnimg.cn/4b1eb780ef534a26b671103fa3cf7e5d.png)

---
## 输出信息到显示器的三种方式

```c
#include <stdio.h>
#include <string.h>

int main()
{
    const char* msg = "hello world\n";
    printf("%s", msg);
    fprintf(stdout, msg);
    fwrite(msg, strlen(msg), 1, stdout);

    return 0;
}

运行结果：
[cwx@VM-20-16-centos fd]$ ./mytest 
hello world
hello world
hello world
```

---

# 系统文件IO
## 接口介绍
<kbd>open</kbd>接口介绍
```c
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);

pathname: 要打开或创建的目标文件
flags: 打开文件时，可以传入多个参数选项，用下面的一个或者多个常量进行“或”运算，构成flags。
参数:
	O_RDONLY: 只读打开
	O_WRONLY: 只写打开
	O_RDWR : 读，写打开
	这三个常量，必须指定一个且只能指定一个
	O_CREAT : 若文件不存在，则创建它。需要使用mode选项，来指明新文件的访问权限
	O_APPEND: 追加写
返回值：
	成功：新打开的文件描述符
	失败：-1
```
<kbd>write</kbd> 接口介绍

```c
#include <unistd.h>

ssize_t write(int fd, const void *buf, size_t count);

fd：要写入文件的文件描述符
buf：指向写入的文件的内容的指针
count：写入内容的字节数
```
<kbd>read</kbd> 接口介绍

```c
#include <unistd.h>

ssize_t read(int fd, void *buf, size_t count);

fd：要读取文件的文件描述符
buf：存放读取的文件的内容的指针
count：读取内容的字节数
```

<kbd>close</kbd> 接口介绍

```c
#include <unistd.h>

int close(int fd);
close（）关闭文件描述符，使其不再引用任何文件，并可重复使用。
```
---
## 测试系统文件操作接口
**写文件：**

```c
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd = open("./myfile", O_WRONLY | O_CREAT, 0664);
    if(fd < 0){
        perror("open");
        return 1;
    }

    int cnt = 5;
    const char* str = "hello Linux\n";
    while(cnt--){
        write(fd, str, strlen(str));
    }

    close(fd);
    
    return 0;
}

运行结果：
[cwx@VM-20-16-centos fd]$ ./mytest 
[cwx@VM-20-16-centos fd]$ cat myfile 
hello Linux
hello Linux
hello Linux
hello Linux
hello Linux
```

**读文件：**

```c
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd = open("./myfile", O_RDONLY);
    if(fd < 0){
        perror("open");
        return 1;
    }

    char buffer[1024];
    ssize_t s = 0;
    while(s = read(fd, buffer, sizeof(buffer)))
    {
        buffer[s] = 0;
        printf("%s", buffer);
    }

    close(fd);

    return 0;
}

运行结果：
[cwx@VM-20-16-centos fd]$ cat myfile 
hello Linux
hello Linux
hello Linux
hello Linux
hello Linux
[cwx@VM-20-16-centos fd]$ ./mytest 
hello Linux
hello Linux
hello Linux
hello Linux
hello Linux
```
---
## open函数返回值(文件描述符)

- **当一个文件没有被打开，文件本身是被保存在磁盘里的，当创建一个空文件是，文件的属性是要占用磁盘空间的，文件有属性，属性也是一种数据。所以，磁盘文件 = 文件内容 + 文件属性，所以我们学的文件操作，就是对文件的属性进行操作。**
- **进程有可能一次性打开多个文件，操作系统就必须对文件的信息进行管理，操作系统进行管理的方式是：先描述，再组织，所以操作系统中就有保存文件属性的struct file，并以数组的形式(fd_array[])将存放文件属性的struct file\*组织起来，数组的下标就为open函数的返回值，称为文件描述符fd，fd_array存放在关联进程和文件的结构体struct files_struct中，进程PCB保存着该结构体的指针struct files_struct\*。**
- **open函数的返回值文件描述符fd本质上是内核中进程和文件关联的数组的下标。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/7e689db9542c45a4b203b55529f49b49.png)


---

## Linux源码结构

**task_struct中的struct files_struct \*files:**
![在这里插入图片描述](https://img-blog.csdnimg.cn/1fdbc2bfdecd422a9a1f3786c49b4c57.png)
**struct files_struct结构体：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/44b55047097c4c73a64b4a9ba699572a.png)
**struct file结构体:**
![在这里插入图片描述](https://img-blog.csdnimg.cn/cec34a06245044149c5a5ba7cd5c6250.png)

---
## Linux下一切皆文件

![在这里插入图片描述](https://img-blog.csdnimg.cn/77530a70c91d4c2d85cc624d3015711b.png)

在计算机中，有很多的外设，比如键盘，显示器，磁盘等等，这些外设都要和内存进行数据交互，要进行数据交互，这些外设就必须有IO操作，底层就必须都读写方法，比如读写键盘，读写显示器，读写磁盘等等。但是读写这些外设的方法，在底层的实现一定是不一样的，那么在Linux下是如何实现一切皆文件的呢？

**在Linux下，实际上设计了一层软件虚拟层，称之为虚拟文件系统(vfs)，之前提到的struct file结构体的组织与管理就是vfs的一种。实际上在每一个struct file中有一批函数指针，比如read方法或者write方法，这样当我们站在上层看的时候，每一个文件都有所对应的read和write方法，读操作就调用read方法，写操作就调用write方法，每个外设文件的读写操作都不一样，根本不关心对应文件对应的是哪个外设，这就实现了类似多态的原理，在上层就是以统一的struct file的视角看待每一个文件，实现了Linux一切皆文件的视角。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/16ad97caf27b48bab598351bd33c1c3b.png)



---
## 文件描述符的分配规则

**查看open返回值：**

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main()
{
    int fd = open("./myfile", O_RDONLY);
    if(fd < 0){
        perror("open");
        return 1;
    }

    printf("fd: %d\n", fd);


    return 0;
}

运行结果：
[cwx@VM-20-16-centos fd]$ ./mytest 
fd: 3
```
可以看到fd的值为3，这是因为**当进程运行时，进程会默认打开三个缺省的文件描述符，分别是0标准输入，1标准输出，2标准错误，012对应的硬件设备是键盘，显示器和显示器。**

**当我们关闭0号或者2号文件描述符:**

```c
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    close(0);
    //close(2);
    int fd = open("./myfile", O_RDONLY);
    if(fd < 0){
        perror("open");
        return 1;
    }

    printf("fd: %d\n", fd);


    return 0;
}

运行结果：
[cwx@VM-20-16-centos fd]$ ./mytest 
fd: 0
[cwx@VM-20-16-centos fd]$ ./mytest 
fd: 2
```
我们可以看到结果是fd: 0或者fd: 2。
可见，**文件描述符的分配规则：当前数组中的下标没有被使用的，且最小的一个下标。**

---

fork()创建子进程时，父子进程的文件描述符会共享吗？为什么所有的进程都会默认打开标准输入，标准输出和标准错误呢？

![在这里插入图片描述](https://img-blog.csdnimg.cn/795f088ef776493aaa0b93f52a7aec9c.png)
**进程在调用fork()创建子进程时，子进程会以父进程为模板，拷贝父进程的内核数据结构生成子进程自己的数据结构，files_struct也会被拷贝到子进程，父子进程的file_struct是一模一样的，所以文件描述符父子进程会共享。**

**因为所有的进程的父进程都是bash，bash是命令行，默认会打开标准输入标准输出标准错误，而所有的进程都会以bash进程为模板，继承内核数据结构和已经被打开的文件信息，所以所有的进程都会默认打开标准输入，标准输出和标准错误。**

---
## 重定向原理
上面我们关闭了0号(标准输入)或者2号(标准错误)文件描述符，如果关闭1号(标准输出)文件描述符呢？

```c
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    close(1);
    int fd = open("./myfile", O_WRONLY | O_CREAT, 0664);
    if(fd < 0){
        perror("open");
        return 1;
    }

    printf("fd: %d\n", fd);

    const char* msg = "hello world\n";
    int cnt = 5;
    while(cnt--)
    {
        write(stdout, msg, strlen(msg));
    }

    return 0;
}

运行结果：
[cwx@VM-20-16-centos fd]$ ./mytest 
[cwx@VM-20-16-centos fd]$ cat myfile 
fd: 1
```
本应该输出到显示器的内容，输出到了myfile这个文件当中，这种现象叫做重定向。

**重定向的本质：**

**当我们关闭了1号文件描述符，标准输出被关闭，1号文件描述符就是空余的，当我们再次打开一个文件的时候，由 "文件描述符的分配规则：当前数组中的下标没有没使用，且最小的一个下标" 我们可以知道，该文件被分配的fd是1号文件描述符，printf的本质是往1号文件描述符对应的文件上打印信息，但是标准输出已经被关闭，1号文件描述符对应的不再是标准输出，而是新打开的文件myfile，所以凡是要写到1号文件描述符的内容，就都被写到了myfile这个文件当中。**



![在这里插入图片描述](https://img-blog.csdnimg.cn/2b8b89ad59a04f0488ad579354f1a51d.png)

---

## dup系统调用实现重定向
**函数原型：**

```c
#include <unistd.h>

int dup2(int oldfd, int newfd);

dup2() 使 newfd 成为 oldfd 的副本，
必要时先关闭 newfd，但请注意以下几点：

  * 如果 oldfd 不是有效的文件描述符，则调用失败，并且 newfd 不会关闭。
  * 如果 oldfd 是一个有效的文件描述符，并且 newfd 与 oldfd 具有相同的值，则 dup2() 什么都不做，并返回 newfd。
```

**测试代码:**

```c
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int main()
{
    int fd = open("./myfile", O_WRONLY | O_CREAT, 0644);
    if(fd < 0){
        perror("open");
        return 1;
    }

    close(1);
    dup2(fd, 1); // 将1号文件描述符变成fd，实现输出重定向
    int cnt = 5;
    const char* msg = "hello world\n"; 
    while(cnt--){
        write(1, msg, strlen(msg));
    }

    return 0;
}

运行结果：
[cwx@VM-20-16-centos fd]$ ./mytest 
[cwx@VM-20-16-centos fd]$ cat myfile 
hello world
hello world
hello world
hello world
hello world
```

---
# FILE
## 库函数和系统调用
在学习FILE之前，我们先认识一下库函数和系统调用的区别。

- <kbd>fopen</kbd><kbd>fread</kbd><kbd>fwrite</kbd><kbd>fclose</kbd>都是C标准库提供的函数，称之为库函数(lib)。
- <kbd>open</kbd><kbd>read</kbd><kbd>write</kbd><kbd>close</kbd>都是系统提供的接口，称之为系统调用接口。

![在这里插入图片描述](https://img-blog.csdnimg.cn/e687dce6651a4e3aa9fa882830db796e.png)
从上图可以清楚看出，**库函数一般都是对系统调用的封装，方便二次开发。**

---
## fd与_fileno
在我们学习重定向原理的时候，有这样一段代码：
```c
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    close(1);
    int fd = open("./myfile", O_WRONLY | O_CREAT, 0644);
    if(fd < 0){
        perror("open");
        return 1;
    }

    printf("fd: %d\n", fd);

    const char* msg = "hello world\n";
    int cnt = 5;
    while(cnt--)
    {
        write(stdout, msg, strlen(msg));
    }

    return 0;
}

运行结果：
[cwx@VM-20-16-centos fd]$ ./mytest 
[cwx@VM-20-16-centos fd]$ cat myfile 
fd: 1
```
<kbd>printf</kbd>的本质是往标准输出(stdout)上输出消息，<kbd>printf</kbd>函数只认识1号文件描述符，发生了重定向，那么<kbd>printf</kbd>是怎么知道stdout所对应的是1号文件描述符呢？
![在这里插入图片描述](https://img-blog.csdnimg.cn/deaf38c4900d4b53bb6b590f615597bb.png)

**stdout是一个FILE\* 的指针，FILE是C语言层面上的结构体，在FILE结构体中，有一个整型的变量_fileno所对应的就是系统层面上的fd文件描述符。**

```c
#include <stdio.h>

int main()
{
    printf("stdin->_fileno: %d\n", stdin->_fileno);
    printf("stdout->_fileno: %d\n", stdout->_fileno);
    printf("stderr->_fileno: %d\n", stderr->_fileno);

    return 0;
}
运行结果：
[cwx@VM-20-16-centos fd]$ ./mytest 
stdin->_fileno: 0
stdout->_fileno: 1
stderr->_fileno: 2
```

![在这里插入图片描述](https://img-blog.csdnimg.cn/554b06a1cb98458bb244712e66c7b3ac.png)

---

## 缓冲区概念
下面观察一段代码：

```c
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{    
    close(1); // 关闭标准输入
    int fd = open("./myfile", O_WRONLY | O_CREAT, 0644);
    if(fd < 0)
    {
        perror("open");
        return 1;
    } 

    printf("fd: %d\n", fd);
    fprintf(stdout, "hello world\n");
    fprintf(stdout, "hello world\n");
    fprintf(stdout, "hello world\n");
    fprintf(stdout, "hello world\n");

    //close(fd);
    return 0;
}

运行结果：(调用close(fd))
[cwx@VM-20-16-centos fd]$ ./mytest 
[cwx@VM-20-16-centos fd]$ cat myfile 
fd: 1
hello world
hello world
hello world
hello world

运行结果：(没有调用close(fd))
[cwx@VM-20-16-centos fd]$ ./mytest 
[cwx@VM-20-16-centos fd]$ cat myfile 
[cwx@VM-20-16-centos fd]$ 

```
现象是当我们关闭了文件描述符fd时，<kbd>fprintf</kbd>本来要输出重定向到文件myfile的内容，并没有载入，而没有关闭了文件描述符fd，文件中就会重定向数据，这是为什么？

**实际上缓冲区分为用户级缓冲区和系统级别的内核缓冲区，<kbd>fprintf</kbd>本质是往stdout写入，stdout是一个FILE结构体指针，FILE结构体中不仅有C语言提供的缓冲区，也是用户级缓冲区，还封装了文件描述符fd，<kbd>fprintf</kbd>写入时，首先先将内容写入到C语言的缓冲区，在定期刷新到内核级缓冲区(有不同的刷新策略)，再写到磁盘中。在进程退出的时候，会刷新FILE内部缓冲区的数据到内核级缓冲区。**

**此时代码发生了重定向，由输出到显示器变为输出到文件(磁盘)，本来应该是行缓冲的刷新策略变成了全缓冲的刷新策略，当未调用close(fd)时，进程退出后会自动刷新用户级缓冲区到内核级缓冲区，文件里就有预期的内容，但是当我们调用close(fd)时，进程在退出前关闭了文件描述符fd，就无法通过文件描述符找到内核级缓冲区进行数据刷新，用户级缓冲区的数据就来不及刷新到内核级缓冲区，导致数据被留在了用户级缓冲区，这就是当调用close(fd)时，文件的内容没有被重定向的原因。**


用户级缓冲区→内核级缓冲区刷新策略：

 1. 立即刷新，不缓冲
 2. 行刷新，行缓冲，比如显示器打印信息
 3. 全缓冲，待到缓冲区满了才缓冲，比如往磁盘中写入


![在这里插入图片描述](https://img-blog.csdnimg.cn/6402ab2e4b084a0e8cbd736e9d98836a.png)



如果在调用close(fd)之前加入fflush(stdout)进行强制刷新，要输出重定向到文件myfile的内容就会被从用户级缓冲区刷新到内核级缓冲区，再写入到文件中。

```c
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{    
    close(1); // 关闭标准输入
    int fd = open("./myfile", O_WRONLY | O_CREAT, 0644);
    if(fd < 0)
    {
        perror("open");
        return 1;
    } 

    printf("fd: %d\n", fd);
    fprintf(stdout, "hello world\n");
    fprintf(stdout, "hello world\n");
    fprintf(stdout, "hello world\n");
    fprintf(stdout, "hello world\n");

    fflush(stdout);
    close(fd);
    return 0;
}

运行结果：
[cwx@VM-20-16-centos fd]$ ./mytest 
[cwx@VM-20-16-centos fd]$ cat myfile 
fd: 1
hello world
hello world
hello world
hello world
```

---
当我们fork()创建子进程时，C标准库提供的用户级缓冲区会不会被拷贝呢？

```c
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    const char* msg = "call write\n";
    write(1, msg, strlen(msg));
    printf("call printf\n");
    fprintf(stdout, "call fprintf\n");
    fputs("call fputs\n", stdout);

    fork();
    return 0;
}

运行结果：
[cwx@VM-20-16-centos fd]$ ./mytest 
call write
call printf
call fprintf
call fputs
[cwx@VM-20-16-centos fd]$ ./mytest > myfile 
[cwx@VM-20-16-centos fd]$ cat myfile 
call write
call printf
call fprintf
call fputs
call printf
call fprintf
call fputs
```
我们可以看到，当进程正常运行时，往显示器上输出内容时，结果无异常，但是如果我们重定向到文件中，我们会发现系统调用接口的内容只有一份，而C标准库的接口有两份，这是为什么？


**当我们发生重定向操作时，缓冲区的刷新策略由行缓冲变成全缓冲，<kbd>fprintf</kbd><kbd>printf</kbd><kbd>fputs</kbd>都是C标准库的函数，要打印的内容会被首先写入父进程中C标准库提供的用户级缓冲区，父进程的缓冲区是父进程的数据，当fork()创建子进程后，子进程会以父进程为模板拷贝数据，在退出进程后父进程会将用户级缓冲区写入到内核级缓冲区，刷新的本质就是一种写入，这时候父子进程就会发生写时拷贝，父子进程的数据会各自私有一份，子进程的缓冲区也会有内容也会刷新缓冲区，这就会发生上面重复打印的现象。**

在代码中加上fflush()强制刷新，会是什么现象？

```c
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    const char* msg = "call write\n";
    write(1, msg, strlen(msg));
    printf("call printf\n");
    fprintf(stdout, "call fprintf\n");
    fputs("call fputs\n", stdout);
    fflush(stdout);

    fork();
    return 0;
}

运行结果：
[cwx@VM-20-16-centos fd]$ ./mytest 
call write
call printf
call fprintf
call fputs
[cwx@VM-20-16-centos fd]$ ./mytest > myfile 
[cwx@VM-20-16-centos fd]$ cat myfile 
call write
call printf
call fprintf
call fputs
```

**当我们加上fflush()强制刷新，就不会出现重复打印的情况，这是因为在fork()创建子进程之前强制刷新，父进程的用户级缓冲区数据被刷新到内核级缓冲区，用户级缓冲区就没数据了，就不会发生写时拷贝，就不会重复打印。**


---

## FILE结构体具体实现

```c
typedef struct _IO_FILE FILE; 在/usr/include/stdio.h
```

```c
在/usr/include/libio.h
struct _IO_FILE {
int _flags; /* High-order word is _IO_MAGIC; rest is flags. */
#define _IO_file_flags _flags
//缓冲区相关
/* The following pointers correspond to the C++ streambuf protocol. */
/* Note: Tk uses the _IO_read_ptr and _IO_read_end fields directly. */
char* _IO_read_ptr; /* Current read pointer */
char* _IO_read_end; /* End of get area. */
char* _IO_read_base; /* Start of putback+get area. */
char* _IO_write_base; /* Start of put area. */
char* _IO_write_ptr; /* Current put pointer. */
char* _IO_write_end; /* End of put area. */
char* _IO_buf_base; /* Start of reserve area. */
char* _IO_buf_end; /* End of reserve area. */
/* The following fields are used to support backing up and undo. */
char *_IO_save_base; /* Pointer to start of non-current get area. */
char *_IO_backup_base; /* Pointer to first valid character of backup area */
char *_IO_save_end; /* Pointer to end of non-current get area. */
struct _IO_marker *_markers;
struct _IO_FILE *_chain;
int _fileno; //封装的文件描述符
#if 0
int _blksize;
#else
int _flags2;
#endif
_IO_off_t _old_offset; /* This used to be _offset but it's too small. */
#define __HAVE_COLUMN /* temporary */
/* 1+column number of pbase(); 0 is unknown. */
unsigned short _cur_column;
signed char _vtable_offset;
char _shortbuf[1];
/* char* _save_gptr; char* _save_egptr; */
_IO_lock_t *_lock;
#ifdef _IO_USE_OLD_IO_FILE
};
```

