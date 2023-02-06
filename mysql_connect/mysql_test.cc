#include <iostream>
#include <cstdio>
#include <string>
#include <mysql.h>

const std::string host = "127.0.0.1";
const std::string user = "cwx";
const std::string passwd = "AsDfGhJk123~!@";
const std::string db = "CRUD";
const unsigned port = 3306;

int main()
{
    // std::cout << "client version: " << mysql_get_client_info() << std::endl;
    
    // 1、初始化
    MYSQL* my = mysql_init(nullptr);
    // 2、链接数据库
    if (mysql_real_connect(my, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, nullptr, 0) == nullptr)
    {
        std::cout << "connect failed" << std::endl;
        return 1;
    }
    std::cout << "connect success" << std::endl;
    // 3、访问数据库
    // std::string sql = "insert into students values (102, 1004, \'Kobe\', \'12341.com\')";
    std::string sql = "select id,sn,name,email from students";
    int res = mysql_query(my, sql.c_str());
    if (res != 0) 
    {
        std::cout << "execute: " << sql << " failed" << std::endl;
        return 2;
    }
    std::cout << "execute: " << sql << " success" << std::endl;
    // 3.1 解析数据 -- 获取行数和列数
    MYSQL_RES* result =  mysql_store_result(my);
    int rows = mysql_num_rows(result);
    int cols = mysql_num_fields(result);
    std::cout << "行数: " << rows << ", 列数: " << cols << std::endl; 
    // 3.2 解析数据 -- 获取列名和数据
    MYSQL_FIELD* fields = mysql_fetch_fields(result);
    for (int i = 0; i < cols; ++i) 
    {
        std::cout << fields[i].name << "\t";
    } 
    std::cout << std::endl;
    
    for (int i = 0; i < rows; ++i) 
    {
        MYSQL_ROW line = mysql_fetch_row(result);
        for (int j = 0; j < cols; ++j) 
        {
            std::cout << line[j] << "\t";
        }
        std::cout << std::endl;
    }
    // 4、关闭数据库
    free(result);
    mysql_close(my);
    
    return 0;
}
