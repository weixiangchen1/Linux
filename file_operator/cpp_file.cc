#include <iostream>
#include <string>
#include <fstream>
using namespace std;


int main()
{
    ofstream out("./log.txt", ios::out | ios::binary);
    if(!out.is_open()){
        cerr << "open erron" << endl;
        return 1;
    }

    int cnt = 5;
    while(cnt--){
        string msg = "hello\n";
        out.write(msg.c_str(), msg.size());
    }

    out.close();
      
    return 0;
}
