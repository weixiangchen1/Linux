#include "Sock.hpp"
#include <pthread.h>

#define SIZE 10240

void* HandlerHttpRequest(void* args)
{
    int sock = *(int*)args;
    delete (int*)args;  
    pthread_detach(pthread_self());

    char buffer[SIZE];
    memset(buffer, 0, sizeof(buffer));
    ssize_t s = recv(sock, buffer, sizeof(buffer), 0);
    if (s > 0) {
        buffer[s] = 0;
        cout << buffer;

        string http_response = "http/1.0 200 OK\n";
        http_response += "Content-Type: text/plain\n";
        http_response += "\n";
        http_response += "hello world!";
        cout << http_response << endl;

        send(sock, http_response.c_str(), http_response.size(), 0);
    }

    close(sock);
    return nullptr;
}

void Usage(string proc)
{
    cout << "Usage: " << proc << " port" << endl;
    exit(1);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        Usage(argv[0]);
    }

    uint16_t port = atoi(argv[1]);
    int listen_sock = Sock::Socket();
    Sock::Bind(listen_sock, port);
    Sock::Listen(listen_sock);

    for( ; ; ) {
        int sock = Sock::Accept(listen_sock);
        if (sock > 0) {
            pthread_t pid;
            int* parm = new int(sock);
            pthread_create(&pid, nullptr, HandlerHttpRequest, parm);
        }
    }

    return 0;
}