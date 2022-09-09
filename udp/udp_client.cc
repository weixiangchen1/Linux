#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

void Usage(std::string proc)
{
    std::cout << "Usage:\n\t" << proc << " server_ip server_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        return 0;
    }


    // 1.创建套接字，打开网络文件
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        std::cerr << "socket error:" << errno << std::endl;
        return 1;
    }

    // 客户端无需显式bind端口号和ip，OS自动bind，采用随机端口的方式
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    server.sin_addr.s_addr = inet_addr(argv[1]);

    // 2.使用服务
    while(true)
    {
        std::cout << "Shell $ ";
        char line[1024];
        fgets(line, sizeof(line), stdin);

        sendto(sock, line, strlen(line), 0, (struct sockaddr*)&server, sizeof(server));
        
        struct sockaddr_in tmp;
        socklen_t len = sizeof(tmp);
        char buffer[1024];
        ssize_t s = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&tmp, &len);
        if(s > 0)
        {       
            buffer[s] = 0; 
            std::cout << buffer << std::endl;
        }
        else
        {
            // TODO
        }
    }

    return 0;
}