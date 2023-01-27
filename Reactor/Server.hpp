#pragma once

#include <iostream>
#include <cerrno>
#include <vector>
#include "Reactor.hpp"
#include "Sock.hpp"
#include "Util.hpp"

#define RECV_SIZE 128

//  1: 读取成功
//  0: 对端关闭
// -1: 读取错误
int RecverCore(int sock, std::string& inbuffer)
{
    while (true) {
        char buffer[RECV_SIZE];
        ssize_t s = recv(sock, buffer, RECV_SIZE - 1, 0);
        if (s > 0) {
            buffer[s] = '\0';
            inbuffer += buffer;
        }
        else if (s < 0) {
            if (errno == EINTR) {
                // 被信号中断
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 已经完全读完
                return 1;
            }
            // 读取出错
            return -1;
        }
        else {
            // 对端关闭
            return 0;
        }
    }
}

int recver(Event* e)
{
    std::cout << "Recverer have been call" << std::endl;
    // 1. 读取数据
    int result = RecverCore(e->sock, e->inbuffer);
    if (result <= 0) {
        // 差错处理
        if (e->errorer)
            e->errorer(e);
        return -1;
    }

    // 2. 分包 -- 解决粘包问题(标识符X分隔)
    std::vector<std::string> tokens;
    std::string sep = "X";
    SplitSegment(e->inbuffer, &tokens, sep);

    // 3. 反序列化
    for (auto& seg : tokens) {
        std::string data1, data2;
        if (Deserialize(seg, &data1, &data2)) {
            // 4. 业务处理
            int x = atoi(data1.c_str());
            int y = atoi(data2.c_str());
            int z = x + y;
            std::string res = to_string(z);
            // 5. 构建响应
            std::string response = data1;
            response += "+";
            response += data2;
            response += "=";
            response += res;
            response += "X";

            e->outbuffer += response;
        }
    }   
    // 6. 发送报文 -- 按需设置写事件就绪
    if (!e->outbuffer.empty()) {
        e->R->EnableRW(e->sock, true, true);
    }
    return 0;
}

int SendCore(int sock, std::string& outbuffer)
{
    while (true) {
        int total = 0;    // 本次循环发送的数据量
        const char* start = outbuffer.c_str();
        int size = outbuffer.size();
        ssize_t cur = send(sock, start + total, size - total, 0);
        if (cur > 0) {
            total += cur;
            if (total == size) {
                outbuffer.clear();
                return 1;
            }
        }
        else {
            if (errno == EINTR) {
                continue;
            }
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 发送接收，但还未发送完
                return 0;
            }

            return -1;
        }
    }
}

int sender(Event* e)
{
    std::cout << "Sender have been call" << std::endl;
    int result = SendCore(e->sock, e->outbuffer);
    if (result == 1) {
        e->R->EnableRW(e->sock, true, false);
    }
    else if (result == 0) {
        e->R->EnableRW(e->sock, true, true);
    }
    else {
        if (e->errorer)
            e->errorer(e);
    }

}

int errorer(Event* e)
{
    std::cout << "Errorer have been call" << std::endl;
    e->R->DeleteEvent(e);
}