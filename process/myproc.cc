#include <iostream>
#include <cstdlib>
#include <unistd.h>
using namespace std;

// 验证孤儿进程
int main()
{
    pid_t id = fork();
    if(id == 0){
        // child
      while(true){
          cout << "child is running!" << endl;
          sleep(2);
      }
    }
    else{
        //parent
      cout << "parent is running!" << endl;
      sleep(10);// 10s后父进程终止
      exit(1);
    }


    return 0;
}

// 验证僵尸进程
//int main()
//{
//    pid_t id = fork();
//    if(id == 0){
//        //child
//       while(true){
//         cout << "child is running!" << endl;
//         sleep(2);
//       }
//    }
//    else{
//        //parent
//        cout << "parent do nothing!" << endl;
//        sleep(50);
//    }
//
//    return 0;
//}
