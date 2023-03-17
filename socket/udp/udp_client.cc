#include <iostream>
#include <cerrno>
#include <cstring>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void Usage(std::string proc)
{
    std::cout << "Usage: \n\t" << proc << " server_ip server_port" << std::endl;
}


int main(int argc, char* argv[])
{
    if (argc != 3) {
        Usage(argv[0]);
        return 1;
    }


    // 1. 创建套接字，打开网络文件
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        std::cerr << "create socket failed errno: " << errno << std::endl;
        return 2;
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_port = htons(atoi(argv[2]));
    socklen_t len = sizeof(server);

    // 2. 使用服务
    while (true) 
    {
        char line[1024];
        std::cout << "Please Enter# ";
        fgets(line, sizeof(line), stdin);

        sendto(sock, line, strlen(line), 0, (struct sockaddr*)&server, len);

        struct sockaddr_in tmp;
        socklen_t len = sizeof(tmp);
        char buffer[1024];
        ssize_t s = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&tmp, &len);
        if (s > 0)
        {
            buffer[s] = 0;
            std::cout << "server# " << buffer << std::endl;
        }
        else
        {
            std::cerr << "recvfrom failed" << std::endl;
        }
    }

    return 0;
}