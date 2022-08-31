#include <iostream>
#include <queue>
using namespace std;

// 懒汉方式实现单例模式
template<typename T>
class TaskQueue
{
    static TaskQueue<T>* inst;
private:
    int _size;
    queue<T> _tq;

private:
    TaskQueue(int size = 10)
    :_size(size)
    {}    

    TaskQueue(const TaskQueue<T> &tq) = delete;
    TaskQueue<T>& operator=(TaskQueue<T>& tq) = delete;

public:
    static TaskQueue<T>* Getinstance()
    {
        if(inst == nullptr)
        {
            inst = new TaskQueue<T>();
        }
    }

    void push(const T data)
    {
        _tq.push(data);
    }

    void pop(T* data)
    {
        *data = _tq.front();
        _tq.pop();
    }

    bool empty()
    {
        return _tq.empty();
    }
};

template<typename T>
TaskQueue<T>* TaskQueue<T>::inst = nullptr;


int main()
{
    TaskQueue<int>* inst = TaskQueue<int>::Getinstance();

    return 0;
}


// 饿汉方式实现单例模式
// template<typename T>
// class TaskQueue
// {
//     static TaskQueue<T> inst;
// private:
//     int _size;
//     queue<T> _tq;

// private:
//     TaskQueue(int size = 10)
//     :_size(size)
//     {}    

//     TaskQueue(const TaskQueue<T> &tq) = delete;
//     TaskQueue<T>& operator=(TaskQueue<T>& tq) = delete;

// public:
//     static TaskQueue<T>* Getinstance()
//     {
//         return &inst;
//     }

//     void push(const T data)
//     {
//         _tq.push(data);
//     }

//     void pop(T* data)
//     {
//         *data = _tq.front();
//         _tq.pop();
//     }

//     bool empty()
//     {
//         return _tq.empty();
//     }
// };

// template<typename T>
// TaskQueue<T> TaskQueue<T>::inst;


// int main()
// {
//     TaskQueue<int>* inst = TaskQueue<int>::Getinstance();

//     return 0;
// }