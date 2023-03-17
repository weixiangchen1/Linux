#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

void Usage(std::string proc)
{
    std::cout << "Usage: \n\t" << proc << "server_ip server_port" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc != 3) 
    {
        Usage(argv[0]);
        return 1;
    }

    std::string server_ip = argv[1];
    uint16_t server_port = atoi(argv[2]);

    // 1. 创建套接字，打开网络文件
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "sock create failed errno: " << errno << std::endl;
        return 2;
    }

    // 2. 建立链接
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());
    server.sin_port = htons(server_port);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server))) 
    {
        std::cerr << "connect failed" << std::endl;
        return 3;
    }

    std::cout << "connect success" << std::endl;

    // 3. 使用服务
    while (true) 
    {
        char line[1024];
        std::cout << "Please Enter# ";
        fgets(line, sizeof(line), stdin);

        write(sock, line, sizeof(line) - 1);

        char buffer[1024];
        ssize_t s = read(sock, buffer, sizeof(buffer) - 1);
        if (s > 0) 
        {
            buffer[s] = 0;
            std::cout << buffer << std::endl;
        }
        else if (s == 0)
        {
            std::cout << "server close..." << std::endl;
            break;
        }
        else
        {
            std::cerr << "read failed" << std::endl;
            break;
        }
    }

    return 0;
}