#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <cstring>
#include <unistd.h>

void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " server_ip " << "server_port" << std::endl;
}

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        Usage(argv[0]);
        return 1;
    }
    std::string svr_ip = argv[1];
    uint16_t svr_port = (uint16_t)atoi(argv[2]);

    // 1.创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        std::cout << "create socket failed!" << std::endl;
        return 2;
    }

    struct sockaddr_in server;
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(svr_port);
    server.sin_addr.s_addr = inet_addr(svr_ip.c_str());

    // 2.发起链接
    if(connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        std::cout << "connect fail!" << std::endl;
        return 3;
    }

    std::cout << "connect success!" << std::endl;

    std::cout << "Please Enter#" << std::endl;
    while(true)
    {
        char buffer[1024] = {0};
        fgets(buffer, sizeof(buffer), stdin);

        write(sock, buffer, sizeof(buffer)-1);
        ssize_t s = read(sock, buffer, sizeof(buffer));
        if(s > 0)
        {
            buffer[s] = 0;
            std::cout << buffer;
        }
    }

    return 0;
}