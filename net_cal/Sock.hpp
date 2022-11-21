#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <sys/types.h>
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
        if (sock < 0) {
            cout << "create socket failed!" << endl;
            exit(2);
        }
        return sock;
    }

    static void Bind(int sock, uint16_t port)
    {
        struct sockaddr_in local;
        local.sin_family = AF_INET;
        local.sin_port = htons(port);
        local.sin_addr.s_addr = INADDR_ANY;

        if (bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0) {
            cerr << "bind error!" << endl;
            exit(3);
        }
    }

    static void Listen(int sock) 
    {
        if (listen(sock, 5) < 0) {
            cerr << "listen error!" << endl;
            exit(4);
        }
    }

    static int Accept(int sock)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        int fd = accept(sock, (struct sockaddr*)&peer, &len);
        if (fd >= 0) {
            return fd;
        }
        return -1;
    }

    static void Connect(int sock, string ip, uint16_t port)
    {
        struct sockaddr_in server;
        server.sin_family = AF_INET;
        server.sin_port = htons(port);
        server.sin_addr.s_addr = inet_addr(ip.c_str());

        if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == 0)
        {
            cout << "connect success!" << endl;
        }
        else{
            cout << "connect failed!" << endl;
            exit(5);
        }
    }
};