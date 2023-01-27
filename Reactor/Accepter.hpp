#pragma once

#include "Sock.hpp"
#include "Reactor.hpp"
#include "Util.hpp"
#include "Server.hpp"

int Accepter(Event* e)
{
    std::cout << "new link is coming: " << e->sock << std::endl;
    int sock = Sock::Accept(e->sock);
    if (sock < 0) {
        std::cout << "Accept Done" << std::endl;
    }
    std::cout << "Accept success: " << sock << std::endl;
    // ET模式，设置文件描述符为非阻塞
    SetNonBlock(sock);
    // 为新链接创建事件Event
    Event* newEvent = new Event();
    newEvent->sock = sock;
    newEvent->R = e->R;
    newEvent->RegisterCallback(recver, sender, errorer);
    // 将新事件添加到Reactor
    e->R->InsertReactor(newEvent, EPOLLIN | EPOLLET);

}