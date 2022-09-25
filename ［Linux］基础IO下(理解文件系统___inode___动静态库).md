![在这里插入图片描述](https://img-blog.csdnimg.cn/5ee93720470047a8b769945644e0cb03.png)

@[TOC](文章目录)

# 理解文件系统

当ls -l查看文件信息时，除了看到文件名，还可以看文件元数据

```c
[cwx@VM-20-16-centos tmp]$ ls -l
total 0
-rw-rw-r-- 1 cwx cwx 0 Aug 14 17:34 file.txt

每行包含7列：
模式 硬链接数 文件所有者 组 大小 最后修改时间 文件名
```
stat命令能够看到更多信息：

```c
[cwx@VM-20-16-centos tmp]$ stat file.txt 
  File: ‘file.txt’
  Size: 0         	Blocks: 0          IO Block: 4096   regular empty file
Device: fd01h/64769d	Inode: 1705873     Links: 1
Access: (0664/-rw-rw-r--)  Uid: ( 1001/     cwx)   Gid: ( 1001/     cwx)
Access: 2022-08-14 17:34:46.317025937 +0800
Modify: 2022-08-14 17:34:46.317025937 +0800
Change: 2022-08-14 17:34:46.317025937 +0800
 Birth: -
```

---


在基础IO上谈到的都是一个文件被打开后的情况，下面我们要谈谈如果一个文件没有被打开呢？

**文件没有被打开时，是被保存在磁盘的。文件=文件内容+文件属性，如果一个空文件被创建出来，这个空文件的属性也是要占磁盘的空间的。
磁盘分为机械硬盘和固态硬盘。在机械硬盘中写入，本质上是写入到机械硬盘的盘片上，磁盘写入的基本单位是：扇区(512字节)，扇区是盘片上的一圈磁道上同半径的小块单元，不同盘片的磁道上下形成一个柱面。磁盘定位内容的过程是先定位到盘片，在对应到哪一个磁道的哪个一块扇区。操作系统把文件写入到磁盘，实际上就是把数据写入到磁盘盘片上。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/20fb184863eb4ee09a158ae50d2d381c.png)

**盘片可以想象成像磁带一样一圈一圈的结构，展开之后就是一个线性的结构，在线性结构中分为一个一个的扇区，每个扇区对应着一个逻辑区块地址(LBA)，通过LBA寻址的方式，可以对应到某个盘片的某个磁道的某个扇区。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/d9ae315612d4406ba6f9fa4cd07ce353.png)


---


# inode
**磁盘的空间非常大，管理的成本非常高，操作系统在管理磁盘的时候，会将磁盘偌大的空间分为一个一个的小空间**，比如我们的Windows操作系统，本质上在硬件层面上只有一个固态硬盘，但是我们可以把硬盘分为C盘，D盘，E盘等等，这就降低了我们的管理成本，提高了效率。


