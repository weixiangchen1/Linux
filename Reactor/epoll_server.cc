#include "Reactor.hpp"
#include "Sock.hpp"
#include "Accepter.hpp"

void Usage(std::string proc)
{
    std::cout << "Usage: " << proc << " port" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        Usage(argv[0]);
        exit(1);
    }
    // 1. 创建套接字，绑定监听
    int listen_sock = Sock::Socket();
    Sock::Bind(listen_sock, (uint16_t)atoi(argv[1]));
    Sock::Listen(listen_sock);
    
    // 2. 创建Reactor反应堆
    Reactor* R = new Reactor();
    R->InitReactor();

    // 3. 创建Event事件，添加到反应堆
    Event* e = new Event();
    e->sock = listen_sock;
    e->R = R;
    // 3.1 注册链接管理器
    e->RegisterCallback(Accepter, nullptr, nullptr);
    // 3.2 插入反应堆，开始链接派发
    R->InsertReactor(e, EPOLLIN | EPOLLET);
    
    for ( ; ; ) {
        R->Dispatcher(1000);
    }

    return 0;
}