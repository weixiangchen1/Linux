#include <iostream>
#include <cerrno>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

const uint16_t port = 8080;

int main()
{
    // 1. 创建套接字，打开网络文件
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "create socket failed errno: " << errno << std::endl;
        return 1;
    }

    // 2. 绑定端口号
    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_port = htons(port);
    local.sin_addr.s_addr = INADDR_ANY;
    socklen_t len = sizeof(local);
    if (bind(sock, (struct sockaddr*)&local, len) < 0)
    {
        std::cerr << "bind failed erron: " << errno << std::endl;
        return 2;
    }

    // 3. 提供服务
    bool quit = false;
    while (!quit) 
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        char buffer[1024];
        ssize_t s = recvfrom(sock, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&peer, &len);
        if (s > 0) 
        {
            buffer[s] = 0;
            std::cout << "client# " << buffer << std::endl;
        }
        else
        {
            std::cerr << "recvfrom failed" << std::endl;
        }

        const std::string echo_string = "hello client";
        sendto(sock, echo_string.c_str(), echo_string.size(), 0, (struct sockaddr*)&peer, len);
    }

    return 0;
}