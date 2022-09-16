#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <cerrno>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>

void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << "port" << std::endl;
}

void ServiceIO(int new_socket)
{
    while(true)
    {
        char buffer[1024] = {0};
        memset(buffer, 0, sizeof(buffer));
        ssize_t s = read(new_socket, buffer, sizeof(buffer)-1);
        if(s > 0)
        {
            buffer[s] = 0;
            std::cout << "client# " << buffer << std::endl;
            std::string echo_string = "> server reveice massage already!";
            write(new_socket, echo_string.c_str(), echo_string.size());
        }
        else if(s == 0)
        {
            std::cout << "> client quit..." << std::endl;
            break;
        }
        else
        {
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        Usage(argv[0]);
        return 1;
    }

    // 1.创建套接字
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_sock < 0)
    {
        std::cerr << "create socket error: " << errno << std::endl;
        return 2;
    }

    // 2.绑定端口号和IP地址
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_port = htons(atoi(argv[1]));
    local.sin_addr.s_addr = INADDR_ANY;

    if(bind(listen_sock, (struct sockaddr*)&local, sizeof(local)) < 0)
    {
        std::cerr << "bind error: " << errno << std::endl;
        return 3;
    }

    // tcp通信是面向连接的，server端需要不断监听client端连接的请求
    const int back_log = 5;
    if(listen(listen_sock, back_log) < 0)
    {
        std::cerr << "listen error: " << errno << std::endl;
        return 4;
    }

    signal(SIGCHLD, SIG_IGN);

    for( ; ; )
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int new_socket = accept(listen_sock, (struct sockaddr*)&local, &len);
        if(new_socket < 0)
        {
            continue;
        }
        u_int16_t cli_port = ntohs(peer.sin_port);
        std::string cli_ip = inet_ntoa(peer.sin_addr);

        std::cout << "get a new link: [" << cli_ip << ":" << cli_port << "]# fd: " << new_socket << std::endl;   

        pid_t id = fork();
        if(id < 0)
        {
            continue;
        }
        else if(id == 0)
        {
            // 子进程
            close(listen_sock);
            ServiceIO(new_socket);
            close(new_socket);
            exit(0);
        }
        else
        {
            // 父进程
            close(new_socket);
        }

    }

    return 0;
}