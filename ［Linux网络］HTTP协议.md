@[TOC](文章目录)
# HTTP简介
应用层协议可以是程序员自己制定的，但是实际上已经有很多优秀的程序员写出了现成的，非常好用的应用层协议供我们使用。HTTP协议就是其中之一。
**HTTP协议是Hyper Text Transfer Protocol（超文本传输协议）的缩写，是用于从万维网（WWW:World Wide Web ）服务器传输超文本到本地浏览器的传送协议。HTTP是一个基于TCP/IP通信协议来传递数据（文本信息，HTML 文件，图片文件等）。**

---

# 认识URL
平日我们常说的网址，就是**URL -- 统一资源定位符**。
**统一资源定位符(Universal Resource Locator，URL)，又叫做网页地址，是互联网上标准的资源的地址（Address）。互联网上的每个文件都有一个唯一的URL，它包含的信息指出文件的位置以及浏览器应该怎么处理它。**


![在这里插入图片描述](https://img-blog.csdnimg.cn/f39e407ab14642119c3108e0dd213e17.png)

我们访问网站，请求的图片视频、html、css、标签、文档等等这些都被称之为 "资源"。
**网站服务器的后台是Linux。前面我们谈过， ip + port 可以唯一确认网络中的一台主机，但是却无法确定网络中的一个"资源"。公网ip地址是可以确定网络中唯一一台主机的，网络资源都是保存在主机中的，Linux或者其他操作系统保存资源都是以文件的形式保存的，在Linux中，唯一标识文件的方式就是通过路径，所以我们通过 ip + Linux路径 就可以唯一确定网络中一个资源。**
ip通过域名的形式呈现，路径通过目录名+/文件呈现。比如我们访问百度主页https://www.baidu.com/index.html，ip以www.baidu.com的形式呈现，路径以/index.html的形式呈现。


---

# urlencode和urldecode
在url中，像 / ? : 等这样的字符,，已经被url当做特殊意义理解了。 因此这些字符不能随意出现。比如，某个参数中需要带有这些特殊字符，就必须先对特殊字符进行转义。

转义的规则如下: 
将需要转码的字符转为16进制，然后从右到左，取4位(不足4位直接处理)，每2位做一位，前面加上%，编码成%XY格式。

比如我们在百度搜索C++:

![在这里插入图片描述](https://img-blog.csdnimg.cn/4c9fed3e462145e5ac34679caf2bcb78.png)

通过urldecode解码工具：

![在这里插入图片描述](https://img-blog.csdnimg.cn/f4e4b7f5088e4cd09f3f093341e1b105.png)'+' 被转义成了 '%2B'，urlencode是urldecode的逆过程。

---
# HTTP协议格式
## HTTP请求
![在这里插入图片描述](https://img-blog.csdnimg.cn/847c95d948e049fdad7dc0db6a2a2bd5.png)
**HTTP请求由四部分组成：**

 1. **请求行**：请求方法 + url + http版本
 2. **请求报头**：HTTP请求的属性，**key : value结构，以冒号分割的键值对**，每一组属性以 '\n' 分隔，遇到空行表示请求报头结束。
 3. **空行**：**用于分隔报头和有效载荷，遇到空行表示请求报头结束。**
 4. **请求正文**：空行后面的内容都是请求正文，请求正文允许为空字符串。 如果请求正文存在，则在请求报头中会有一个**Content-Length属性来标识请求正文的长度**。

**HTTP如何进行解包和封装？**
当客户端收到HTTP请求时，对HTTP数据包进行读取，**当遇到空行时，表示请求报头结束，空行后的数据为请求正文即有效载荷，如果请求正文不为空，请求报头中有Content-Length表示请求正文的长度，通过一些字符串切割算法就可以有效得对报头和有效载荷进行分割。**

**HTTP请求或者响应，是怎么被读取的？**
HTTP请求和响应，在我们看来有不同的结构，但是在网络看来，HTTP请求和响应只是一个大的字符串。

---
## 实现一个简单的HTTP服务器
实现一个最简单的HTTP服务器，按照HTTP协议的要求构造请求报头和请求响应，在网页输出简单的登录界面。

**项目结构：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/677653698c4342cab3eadc5bb473ddc3.png)

**index.html：**

```html
<!DOCTYPE html>

<html>
    <head>
        <meta charset="utf-8">
    </head>
    <body>

        <form action="/" method="POST">
            姓名: <input type="text" name="name"><br/>
            密码: <input type="password" name="passwd"><br/>
            <input type="submit" value="登陆">
        </form>
    </body>
</html>
```

**Http.cc:**

```cpp
#include "Sock.hpp"
#include <pthread.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE 10240
#define WWWROOT "./wwwroot/"
#define HOME_PAGE "index.html"

void* HandlerHttpRequest(void* args)
{
    int sock = *(int*)args;
    delete (int*)args;  
    pthread_detach(pthread_self());

    char buffer[SIZE];
    memset(buffer, 0, sizeof(buffer));
    ssize_t s = recv(sock, buffer, sizeof(buffer), 0);
    if (s > 0) {
        buffer[s] = 0;
        cout << buffer;
        string html_file = WWWROOT;
        html_file += HOME_PAGE;

        ifstream in(html_file);
        if (!in.is_open()) {
            string http_response = "http/1.0 404 Not Found\n";
            http_response += "Content-Type: text/html; charset: utf-8\n";
            http_response += "\n";
            http_response += "<html><p>你访问的资源不存在</p></html>";
            send(sock, http_response.c_str(), http_response.size(), 0);
        }
        else {
            struct stat st;
            stat(html_file.c_str(), &st);
            string http_response = "http/1.0 200 OK\n";
            http_response += "Content-Type: text/html; charset: utf-8\n";
            http_response += "Content-Length: ";
            http_response += to_string(st.st_size);
            http_response += "\n\n";

            string content;
            string line;
            while (getline(in, line)) {
                content += line;
            }
            http_response += content;
            in.close();
            send(sock, http_response.c_str(), http_response.size(), 0);
        }
    }

    close(sock);
    return nullptr;
}

void Usage(string proc)
{
    cout << "Usage: " << proc << " port" << endl;
    exit(1);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        Usage(argv[0]);
    }

    uint16_t port = atoi(argv[1]);
    int listen_sock = Sock::Socket();
    Sock::Bind(listen_sock, port);
    Sock::Listen(listen_sock);

    for( ; ; ) {
        int sock = Sock::Accept(listen_sock);
        if (sock > 0) {
            pthread_t pid;
            int* parm = new int(sock);
            pthread_create(&pid, nullptr, HandlerHttpRequest, parm);
        }
    }

    return 0;
}
```
**Sock.hpp：**

```cpp
#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

class Sock
{
public:
    static int Socket()
    {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            cerr << "socket error" << endl;
            exit(2);
        }
        return sock;
    }

    static void Bind(int sock, uint16_t port)
    {
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;

        if (bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0)
        {
            cerr << "bind error!" << endl;
            exit(3);
        }
    }

    static void Listen(int sock)
    {
        if (listen(sock, 5) < 0)
        {
            cerr << "listen error !" << endl;
            exit(4);
        }
    }

    static int Accept(int sock)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int fd = accept(sock, (struct sockaddr *)&peer, &len);
        if(fd >= 0){
            return fd;
        }
        return -1;
    }

    static void Connect(int sock, std::string ip, uint16_t port)
    {
        struct sockaddr_in server;
        memset(&server, 0, sizeof(server));

        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr.s_addr = inet_addr(ip.c_str());

        if(connect(sock, (struct sockaddr*)&server, sizeof(server)) == 0)
        {
            cout << "Connect Success!" << endl;
        }
        else
        {
            cout << "Connect Failed!" << endl;
            exit(5);
        }
    }
};
```
**运行结果：**

![在这里插入图片描述](https://img-blog.csdnimg.cn/d3574e050eae4f06a8c8eed13455efea.png)
**http请求的/并不是根目录，而叫做web根目录，我们向服务器发送请求时，一般请求的是一个具体的资源，但如果请求的是/，就意味着请求该网站的首页，所以一般网页都要有首页资源。**






---



## HTTP响应
![在这里插入图片描述](https://img-blog.csdnimg.cn/0b6d5cce332d429bb4d0354388685ba1.png)
**HTTP响应由四部分组成：**

 1. **状态行**：http版本 + 状态码 + 状态码描述
 2. **响应报头**：HTTP响应的属性，**key : value结构，以冒号分割的键值对**，每一组属性以 '\n' 分隔，遇到空行表示请求报头结束。
 3. **空行**：**用于分隔报头和有效载荷，遇到空行表示响应报头结束。**
 4. **响应正文**：空行后面的内容都是响应正文，响应正文允许为空字符串。 如果响应正文存在，则在响应报头中会有一个**Content-Length属性来标识响应正文的长度**。

---

# HTTP的方法
| 方法|说明|支持的HTTP协议版本|
|--|--|--|
| GET|获取资源|1.0、1.1|
|POST|传输实体主体|1.0、1.1|
|PUT|传输文件|1.0、1.1|
|HEAD|获取报文首部|1.0、1.1|
|DELETE|删除文件|1.0、1.1|
|OPTIONS|询问支持的方法|1.1|
|TRACE|追踪路径|1.1|
|CONNECT|要求用隧道协议连接代理|1.1|
|LINK|建立和资源之间的联系|1.1|
|UNLINK|断开连接关系|1.1|

虽然HTTP有很多种方法，但是大部分网站是不会将所有的方法全部暴露给用户使用的，**GET和POST方法使用的频次最高。**

---

## GET 和 POST方法
**GET方法：**
![在这里插入图片描述](https://img-blog.csdnimg.cn/48c0b623e5ca4a0793ac64178f3bf57d.png)如果提交参数，GET方法是**通过URL进行提交**的，参数会显示在URL里。



**POST方法：**
![在这里插入图片描述](https://img-blog.csdnimg.cn/bd3d9896d5b9418bb126c2001bda8f08.png)
如果提交参数，POST方法是**通过正文来提交的**。
<br>

**GET和POST方法概念：**

- **GET方法**：GET方法是获取资源，是最常用的方法，默认用户要获取所有的网站，都是使用GET方法。但是同时GET方法也可以提交参数，参数通过URL拼接提交给服务端。
- **POST方法** : POST方法是推送资源，是提交参数比较常用的方法。如果提交参数，是通过正文部分提交的。服务端通过空行 + Content-Length解包获取正文信息从而获取参数。


**GET和POST方法区别：**

- **参数提交的位置不同，POST方法相对GET方法比较私密**，但是此处的私密不等于安全，GET方法不私密，提交参数会回显到URL输入框上，增加了被盗取的风险。
- GET方法是通过URL传参，**URL是有大小限制的**，这与具体的浏览器有关。POST方法是通过正文传参的，**一般大小没有限制**。


**GET和POST方法如何选择？**

- 如果提交的参数不敏感，数量较小，可以选择GET方法，否则选择POST方法。


---


# HTTP状态码
|  |类别|原因短语|
|--|--| --|
|1XX|Informational(信息类状态码)|接收的请求正在处理|
|2XX|Success(成功状态码)|请求正常处理完毕|
|3XX|Redirection(重定向状态码)|需要进行附加操作已完成请求|
|4XX|Client Error(客户端错误状态码)|服务器无法处理请求|
|5XX|Server Error(服务端错误状态码)|服务器处理请求错误|

应用层协议是众多人进行编写的，编写的人参差不齐，HTTP状态码很多人不清楚如何使用，加上浏览器种类众多，导致对状态码的支持不是很完善，比如类似404状态码，对浏览器没有指导意义，浏览器仍会照常显示网页。

**最常见的状态码，比如 200(OK)，404(Not Found)，403(Forbidden)，302(Redirect，重定向)，504(Bad Gateway)。**

---

## 3XX状态码 -- 重定向
|状态码|类别|
|--|--| 
|301|永久重定向|
|302 or 307|临时重定向|

- 重定向是必须浏览器支持301、302或307状态码才可使用，服务端必须告诉浏览器要跳转到哪个网址，**http报头属性中有Location属性**来告知浏览器要重定向的网址。
- **临时重定向**：比如当我们访问某个界面时，提示用户登录，跳转到了登录页面，登录完成后跳转到原页面，这就是临时重定向的一种场景。
- **永久重定向**：网页搬迁，域名更换。

<br>

**验证重定向：**
**Http.cc：(项目结构和其他文件与上文相同)**

```cpp
#include "Sock.hpp"
#include <pthread.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE 10240
#define WWWROOT "./wwwroot/"
#define HOME_PAGE "index.html"

void* HandlerHttpRequest(void* args)
{
    int sock = *(int*)args;
    delete (int*)args;  
    pthread_detach(pthread_self());

    char buffer[SIZE];
    memset(buffer, 0, sizeof(buffer));
    ssize_t s = recv(sock, buffer, sizeof(buffer), 0);
    if (s > 0) {
        buffer[s] = 0;
        cout << buffer;

        string http_response = "http/1.1 301 Permanently moved\n";
        http_response += "Location: https://www.csdn.net/\n";
        http_response += "\n";
        send(sock, http_response.c_str(), http_response.size(), 0);
    }

    close(sock);
    return nullptr;
}

void Usage(string proc)
{
    cout << "Usage: " << proc << " port" << endl;
    exit(1);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        Usage(argv[0]);
    }

    uint16_t port = atoi(argv[1]);
    int listen_sock = Sock::Socket();
    Sock::Bind(listen_sock, port);
    Sock::Listen(listen_sock);

    for( ; ; ) {
        int sock = Sock::Accept(listen_sock);
        if (sock > 0) {
            pthread_t pid;
            int* parm = new int(sock);
            pthread_create(&pid, nullptr, HandlerHttpRequest, parm);
        }
    }

    return 0;
}
```
**运行结果：**
![在这里插入图片描述](https://img-blog.csdnimg.cn/d9ddb32a5c2a43eaaecafd729df40c54.png)

---
# HTTP常见报头属性
- **Content-Type**: 数据类型(text/html等)
- **Content-Length**: 正文的长度
- **Host**: 客户端告知服务器, 所请求的资源是在哪个主机的哪个端口上;
- **User-Agent**: 声明用户的操作系统和浏览器版本信息;
- **Referer**: 当前页面是从哪个页面跳转过来的;
- **Location**: 搭配3xx状态码使用, 告诉客户端接下来要去哪里访问;
- **Cookie**: 用于在客户端存储少量信息. 通常用于实现会话(session)的功能
- **Connection**: 通信时采用的是长链接还是短链接
- **Referer**：请求头部字段，可以理解为保存本次请求的来源链接，或者说表示浏览器所访问的前一个页面，正是那个页面上的某个链接将浏览器带到了当前所请求的这个页面。
- **ETag**：响应头部字段，对于某个资源的某个特定版本的一个标识符，通常是一个 消息散列（比如在断点续传中用于判断续传的文件是否与上次下载的文件一致，一致则续传，不一致则重新下载）


---
## Connect -- 长链接与短链接
**Connection：keep-alive 表示长链接
Connection：close 表示短链接**

一般而言，一个网站都是由很多份资源组成的，**http/1.0采用的是短链接**，**短链接的历程是客户端发送请求，服务端响应，关闭网络套接字**。访问一个由多个资源组成的网站时，短链接就要经历多次http请求，http协议是基于tcp协议，tcp通信必须经历 建立连接->传递数据->断开连接，**每一次http请求都要经历断开连接，这样会非常耗时，影响效率。**

**http/1.1支持长链接，通过减少建立tcp连接来达到提高效率的目的。**

---

## Cookie 和 Session
当我们在访问网站时，比如访问淘宝时，我们进行各种页面跳转，淘宝仍然认识用户。
![在这里插入图片描述](https://img-blog.csdnimg.cn/dccaa672aca54e4cb1bf2a2d2820c5eb.png)

**http协议本身是一种无状态的协议，是指协议对于交互性场景没有记忆能力, 保持用户的登录状态不是http协议本身要解决的事，http主要解决网络资源获取的问题，但是http可以提供一些技术支持，保证网站具有会话保持的功能**。服务器并不知道某一个用户已经登录过了，那么是什么让服务器具有记忆呢？

HTTP本身是一个无状态的连接协议，为了支持客户端与服务器之间的交互，我们就需要通过不同的技术为交互存储状态，而这些不同的技术就是**Cookie和Session**。


---

### Cookie

- **浏览器视角**：Cookie实际上是一个文件，该文件保存着用户的私密信息。
- **http协议视角**：一旦网站拥有Cookie，在发起请求时，都会在http请求中携带Cookie信息。

![在这里插入图片描述](https://img-blog.csdnimg.cn/fd911d2321d94feb84e614e2fcbac878.png)

**cookie的基础属性：**

- domain：可以访问该Cookie的域名。如果设置为“.google.com”，则所有以“google.com”结尾的域名都可以访问该Cookie。注意第一个字符必须为“.”。

- path：Cookie的使用路径。如果设置为“/sessionWeb/”，则只有contextPath为“/sessionWeb”的程序可以访问该Cookie。如果设置为“/”，则本域名下contextPath都可以访问该Cookie。注意最后一个字符必须为“/”。

- httponly：如果cookie中设置了HttpOnly属性，那么通过js脚本将无法读取到cookie信息，这样能有效的防止XSS攻击，窃取cookie内容，这样就增加了cookie的安全性,但不是绝对防止了攻击

- secure：该Cookie是否仅被使用安全协议传输。安全协议。安全协议有HTTPS，SSL等，在网络上传输数据之前先将数据加密。默认为false。

- expires:指定了cookie的生存期，默认情况下cookie是暂时存在的，他们存储的值只在浏览器会话期间存在，当用户退出浏览器后这些值也会丢失，如果想让cookie存在一段时间，就要为expires属性设置为未来的一个过期日期。现在已经被max-age属性所取代，max-age用秒来设置cookie的生存期

**验证Cookie**：
**Http.cc：(项目结构和其他文件与上文相同)**

```cpp
#include "Sock.hpp"
#include <pthread.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE 10240
#define WWWROOT "./wwwroot/"
#define HOME_PAGE "index.html"

void* HandlerHttpRequest(void* args)
{
    int sock = *(int*)args;
    delete (int*)args;  
    pthread_detach(pthread_self());

    char buffer[SIZE];
    memset(buffer, 0, sizeof(buffer));
    ssize_t s = recv(sock, buffer, sizeof(buffer), 0);
    if (s > 0) {
        buffer[s] = 0;
        cout << buffer;
        string html_file = WWWROOT;
        html_file += HOME_PAGE;

        ifstream in(html_file);
        if (!in.is_open()) {
            string http_request = "http/1.0 404 Not Found\n";
            http_request += "Content-Type: text/html; charset: utf-8\n";
            http_request += "\n";
            http_request += "<html><p>你访问的资源不存在</p></html>";
            send(sock, http_request.c_str(), http_request.size(), 0);
        }
        else {
            struct stat st;
            stat(html_file.c_str(), &st);
            string http_request = "http/1.0 200 OK\n";
            http_request += "Content-Type: text/html; charset: utf-8\n";
            http_request += "Content-Length: ";
            http_request += to_string(st.st_size);
            http_request += "\n";
            http_request += "Set-Cookie: id=cwx\n";
            http_request += "Set-Cookie: password=123456\n";
            http_request += "\n";

            string content;
            string line;
            while (getline(in, line)) {
                content += line;
            }
            http_request += content;
            in.close();
            send(sock, http_request.c_str(), http_request.size(), 0);
        }
    }

    close(sock);
    return nullptr;
}

void Usage(string proc)
{
    cout << "Usage: " << proc << " port" << endl;
    exit(1);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        Usage(argv[0]);
    }

    uint16_t port = atoi(argv[1]);
    int listen_sock = Sock::Socket();
    Sock::Bind(listen_sock, port);
    Sock::Listen(listen_sock);

    for( ; ; ) {
        int sock = Sock::Accept(listen_sock);
        if (sock > 0) {
            pthread_t pid;
            int* parm = new int(sock);
            pthread_create(&pid, nullptr, HandlerHttpRequest, parm);
        }
    }

    return 0;
}
```

**运行结果：**
![在这里插入图片描述](https://img-blog.csdnimg.cn/cd037110ae30422e8b6b59d0aee7de44.png)

**在网络通信的过程中，如果有人盗取了Cookie文件，别人就可以使用用户的身份访问特定的资源，如果Cookie文件保存的是用户名密码，就会造成严重的安全问题。所以说单独使用Cookie是具有一定安全隐患的，Cookie更多的使用场景是搭配着Session一起使用。**

---

###  Session
Session是另一种记录客户状态的机制，不同的是Cookie保存在客户端浏览器中，而Session保存在服务器上。
**当浏览器第一次访问服务器时，服务器创建一个session对象(该对象有一个唯一的id,一般称之为sessionId),服务器会将sessionId以cookie的方式发送给浏览器。当浏览器再次访问服务器时，会将sessionId发送过来，服务器依据sessionId就可以找到对应的session对象。**
**核心思路就是把用户的私密信息保存到服务器上。**

![在这里插入图片描述](https://img-blog.csdnimg.cn/13127ceb059b418bb8f0005b218d65ac.png)
Session可以存放各种类别的数据，相比只能存储字符串的cookie，能给开发人员存储数据提供很大的便利。

---

