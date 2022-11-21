#pragma once

#include <iostream>
#include <string>
#include <jsoncpp/json/json.h>

using namespace std;

typedef struct request
{
    int x; // 操作数1
    int y; // 操作数2
    char op; // 操作符
}request_t;

typedef struct response
{
    int code; // 计算完毕的状态 0：success
    int result; // 运算结果
}response_t;

string SerializeRequest(const request_t& req)
{
    Json::Value root;         
    root["datax"] = req.x;      
    root["datay"] = req.y;      
    root["operator"] = req.op;

    Json::FastWriter writer;
    string json_string = writer.write(root);      
    return json_string;
}

string SerializeResponse(const response_t& resp)
{
    Json::Value root;         
    root["code"] = resp.code;      
    root["result"] = resp.result;      

    Json::FastWriter writer;    
    string json_string = writer.write(root);    
      
    return json_string;
}

void DeserializeRequest(const string& josn_string, request_t& req)
{
    Json::Value root;
    Json::Reader reader;

    reader.parse(josn_string, root);
    req.x = root["datax"].asInt();
    req.y = root["datay"].asInt();
    req.op = (char)root["operator"].asInt();
}

void DeserializeResponse(const string& josn_string, response_t& resp)
{
    Json::Value root;
    Json::Reader reader;

    reader.parse(josn_string, root);
    resp.code = root["code"].asInt();
    resp.result = root["result"].asInt();
}