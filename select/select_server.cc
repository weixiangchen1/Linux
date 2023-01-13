#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/select.h>
#include "Sock.hpp"

#define NUM (sizeof(fd_set)*8)

int fd_array[NUM];   // 存放文件描述符的数组

void Usage(std::string proc) {
    std::cout << "Usage: " << proc << " port" << std::endl;
    exit(1);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        Usage(argv[0]);
    }

    uint16_t port = (uint16_t)atoi(argv[1]);
    int listen_sock = Sock::Socket();
    Sock::Bind(listen_sock, port);
    Sock::Listen(listen_sock);

    // 初始化fd_array[] 
    for (int i = 0; i < NUM; ++i) {
        fd_array[i] = -1;
    }

    // 时间循环
    fd_set rfds;
    fd_array[0] = listen_sock;
    for ( ; ; ) {
        FD_ZERO(&rfds);
        int max_fd = fd_array[0];  // 最大的文件描述符
        for (int i = 0; i < NUM; ++i) {
            if (fd_array[i] == -1) 
                continue;
            FD_SET(fd_array[i], &rfds);  // 将文件描述符设置进位图中

            if (max_fd < fd_array[i]) {
                max_fd = fd_array[i];    // 更新最大文件描述符
            }
        }

        struct timeval timeout {2, 0};
        int n = select(max_fd + 1, &rfds, nullptr, nullptr, &timeout);
        switch (n)
        {
        case -1:
            std::cerr << "select error..." << std::endl;
            break;
        case 0:
            std::cout << "timeout..." << std::endl;
            break;
        default:
            std::cout << "There are events ready..." << std::endl;
            for (int i = 0; i < NUM; ++i) {
                if (fd_array[i] == -1)
                    continue;

                // 遍历寻找就绪的文件描述符fd
                if (FD_ISSET(fd_array[i], &rfds)) {
                    std::cout << "fd: " << fd_array[i] << " read events is ready..." << std::endl;

                    // fd = listen_sock时，需要accept获取新链接
                    if (fd_array[i] == listen_sock) {
                        std::cout << "listen_sock: " << listen_sock << " have a new link..." << std::endl;
                        int sock = Sock::Accept(listen_sock);
                        std::cout << "listen_sock: " << listen_sock << " get a new link success..." << std::endl;
                        
                        // 获取新链接后，需要将新链接的文件描述符添加到fd_array，以便之后设置进select
                        int pos;
                        for (pos = 1; pos < NUM; ++pos) {
                            if (fd_array[pos] == -1)
                                break; 
                        }
                        
                        if (pos < NUM) {
                            fd_array[pos] = sock;
                            std::cout << "new link " << sock << " is add fd_array[] already..." << std::endl;
                        }
                        else {
                            std::cout << "The server is fully loaded..." << std::endl;
                            close(sock);
                        }
                    }
                    // fd != listen_sock时，普通文件描述符正常执行读操作
                    else {
                        std::cout << "sock: " << fd_array[i] << " perform a read operation..." << std::endl;
                        char recv_buffer[1024] = {0};
                        ssize_t s = recv(fd_array[i], recv_buffer, sizeof(recv_buffer) - 1, 0);
                        if (s > 0) {
                            recv_buffer[s] = '\0';
                            std::cout << "client[" << fd_array[i] << "]# " << recv_buffer << std::endl;
                        }
                        else if (s == 0) {
                            std::cout << "sock: " << fd_array[i] << "is close" << std::endl;
                            // 对端关闭了链接
                            close(fd_array[i]);
                            fd_array[i] == -1;
                        }
                        else {
                            std::cout << "sock: " << fd_array[i] << "is close" << std::endl;
                            // 对端关闭了链接
                            close(fd_array[i]);
                            fd_array[i] == -1;                            
                        }
                    }
                }
            }
            break;
        }

    }


    return 0;
}