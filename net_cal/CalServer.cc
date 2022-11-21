#include "Sock.hpp"
#include "Protocol.hpp"
#include <pthread.h>

static void Usage(string proc)
{
    cout << "Usage: " << proc << " port" << endl;
    exit(1);
}

void* HandlerRequest(void* args)
{
    int sock = *(int*)args;
    delete (int*)args;
    pthread_detach(pthread_self());

    request_t req;
    char buffer[1024];
    ssize_t s = read(sock, buffer, sizeof(buffer) - 1);
    // ssize_t s = read(sock, &req, sizeof(req));
    if (s > 0) {
        buffer[s] = 0;
        response_t resp;
        string str = buffer;
        DeserializeRequest(str, req);
        switch (req.op)
        {
        case '+':
            resp.result = req.x + req.y;
            break;
        case '-':
            resp.result = req.x - req.y;
            break;
        case '*':
            resp.result = req.x * req.y;
            break;
        case '/':
            if (req.y == 0) 
            {
                resp.code = -1;
            }
            else
            {
                resp.result = req.x / req.y;
                resp.code = 0;
            }
            break;
        case '%':
            if (req.y == 0) 
            {
                resp.code = -2;
            }
            else
            {
                resp.result = req.x % req.y;
                resp.code = 0;
            }
            break;
        
        default:
            resp.code = -3;
            break;
        }
        cout << "request: " << req.x << req.op << req.y << endl;
        string json_string = SerializeResponse(resp);
        write(sock, json_string.c_str(), json_string.size());
        cout << "server over!" << endl;
    }


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
            cout << "get a new connect" << endl;
            int* pram = new int(sock);
            pthread_t pid;
            pthread_create(&pid, nullptr, HandlerRequest, (void*)pram);
        }
    }


    return 0;
}