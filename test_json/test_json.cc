#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

using namespace std;

typedef struct Calculate
{
    int x;
    int y;
    char op;
}Calculate;


int main()
{
    // 序列化过程
    // Calculate cal{10, 20, '+'};
    // Json::Value root;
    // root["datax"] = cal.x;
    // root["datay"] = cal.y;
    // root["operator"] = cal.op;

    // Json::FastWriter writer;
    // string json_string = writer.write(root);
    // cout << json_string << endl;    

    // 反序列化过程
    Calculate cal;
    string json_string = R"({"datax":10,"datay":20,"operator":43})";
    Json::Value root;
    Json::Reader reader;
    reader.parse(json_string, root);
    cal.x = root["datax"].asInt();
    cal.y = root["datay"].asInt();
    cal.op = (char)root["operator"].asInt();
    cout << "x: " << cal.x << " y: " << cal.y << " op: " << cal.op << endl;

    return 0;
}
