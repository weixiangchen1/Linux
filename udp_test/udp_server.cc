#include <iostream>
#include <string>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const uint16_t port = 8080;

int main()
{
    // 1. 创建套接字，打开网络文件
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        std::cerr << "socket create error:" << errno << std::endl;
        return 1;
    }

    // 2. 服务器绑定端口和ip
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    // 将计算机变量的主机序列转为网络序列
    local.sin_port = htons(port);
    // 服务器可能有多张网卡配置多个ip，需要获取所有发送到该主机所有ip的数据
    local.sin_addr.s_addr = INADDR_ANY;

    if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
    {
        std::cerr << "bind error:" << errno << std::endl;
        return 2;
    }

    // 3. 提供服务
    char buffer[1024];
    while(true)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        // 接受客户端的数据，输出型参数获取对端端口号和ip
        recvfrom(sock, buffer, sizeof(buffer)-1, 0, (struct sockaddr*)&peer, &len);

        std::cout << "client# " << buffer << std::endl;

        // 响应客户端
        std::string msg = "server receive message already";
        sendto(sock, msg.c_str(), msg.size(), 0, (struct sockaddr*)&peer, len);
    }

    return 0;
}