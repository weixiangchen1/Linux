#include "cpp-httplib/httplib.h"

int main()
{
    httplib::Server server;
    server.Get("/hello", [](const httplib::Request& req, httplib::Response& resp){
        resp.set_content("hello http", "text/plain; charset=UTF-8");
    });

    server.listen("0.0.0.0", 8081);
    
    return 0;
}
