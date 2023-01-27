#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sys/fcntl.h>

void SetNonBlock(int fd)
{
    int fl = fcntl(fd, F_GETFL);
    if (fl < 0) {
        perror("fcntl");
        return;
    }
    fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}

void SplitSegment(std::string& inbuffer, std::vector<std::string>* tokens, std::string& sep)
{
    while (true) {
        int pos = inbuffer.find(sep);
        if (pos == std::string::npos) {
            return;
        }
        std::string sub = inbuffer.substr(0, pos);
        tokens->push_back(sub);
        inbuffer.erase(0, pos + sep.size());
    }
}

bool Deserialize(std::string& seg, std::string* out1, std::string* out2)
{
    std::string op = "+";
    int pos = seg.find(op);
    if (pos == std::string::npos) 
        return false;
    *out1 = seg.substr(0, pos);
    *out2 = seg.substr(pos + op.size());
    return true;
}