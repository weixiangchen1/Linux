![在这里插入图片描述](https://img-blog.csdnimg.cn/5ee93720470047a8b769945644e0cb03.png)
@[TOC](文章目录)

# 进程优先级
## 基本概念

 - **cpu资源分配的先后顺序，就是指进程的优先权（priority）**。
 - 优先权高的进程有优先执行权利。配置进程优先权对多任务环境的linux很有用，可以改善系统性能。
 - 还可以把进程运行到指定的CPU上，这样一来，把不重要的进程安排到某个CPU，可以大大改善系统整体性能。
---
## 查看系统进程
在linux或者unix系统中，用ps –l命令则会类似输出以下几个内容：

![在这里插入图片描述](https://img-blog.csdnimg.cn/f5467f59c1ee4ff4a5cd2a94d55450d2.png)

 - UID : 代表执行者的身份
 - PID : 代表这个进程的代号
 - PPID ：代表这个进程是由哪个进程发展衍生而来的，亦即父进程的代号
 - PRI ：代表这个进程可被执行的优先级，其值越小越早被执行
 - NI ：代表这个进程的nice值
----
## PRI和NI
- **PRI即进程的优先级**，换句话讲就是进程被CPU执行的先后顺序，**PRI的值越小优先级越高，即越先被调度**。
- **NI即进程的nice值**，表示**进程可被执行的优先级的修正数值**。
- nice值的范围为-20-19，一共40个级别，当nice值为负数时，进程的优先级值将变小，即优先级变高，当nice值为正数时，进程的优先级变大，即优先级变低。所以，**在Linux下，调整进程的优先级就是调整nice值**。
- **PRI计算公式：PRI(new) = PRI(old) + nice**
---
## top命令更改已存在进程的nice值
1. 进入top
![在这里插入图片描述](https://img-blog.csdnimg.cn/c9a628c1b34944ea9292be7923287378.png)

2. 进入top后按"r"
![在这里插入图片描述](https://img-blog.csdnimg.cn/a88af3ce04d146efa3b3b7ac6f47ca5b.png)

3. 输入进程PID
![在这里插入图片描述](https://img-blog.csdnimg.cn/f3b456281b084312a30623c2774bbbb6.png)

4. 输入nice值
![在这里插入图片描述](https://img-blog.csdnimg.cn/7615ceafee9a4d2d960fbf3cbeea85c3.png)

更改nice值前的优先级：
![在这里插入图片描述](https://img-blog.csdnimg.cn/59dc7aa32a4644dcae0f24e32d743a78.png)
更改nice值后的优先级：
![在这里插入图片描述](https://img-blog.csdnimg.cn/ed1c6c913e1e46bf87808339c792c05a.png)

---
**nice值为什么是一个比较小的范围呢？**
 - 不管优先级再怎么设置，**进程的优先级也只是一种相对的优先级，不是绝对的优先级**，操作系统要保证每一个进程可以享受到系统的CPU的资源，否则会出想很严重的进程"饥饿问题"。

---

# 环境变量
## 基本概念
 - **环境变量(environment variables)一般是指在操作系统中用来指定操作系统运行环境的一些参数**。
 - 如：我们在编写C/C++代码的时候，在链接的时候，从来不知道我们的所链接的动态静态库在哪里，但是照样可以链接成功，生成可执行程序，**原因就是有相关环境变量帮助编译器进行查找**。
 - 环境变量通常具有某些特殊用途，还有在系统当中通常具有**全局特性**。
 - **语言上定义变量的本质是在内存中开辟空间，环境变量的本质是操作系统(OS)在内存/磁盘文件中开辟空间，用来保存系统相关的数据。**
---
## 常见环境变量
 - PATH:指定命令搜索路径。
 - HOME:指定用户的主工作目录(即用户登陆到Linux系统中时,默认的目录)。
 - SHELL:当前Shell,它的值通常是/bin/bash。

![在这里插入图片描述](https://img-blog.csdnimg.cn/234269de0e1b4df6a95e12f5b995a738.png)

---
## 测试PATH
我们自己编写的程序，命令，工具都是一个可以执行的工具，当我们在执行自己写的C语言程序时，需要带上./myproc，但是我们在执行命令的时候，却不用带上具体的路径，这是为什么呢？
![在这里插入图片描述](https://img-blog.csdnimg.cn/7ed13286710742fb8f7f9f9b25581157.png)

**本质上时因为有环境变量的存在。**

将我们的程序所在路径加入环境变量PATH当中，就可以实现不带具体的路径就可以运行我们自己编写的可执行程序：

![在这里插入图片描述](https://img-blog.csdnimg.cn/c02cba5203ce4036bae9aadadd582642.png)

---
## 测试HOME
root和普通用户的家目录是不一样的。
![在这里插入图片描述](https://img-blog.csdnimg.cn/995472081fc144d7b8f79fe1310d8e1b.png)

---
## 环境变量相关的命令

 - **echo**: 显示某个环境变量值
 - **export**: 设置一个新的环境变量
 -  **env**: 显示所有环境变量
 - **unset**: 清除环境变量
 - **set**: 显示本地定义的shell变量和环境变量
---

## 环境变量的组织方式
![在这里插入图片描述](https://img-blog.csdnimg.cn/c4803c7daf7848619f3a64960a421cda.png)
**每个程序都会收到一张环境表，环境表是一个字符指针数组，每个指针指向一个以’\0’结尾的环境字符串。**

---

## 获取环境变量

 1. **命令行参数**

```c
#include <stdio.h>    
      
int main(int argc, char* argv[], char* env[])    
{    
    for(int i=0; env[i]; i++)    
    {    
        printf("env[%d]: %s\n", i, env[i]);                                                                                                                  
    } 
             
    return 0;    
}    

```

![在这里插入图片描述](https://img-blog.csdnimg.cn/bc76ea09a65244afbe2f5dd35c61f40d.png)


 2. **通过第三方变量environ获取**

```c
#include <stdio.h>    
    
int main()    
{    
    extern char **environ;    
    for(int i=0; environ[i]; i++){    
        printf("environ[%d]: %s\n", i, environ[i]);                                                                                                            
    }    
    
    return 0;    
} 
```
![在这里插入图片描述](https://img-blog.csdnimg.cn/fa861623a62e41dfa94dc459ea458444.png)

 3. **通过系统调用获取或设置环境变量**

```c
#include <stdio.h>    
#include <stdlib.h>    
     
int main()    
{    
    printf("%s\n", getenv("PATH"));    
    printf("%s\n", getenv("HOME"));    
    printf("%s\n", getenv("SHELL"));    
    printf("%s\n", getenv("USER"));                                                                                                                            
    
    return 0;    
} 
```

![在这里插入图片描述](https://img-blog.csdnimg.cn/a53ad42b4f124b949055830235abc1bb.png)

---

## 环境变量通常是具有全局属性的

 - 环境变量通常具有全局属性，可以被子进程继承下去

验证：
**在命令行上启动的进程，父进程都是bash**。所以说，当我们在bash定义环境变量，命令行上的程序也会继承bash定义环境变量。

定义环境变量MYENV之前，用echo命令查看是没有结果的：
![在这里插入图片描述](https://img-blog.csdnimg.cn/834f6f5537e64b3e9e471691d10c84bf.png)

编写程序验证:

```c
#include <stdio.h>    
#include <stdlib.h>    

int main()
{
    char* env = getenv("MYENV");
    if(env){
        printf("%s\n", env);
    }
                                                                                                                                                               
    return 0;                                                                                                                                      
}     
```
程序运行也是没有结果的，说明该环境变量根本不存在。

![在这里插入图片描述](https://img-blog.csdnimg.cn/60c70da801294933809d673e75763b45.png)
当我们导入环境变量MYENV:
![在这里插入图片描述](https://img-blog.csdnimg.cn/e13b06ca8d8b457cbaf20e4ff2ee39f3.png)

**所以说，环境变量通常具有全局属性，是可以被子进程继承下去的。**

---

