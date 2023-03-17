#include <iostream>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

void Usage(std::string proc)
{
    std::cout << "Usage: \n\t" << proc << " server_port" << std::endl;
}

void ServerIO(int new_fd)
{
    // 提供服务
    while (true)
    {
        char buffer[1024] = {0};
        ssize_t s = read(new_fd, buffer, sizeof(buffer));
        if (s > 0)
        {
            buffer[s] = 0;
            std::string echo_string = "server recevie message: ";
            echo_string += buffer;

            std::cout << echo_string << std::endl;

            write(new_fd, echo_string.c_str(), echo_string.size());
        }
        else if (s == 0)
        {
            // 对端关闭连接
            std::cout << "client close..." << std::endl;
            break;
        }
        else
        {
            // 读取失败
            std::cerr << "read failed" << std::endl;
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        return 1;
    }

    // signal忽略SIGCHLD，表示子进程会自主退出释放资源，无需父进程管理
    signal(SIGCHLD, SIG_IGN);

    // 1. 创建套接字，打开网络文件
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0)
    {
        std::cerr << "socket create failed errno: " << errno << std::endl;
        return 2;
    }

    // 2. 绑定IP地址和PORT端口号
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(atoi(argv[1]));

    if (bind(listen_sock, (struct sockaddr *)&local, sizeof(local)) < 0)
    {
        std::cerr << "bind failed" << std::endl;
        return 3;
    }

    // 3. 设置套接字为Listen状态
    if (listen(listen_sock, 5) < 0)
    {
        std::cerr << "listen failed" << std::endl;
        return 4;
    }

    // 4. 获取新链接并提供服务
    while (true)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int new_fd = accept(listen_sock, (struct sockaddr *)&peer, &len);
        if (new_fd < 0)
        {
            continue;
        }

        std::cout << "get a new link, fd: " << new_fd << std::endl;

        pid_t pid = fork();
        if (pid < 0)
        {
            std::cout << "fork create child process failed" << std::endl;
            return 5;
        }
        else if (pid == 0)
        {
            // 子进程
            ServerIO(new_fd);
            exit(0);
        }
        else
        {
            // 父进程
            // do nothing
        }
    }

    return 0;
}