![在这里插入图片描述](https://img-blog.csdnimg.cn/febe67eefee047c6a1dcfe0a97efda09.png)

**这是Linux特有的EXT系列的文件系统，每个分区的开始是BootBlock，这个区域是与启动相关的，启动分区每一个分区都有，一般在每个盘片的第一个扇区当中。
分区之后每个分区在BootBlock之后又将分区分成若干个小空间(Block group 0-n)，每个Block group都包含着一些字段：**
+ **Super Block**: 代表当前Block group的空间使用情况，包括文件系统相关类型信息，包括inode/block的总量、使用量、剩余量, 以及文件系统的格式与相关信息等，每个 block group 都可能含有 superblock ，事实上除了第一个 block group 内会含有 superblock 之外，后续的 block group 不一定含有 superblock ，而若含有 superblock 则该 superblock 主要是做为第一个 block group 内 superblock 的备份。 
+ **Group Descriptor Table**: 块组描述符，描述块组属性信息，该字段与当前组(Block group)相关，包含当前组的描述信息，包括组的标识符，组的总空间，起始位置和结束位置等等。
+ **inode bitmap**: 记录使用与未使用的 inode 号码,并在进行文件添加修改时候对应的修改 inode 的使用状况，比特位的位置代表inode编号，比特位的值(0或1)代表该inode是否被使用。
+ **block bitmap**: 记录使用与未使用的 block 号码,并在进行文件添加修改时候对应的修改 block 的使用状况，比特位的位置代表block编号，比特位的值(0或1)代表该block是否被使用。
+ **inode table**: 包含一个个数据块，保存文件的属性信息，该文件的存取模式(read/write/excute)，该文件的拥有者与群组(owner/group)，该文件的容量，该文件创建或状态改变的时间(ctime)，inode和block的映射关系等等。
+ **Data block**: 包含一个个数据块，保存文件的内容，每个 block 内最多只能够放置一个文件的数据，若文件大于 block 的大小,则一个文件会占用多个 block 数量，若文件小于 block ,则该 block 的剩余容量就不能够再被使用了。

![在这里插入图片描述](https://img-blog.csdnimg.cn/4b4323e25d784bbe96a0166af050bbb5.png)

---

当我们创建一个文件时，并向文件写入字符串，经历了哪些过程？

**首先操作系统要先遍历inode bitmap找到未使用的inode号码，将该比特位的值改为已使用。再遍历block bitmap找到未使用的block号码，将该比特位的值改为已使用。将inode编号在inode table所对应的数据块，写入文件的属性，通过数据块里的inode和block的映射关系，找到Data block里对应的数据块写入字符串。**

- **每一个文件都有独立的inode，在Linux下，文件名在系统层面上每一意义，Linux下真正标识一个文件，是通过文件的inode编号。**
![在这里插入图片描述](https://img-blog.csdnimg.cn/2cce739de20a4592928e8b77b88d9755.png)

- **在Linux下一切皆文件，目录也是一个文件，目录也有独立的inode，目录也有数据块，里面包含着文件名和inode的映射关系（文件名:inode）。**
![在这里插入图片描述](https://img-blog.csdnimg.cn/28e00d8dd28c4f7eb6bbee32f685e387.png)
---

解析下列代码在系统层面具体细节：
```c
[cwx@VM-20-16-centos tmp]$ touch hello.c
[cwx@VM-20-16-centos tmp]$ echo "hello world" > hello.c 
[cwx@VM-20-16-centos tmp]$ cat hello.c 
hello world
[cwx@VM-20-16-centos tmp]$ ls -l -i
total 4
1705873 -rw-rw-r-- 1 cwx cwx 12 Aug 14 15:51 hello.c
[cwx@VM-20-16-centos tmp]$ rm hello.c 

touch hello.c该指令会创建文件申请inode，
将(hello.c:1705873)这对映射关系写入tmp目录的数据块里。

cat hello.c该指令会先查看tmp目录的数据块的映射关系，找到文件对
应的inode编号，再通过inode编号找到存放文件属性的inode数据块，
通过inode和block的映射关系，找到文件内容的数据块，打印文件信息。

ls -l -i该指令会先查看tmp目录的数据块的映射关系，找到文件对应的
inode编号，再通过inode编号找到存放文件属性的inode数据块，打印
文件属性信息。

rm hello.c该指令先查看tmp目录的数据块的映射关系，找到文件
对应的inode编号，再对应到inode bitmap的比特位由1置0，再从
inode数据块里的inode与block的映射关系，找到block编号，再
对应到block bitmap的比特位由1置0
```

---
# 理解软硬链接
## 软链接
**添加软链接：**

```c
[cwx@VM-20-16-centos soft_hard_link]$ ln -s file.txt file_soft
[cwx@VM-20-16-centos soft_hard_link]$ ls -l -i
total 0
1705881 lrwxrwxrwx 1 cwx cwx 8 Aug 15 14:51 file_soft -> file.txt
1705880 -rw-rw-r-- 1 cwx cwx 0 Aug 15 14:50 file.txt
```

**删除软链接：**

```c
[cwx@VM-20-16-centos soft_hard_link]$ unlink file_soft 
[cwx@VM-20-16-centos soft_hard_link]$ ll
total 0
-rw-rw-r-- 1 cwx cwx 0 Aug 15 14:50 file.txt
```

**软链接运用场景：**

**如果我们当前的工作目录是soft_hard_link，我们要在当前工作目录下运行bin/sbin/test/下的可执行程序test，做法是：**

```c
[cwx@VM-20-16-centos soft_hard_link]$ ll
total 4
drwxrwxr-x 3 cwx cwx 4096 Aug 15 14:57 bin
[cwx@VM-20-16-centos soft_hard_link]$ ./bin/sbin/test/test 
hello world
```
**这样的做法太过麻烦，我们可以创建一个软链接：**

```c
[cwx@VM-20-16-centos soft_hard_link]$ ln -s bin/sbin/test/test test_soft
[cwx@VM-20-16-centos soft_hard_link]$ ll
total 4
drwxrwxr-x 3 cwx cwx 4096 Aug 15 14:57 bin
lrwxrwxrwx 1 cwx cwx   18 Aug 15 15:02 test_soft -> bin/sbin/test/test
[cwx@VM-20-16-centos soft_hard_link]$ ./test_soft 
hello world
```
**软链接就相当于Windows操作系统下的快捷方式。**

```c
[cwx@VM-20-16-centos soft_hard_link]$ ls -l -i
total 0
1705881 lrwxrwxrwx 1 cwx cwx 8 Aug 15 14:51 file_soft -> file.txt
1705880 -rw-rw-r-- 1 cwx cwx 0 Aug 15 14:50 file.txt
```
**软链接是独立的文件，拥有自己独立的inode，拥有独立的inode属性集和数据块，数据块中保存着指向文件的路径和文件名，在目录下也存在软链接和inode的映射关系。**

---

## 硬链接
**添加硬链接：**

```c
[cwx@VM-20-16-centos soft_hard_link]$ ln file.txt file_hard
[cwx@VM-20-16-centos soft_hard_link]$ ls -l -i
total 4
1705881 -rw-rw-r-- 2 cwx cwx    0 Aug 15 15:20 file_hard
1705881 -rw-rw-r-- 2 cwx cwx    0 Aug 15 15:20 file.txt
```
**我们可以硬链接和原文件的inode相等，硬链接不是一个独立的文件，而是一个文件名和inode编号的映射关系，因为硬链接没有独立的inode。创建硬链接的本质是在特定目录下，填写一对文件名和inode的映射关系。**

**硬链接数：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/e4a3a3a2ff7c4e4c8b40154e61a30c57.png)

当我们新创建一个文件时，文件的默认硬链接数是1，当我们添加硬链接时，硬链接数就会发生改变：
![在这里插入图片描述](https://img-blog.csdnimg.cn/7bc11a6c31bb4ba5abd17ff2e29f0d03.png)

**硬链接数的本质是有多少个文件名指向对应的inode，硬链接数保存在inode属性集里，表示有多少个文件指向该inode，创建硬链接本质是硬链接数加一，删除文件或硬链接就减一，如果硬链接数为0，文件就会被删除，这是引用计数的原理。**

**目录的硬链接数：**

当我们创建一个目录，默认的硬链接数是2：

```c
[cwx@VM-20-16-centos soft_hard_link]$ mkdir dir
[cwx@VM-20-16-centos soft_hard_link]$ ls -l -i -d dir/
1705887 drwxrwxr-x 2 cwx cwx 4096 Aug 15 15:44 dir/
```
这是因为dir目录下，.指向的也是dir目录，inode值一致：

```c
[cwx@VM-20-16-centos soft_hard_link]$ mkdir dir
[cwx@VM-20-16-centos soft_hard_link]$ ls -l -i -d dir/
1705887 drwxrwxr-x 2 cwx cwx 4096 Aug 15 15:44 dir/
[cwx@VM-20-16-centos soft_hard_link]$ ls -l -i -d dir/.
1705887 drwxrwxr-x 2 cwx cwx 4096 Aug 15 15:44 dir/.
```
如果我们在dir目录下再创建一个目录，dir的硬链接数会变为3：

```c
[cwx@VM-20-16-centos soft_hard_link]$ ls -l -i -d dir/
1705887 drwxrwxr-x 2 cwx cwx 4096 Aug 15 15:49 dir/
[cwx@VM-20-16-centos soft_hard_link]$ mkdir dir/tmp
[cwx@VM-20-16-centos soft_hard_link]$ ls -l -i -d dir/
1705887 drwxrwxr-x 3 cwx cwx 4096 Aug 15 15:49 dir/
```
这是因为在dir/tmp目录下，..指向的也是dir，inode值一致：

```c
[cwx@VM-20-16-centos soft_hard_link]$ ls -l -i -d dir/
1705887 drwxrwxr-x 3 cwx cwx 4096 Aug 15 15:49 dir/
[cwx@VM-20-16-centos soft_hard_link]$ ls -l -i -d dir/.
1705887 drwxrwxr-x 3 cwx cwx 4096 Aug 15 15:49 dir/.
[cwx@VM-20-16-centos soft_hard_link]$ ls -l -i -d dir/tmp/..
1705887 drwxrwxr-x 3 cwx cwx 4096 Aug 15 15:49 dir/tmp/..
```

---
# 文件的三个时间

 - **Access 最后访问时间**
 - **Modify 文件内容最后修改时间**
 - **Change 属性最后修改时间**

stat命令查看文件的三个时间：

```c
[cwx@VM-20-16-centos dir]$ touch file.txt
[cwx@VM-20-16-centos dir]$ stat file.txt 
  File: ‘file.txt’
  Size: 0         	Blocks: 0          IO Block: 4096   regular empty file
Device: fd01h/64769d	Inode: 1705885     Links: 1
Access: (0664/-rw-rw-r--)  Uid: ( 1001/     cwx)   Gid: ( 1001/     cwx)
Access: 2022-08-15 16:13:08.574539121 +0800
Modify: 2022-08-15 16:13:08.574539121 +0800
Change: 2022-08-15 16:13:08.574539121 +0800
 Birth: -
```

---

## Modify
**Modify表示最近一次文件内容的时间。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/21a76ee8bd8a4e5d9c093bbffc91c3e4.png)

---

## Change
**Change表示最近一个文件属性的时间，当我们修改文件内容时，有可能会修改文件的属性，比如会更改文件的大小属性。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/24881bd3bf8a48c2b2dad032707add33.png)

---
## Access
**Access表示文件最近被访问的时间。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/c715f435001f487da4b5d6394175c6d1.png)
可在实际访问过程中，我们发现Access时间并没有改变，为什么？

**在相对较新的Linux内核中，Access时间不会被立即更新，而是有一定的时间间隔，操作系统才会自动进行更新。当我们进行文件操作时，文件访问时最高频的，如果我们频繁更新Access时间，就有可能导致系统出现"刷盘"问题，导致系统变慢，所以新的内核做了优化，防止与磁盘进行过多的交互，导致效率变低。**

---

## 时间与编译的关系

当我们在编写Makefile后，进行make编译时，make第二次时，系统就不让我们编译了，而更新了内容后，又可以再次make编译：

```c
[cwx@VM-20-16-centos dir]$ ls
makefile  test.c
[cwx@VM-20-16-centos dir]$ make
gcc -o mytest test.c
[cwx@VM-20-16-centos dir]$ make
make: `mytest' is up to date.
[cwx@VM-20-16-centos dir]$ vim test.c
[cwx@VM-20-16-centos dir]$ make
gcc -o mytest test.c
```
操作系统是如何识别的呢？

```c
[cwx@VM-20-16-centos dir]$ vim test.c 
[cwx@VM-20-16-centos dir]$ stat mytest 
  File: ‘mytest’
  Size: 8360      	Blocks: 24         IO Block: 4096   regular file
Device: fd01h/64769d	Inode: 1705888     Links: 1
Access: (0775/-rwxrwxr-x)  Uid: ( 1001/     cwx)   Gid: ( 1001/     cwx)
Access: 2022-08-15 16:43:49.385110329 +0800
Modify: 2022-08-15 16:43:49.202109280 +0800
Change: 2022-08-15 16:43:49.202109280 +0800
 Birth: -
 
[cwx@VM-20-16-centos dir]$ stat test.c 
  File: ‘test.c’
  Size: 108       	Blocks: 8          IO Block: 4096   regular file
Device: fd01h/64769d	Inode: 1705885     Links: 1
Access: (0664/-rw-rw-r--)  Uid: ( 1001/     cwx)   Gid: ( 1001/     cwx)
Access: 2022-08-15 16:48:22.111673180 +0800
Modify: 2022-08-15 16:48:22.108673162 +0800
Change: 2022-08-15 16:48:22.108673162 +0800
 Birth: -
```
**操作系统会对比可执行程序和源文件的Modify时间，如果可执行程序的Modify时间比源文件的Modify时间晚，就可以再次编译，反之则不能再次编译。
Makefile和gcc会根据时间问题，来判定源文件和可执行程序的内容修改时间谁更新，从而指导系统哪些源文件需要被重新编译。**

---

# 动态库和静态库

 - **静态库（.a）：程序在编译链接的时候把库的代码链接到可执行文件中。程序运行的时候将不再需要静态库。**
 - **动态库（.so）：程序在运行的时候才去链接动态库的代码，多个程序共享使用库的代码。**
 - **一个与动态库链接的可执行文件仅仅包含它用到的函数入口地址的一个表，而不是外部函数所在目标文件的整个机器码**
 - **在可执行文件开始运行以前，外部函数的机器码由操作系统从磁盘上的该动态库中复制到内存中，这个过程称为动态链接（dynamic linking）**
 - **动态库可以在多个程序间共享，所以动态链接使得可执行文件更小，节省了磁盘空间。操作系统采用虚拟内存机制允许物理内存中的一份动态库被要用到该库的所有进程共用，节省了内存和磁盘空间。**


---

**gcc动态链接编译：**

```c
[cwx@VM-20-16-centos test]$ gcc -o mytest mytest.c 
[cwx@VM-20-16-centos test]$ ls -l
total 16
-rwxrwxr-x 1 cwx cwx 8360 Aug 15 17:06 mytest
-rw-rw-r-- 1 cwx cwx   79 Aug 15 17:06 mytest.c
[cwx@VM-20-16-centos test]$ file mytest
mytest: ELF 64-bit LSB executable, x86-64, version 1 (SYSV),
dynamically linked (uses shared libs), for GNU/Linux 2.6.32,
BuildID[sha1]=64bf6118050bfc487f2a814f055c4fda1bea52f4, not
stripped
```

**gcc静态链接编译：**

```c
[cwx@VM-20-16-centos test]$ gcc -o mytest_static mytest.c -static
[cwx@VM-20-16-centos test]$ ls -l
total 860
-rwxrwxr-x 1 cwx cwx   8360 Aug 15 17:06 mytest
-rw-rw-r-- 1 cwx cwx     79 Aug 15 17:06 mytest.c
-rwxrwxr-x 1 cwx cwx 861288 Aug 15 17:06 mytest_static
[cwx@VM-20-16-centos test]$ file mytest_static 
mytest_static: ELF 64-bit LSB executable, x86-64, version 1
(GNU/Linux), statically linked, for GNU/Linux 2.6.32,
BuildID[sha1]=371a6525dc2e37230a1f284fa8fdbad6a252e3fb, not
stripped

```

---

**库文件的命名：**

动态库：libXXX.so
静态库：libXXX.a-XXX
库的真实名字：去掉lib前缀，去掉.a- or so-后缀之后，就是库名称

![在这里插入图片描述](https://img-blog.csdnimg.cn/f7696879d78849e4abce1b374ac22cd9.png)

---
**测试程序：**

```c
----------------add.c------------------
#include "add.h"

int my_add(int x, int y)
{
    return x + y;
}
----------------add.h------------------
#include <stdio.h>

extern int my_add(int x, int y);
----------------sub.c------------------
#include "sub.h"

int my_sub(int x, int y)
{
    return x - y;
}
----------------sub.h------------------
#include <stdio.h>

extern int my_sub(int x, int y);

```

---

## 制作静态库
如果要将上述四个文件打包成静态库，应该怎么做？

**Makefile文件：**

```c
libmymath.a:sub.o add.o
	ar -rc $@ $^ 
#ar是gnu归档工具，rc表示(replace and create)
%.o:%.c
	gcc -c $<

.PHONY:clean
clean:
	rm -rf libmymath.a *.o lib

.PHONY:lib
lib:
	mkdir lib
	cp libmymath.a *.h lib
#打包add.h、sub.h和静态库libmymath.a
```
**make编译链接后，生成libmymath.a静态库：**

```c
[cwx@VM-20-16-centos test_lib]$ make
gcc -c sub.c
gcc -c add.c
ar -rc libmymath.a sub.o add.o
[cwx@VM-20-16-centos test_lib]$ ll
total 32
-rw-rw-r-- 1 cwx cwx   65 Jul 15 17:24 add.c
-rw-rw-r-- 1 cwx cwx   53 Jul 15 17:24 add.h
-rw-rw-r-- 1 cwx cwx 1240 Aug 15 17:24 add.o
-rw-rw-r-- 1 cwx cwx 2694 Aug 15 17:24 libmymath.a
-rw-rw-r-- 1 cwx cwx  174 Aug 15 17:17 Makefile
-rw-rw-r-- 1 cwx cwx   65 Jul 15 17:24 sub.c
-rw-rw-r-- 1 cwx cwx   53 Jul 15 17:24 sub.h
-rw-rw-r-- 1 cwx cwx 1240 Aug 15 17:24 sub.o
```
**make output，打包add.h、sub.h和静态库libmymath.a到lib目录：**

```c
[cwx@VM-20-16-centos test_lib]$ make lib
mkdir lib
cp libmymath.a *.h lib
[cwx@VM-20-16-centos test_lib]$ ll lib/
total 12
-rw-rw-r-- 1 cwx cwx 1240 Aug 15 17:47 add.h
-rw-rw-r-- 1 cwx cwx 2694 Aug 15 17:47 libmymath.a
-rw-rw-r-- 1 cwx cwx 1240 Aug 15 17:47 sub.h
```
**查看静态库中的目录列表：**

```c
[cwx@VM-20-16-centos test_lib]$ ar -tv libmymath.a 
rw-rw-r-- 1001/1001   1240 Aug 15 19:19 2022 sub.o
rw-rw-r-- 1001/1001   1240 Aug 15 19:19 2022 add.o
```

---

## 使用静态库

当我们要在use_lib目录下使用打包好的静态库：

```c
[cwx@VM-20-16-centos use_lib]$ ls
lib  test.c
[cwx@VM-20-16-centos use_lib]$ tree
.
|-- lib
|   |-- add.h
|   |-- libmymath.a
|   `-- sub.h
`-- test.c

1 directory, 4 files
[cwx@VM-20-16-centos use_lib]$ cat test.c 
#include "add.h"
#include "sub.h"

int main()
{
    int x = 10;
    int y = 20;

    printf("add: %d\n", add(x,y));
    printf("sub: %d\n", sub(x,y));

    return 0;
}
```
当我们链接编译时，会发现系统报错：

```c
[cwx@VM-20-16-centos use_lib]$ gcc test.c 
test.c:1:17: fatal error: add.h: No such file or directory
 #include "add.h"
                 ^
compilation terminated.
```
原因是编译器找不到头文件，需要添加库搜索路径和指定库名称：
![在这里插入图片描述](https://img-blog.csdnimg.cn/d121b1accb4c424d864a502b32edb487.png)

```c
[cwx@VM-20-16-centos use_lib]$ gcc test.c -I./lib -L./lib -lmymath
[cwx@VM-20-16-centos use_lib]$ ll
total 20
-rwxrwxr-x 1 cwx cwx 8360 Aug 15 20:17 a.out
drwxrwxr-x 2 cwx cwx 4096 Aug 15 19:55 lib
-rw-rw-r-- 1 cwx cwx  223 Aug 15 20:17 test.c
[cwx@VM-20-16-centos use_lib]$ ./a.out 
add: 30
sub: -10
```
之前写的代码，也运用了库，为什么没有指明选项呢？

**因为之前的库都在系统的默认路径下：/lib64，/usr/include等，编译器是可以识别这些路径的，如果我们自己打包的库也不想带选项，可以把对应的库和头文件拷贝到默认路径下，但是不推荐这么做，避免污染库。**

---
**库搜索路径：**

 - 从左到右搜索-L指定的目录
 - 由环境变量指定的目录 （LIBRARY_PATH）
 - 由系统指定的目录（/usr/lib，/usr/local/lib）
---

## 制作动态库

- shared: 表示生成共享库格式
 - fPIC：产生位置无关码(position independent code)

**Makefile文件：**

```c
libmymath.so:add.o sub.o
	gcc -shared -o $@ $^
%.o:%.c
	gcc -fPIC -c $<

.PHONY:clean
clean:
	rm -f libmymath.so *.o

.PHONY:lib
lib:
	mkdir lib
	cp *.h libmymath.so lib
```
**make编译链接：**

```c
[cwx@VM-20-16-centos dynamic_libraries]$ make
gcc -fPIC -c add.c
gcc -fPIC -c sub.c
gcc -shared -o libmymath.so add.o sub.o
[cwx@VM-20-16-centos dynamic_libraries]$ ll
total 36
-rw-rw-r-- 1 cwx cwx   65 Aug 15 20:42 add.c
-rw-rw-r-- 1 cwx cwx   53 Aug 15 20:42 add.h
-rw-rw-r-- 1 cwx cwx 1240 Aug 15 21:04 add.o
-rwxrwxr-x 1 cwx cwx 7944 Aug 15 21:04 libmymath.so
-rw-rw-r-- 1 cwx cwx  169 Aug 15 21:04 makefile
-rw-rw-r-- 1 cwx cwx   65 Aug 15 20:42 sub.c
-rw-rw-r-- 1 cwx cwx   53 Aug 15 20:42 sub.h
-rw-rw-r-- 1 cwx cwx 1240 Aug 15 21:04 sub.o
```
**make lib打包动态库：**

```c
[cwx@VM-20-16-centos dynamic_libraries]$ make lib
mkdir lib
cp *.h libmymath.so lib
[cwx@VM-20-16-centos dynamic_libraries]$ ll
total 40
-rw-rw-r-- 1 cwx cwx   65 Aug 15 20:42 add.c
-rw-rw-r-- 1 cwx cwx   53 Aug 15 20:42 add.h
-rw-rw-r-- 1 cwx cwx 1240 Aug 15 21:04 add.o
drwxrwxr-x 2 cwx cwx 4096 Aug 15 21:05 lib
-rwxrwxr-x 1 cwx cwx 7944 Aug 15 21:04 libmymath.so
-rw-rw-r-- 1 cwx cwx  169 Aug 15 21:04 makefile
-rw-rw-r-- 1 cwx cwx   65 Aug 15 20:42 sub.c
-rw-rw-r-- 1 cwx cwx   53 Aug 15 20:42 sub.h
-rw-rw-r-- 1 cwx cwx 1240 Aug 15 21:04 sub.o
```
---

## 使用动态库
**如果我们和使用静态库一样使用动态库，编译可以通过，但是运行程序时会报错：**

```c
[cwx@VM-20-16-centos use_lib]$ clear 
[cwx@VM-20-16-centos use_lib]$ make
gcc -o mytest mytest.c -I./lib -L./lib -lmymath
[cwx@VM-20-16-centos use_lib]$ ll
total 24
drwxrwxr-x 2 cwx cwx 4096 Aug 15 21:05 lib
-rw-rw-r-- 1 cwx cwx   89 Aug 15 21:18 makefile
-rwxrwxr-x 1 cwx cwx 8440 Aug 15 21:18 mytest
-rw-rw-r-- 1 cwx cwx  214 Aug 15 21:17 mytest.c
[cwx@VM-20-16-centos use_lib]$ ./mytest 
./mytest: error while loading shared libraries: libmymath.so: cannot open shared object file: No such file or directory
```
**ldd mytest查看链接情况：**

```c
[cwx@VM-20-16-centos use_lib]$ ldd mytest
	linux-vdso.so.1 =>  (0x00007ffe6e3ba000)
	libmymath.so => not found
	libc.so.6 => /lib64/libc.so.6 (0x00007f2a393f6000)
	/lib64/ld-linux-x86-64.so.2 (0x00007f2a397c4000)
```
我们会发现我们所依赖libmymath.so的库没有被系统找到，<kbd>gcc -o mytest mytest.c -I./lib -L./lib -lmymath</kbd>只是告知编译器头文件和库路径在哪里，但是当程序编译完成后，就与编译无关，需要在运行的时候，进一步告知系统库的位置：

 1、**将库和头文件拷贝到默认路径**
 
 2、**更改<kbd> LD_LIBRARY_PATH</kbd>**

```c
[cwx@VM-20-16-centos lib]$ pwd
/home/cwx/learn/lesson16/test_lib/dynamic_libraries/use_lib/lib

#更改LD_LIBRARY_PATH
[cwx@VM-20-16-centos use_lib]$ echo $LD_LIBRARY_PATH
/home/cwx/learn/lesson16/test_lib/dynamic_libraries/use_lib/lib
[cwx@VM-20-16-centos lib]$ export LD_LIBRARY_PATH=/home/cwx/learn/lesson16/test_lib/dynamic_libraries/use_lib/lib
[cwx@VM-20-16-centos use_lib]$ ./mytest 
add: 30
sub: -10
```
**ldd命令查看链接情况：**

```c
/home/cwx/learn/lesson16/test_lib/dynamic_libraries/use_lib/lib
[cwx@VM-20-16-centos use_lib]$ ldd mytest
	linux-vdso.so.1 =>  (0x00007ffce1b14000)
	libmymath.so => /home/cwx/learn/lesson16/test_lib/dynamic_libraries/use_lib/lib/libmymath.so (0x00007fc9b0960000)
	libc.so.6 => /lib64/libc.so.6 (0x00007fc9b0592000)
	/lib64/ld-linux-x86-64.so.2 (0x00007fc9b0b62000)
```


 
3、**ldconfig 配置/etc/ld.so.conf.d/，ldconfig更新**

```c
[cwx@VM-20-16-centos use_lib]$ cd /etc/ld.so.conf.d/ -d
[root@VM-20-16-centos ld.so.conf.d]# touch hello.conf
[root@VM-20-16-centos ld.so.conf.d]# echo "/home/cwx/learn/lesson16/test_lib/dynamic_libraries/use_lib/lib" > hello.conf 
[cwx@VM-20-16-centos use_lib]$ sudo ldconfig
[cwx@VM-20-16-centos use_lib]$ unset LD_LIBRARY_PATH
[cwx@VM-20-16-centos use_lib]$ echo $LD_LIBRARY_PATH

[cwx@VM-20-16-centos use_lib]$ ./mytest 
add: 30
sub: -10
```
**ldd命令查看链接情况：**

```c
/home/cwx/learn/lesson16/test_lib/dynamic_libraries/use_lib/lib
[cwx@VM-20-16-centos use_lib]$ ldd mytest
	linux-vdso.so.1 =>  (0x00007ffce1b14000)
	libmymath.so => /home/cwx/learn/lesson16/test_lib/dynamic_libraries/use_lib/lib/libmymath.so (0x00007fc9b0960000)
	libc.so.6 => /lib64/libc.so.6 (0x00007fc9b0592000)
	/lib64/ld-linux-x86-64.so.2 (0x00007fc9b0b62000)
```

---
