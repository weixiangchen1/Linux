#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <unistd.h>
#include <cstdlib>
#include <sys/epoll.h>

#define SIZE 256
#define NUM 128

class Event;
class Reactor;

typedef int (*callback_t)(Event* ev);

// epoll多路转接管理的结构体Event
class Event
{
public:
    int sock;                   // 文件描述符
    std::string inbuffer;       // 文件描述符对应的输入缓冲区
    std::string outbuffer;      // 文件描述符对应的输出缓冲区

    callback_t recver;          // 读回调函数
    callback_t sender;          // 写回调函数
    callback_t errorer;         // 错误回调函数

    Reactor* R;                 // 事件对应的反应堆
public:
    Event()
        :sock(-1)
        , recver(nullptr)
        , sender(nullptr)
        , errorer(nullptr)
    {}

    // 注册回调函数
    void RegisterCallback(callback_t _recver, callback_t _sender, callback_t _errorer)
    {
        recver = _recver;
        sender = _sender;
        errorer = _errorer;
    }

    ~Event() 
    {}
};

class Reactor
{
public:
    int epfd;
    std::unordered_map<int, Event*> events;  // 文件描述符和事件结构体的映射关系

public:
    Reactor()
        :epfd(-1)
    {}

    // 创建epoll模型
    void InitReactor()
    {
        epfd = epoll_create(SIZE);
        if (epfd < 0) {
            std::cout << "create epoll failed" << std::endl;
            exit(2);
        }
        std::cout << "create epoll success" << std::endl;
    }

    // 注册epoll事件监听
    bool InsertReactor(Event* e, uint32_t evs)
    {
        // 1. 添加事件到epoll模型
        struct epoll_event ev;
        ev.events = evs;
        ev.data.fd = e->sock;
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, e->sock, &ev) < 0) {
            std::cout << "InsertReactor failed" << std::endl;
            return false;
        }

        // 2. 添加事件到events映射关系
        events.insert({e->sock, e});
    }

    // 使能读写
    bool EnableRW(int sock, bool enable_read, bool enable_write)
    {
        struct epoll_event ev;
        ev.data.fd = sock;
        ev.events = EPOLLET | (enable_read ? EPOLLIN : 0) | (enable_write ? EPOLLOUT : 0);
        if (epoll_ctl(epfd, EPOLL_CTL_MOD, sock, &ev) < 0) {
            return false;
        }
        return true;
    }

    void DeleteEvent(Event* e)
    {
        int sock = e->sock;
        auto it = events.find(sock);
        if (it != events.end()) {
            // 1. 从epoll模型中删除
            epoll_ctl(epfd, EPOLL_CTL_DEL, sock, nullptr);
            // 2. 从unordered_map中删除
            events.erase(it);
            // 3. 关闭文件描述符
            close(sock);
            // 4. delete event结点
            delete e;
        }
    }

    bool IsSockReady(int sock) 
    {
        auto it = events.find(sock);
        return it != events.end();
    }

    // 就绪事件的派发器
    void Dispatcher(int timeout)
    {
        struct epoll_event revs[NUM];
        int n = epoll_wait(epfd, revs, NUM, timeout);
        for (int i = 0; i < n; ++i) {
            int sock = revs[i].data.fd;
            uint32_t event = revs[i].events;

            // 链接错误和链接挂断的差错处理转化为IO解决
            if (event & EPOLLERR)
                event |= (EPOLLIN | EPOLLOUT);
            if (event & EPOLLHUP)
                event |= (EPOLLIN | EPOLLOUT);

            // 读写事件就绪：通过映射关系找到对应回调函数调用
            if (event & EPOLLIN) {
                if (events[sock]->recver)
                    events[sock]->recver(events[sock]);
            }
            if (event & EPOLLOUT) {
                if (events[sock]->sender)
                    events[sock]->sender(events[sock]);
            }
        }
    }
};