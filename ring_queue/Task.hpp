#pragma once
#include <iostream>
#include <string>
#include <pthread.h>

namespace ns_task
{
    class Task
    {
    private:
        int _x;
        int _y;
        char _op;
    public:
        Task() {}
        Task(int x, int y, char op):_x(x), _y(y), _op(op) {}
        std::string message()
        {
            std::string msg = std::to_string(_x);
            msg += _op;
            msg += std::to_string(_y);
            msg += "=?";

            return msg;
        }
        int Run()
        {
            int res = 0;
            switch(_op)
            {
            case '+':
                res = _x + _y;
                break;
            case '-':
                res = _x - _y;
                break;
            case '*':
                res = _x * _y;
                break;
            case '/':
                res = _x / _y;
                break;
            case '%':
                res = _x % _y;
                break;
            default:
                break;
            }
            std::cout << "Consumer Thread: " << pthread_self() << " Task: " << _x << _op << _y << "=" << res << std::endl;
            return res;
         }
        
        int operator()()
        {
            return Run();
        }

        ~Task(){}
    };
}