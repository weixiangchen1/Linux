#include "Sock.hpp"
#include <pthread.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>

#define SIZE 10240
#define WWWROOT "./wwwroot/"
#define HOME_PAGE "index.html"

void* HandlerHttpRequest(void* args)
{
    int sock = *(int*)args;
    delete (int*)args;  
    pthread_detach(pthread_self());

    char buffer[SIZE];
    memset(buffer, 0, sizeof(buffer));
    ssize_t s = recv(sock, buffer, sizeof(buffer), 0);
    if (s > 0) {
        buffer[s] = 0;
        cout << buffer;
        string html_file = WWWROOT;
        html_file += HOME_PAGE;

        ifstream in(html_file);
        if (!in.is_open()) {
            string http_request = "http/1.0 404 Not Found\n";
            http_request += "Content-Type: text/html; charset: utf-8\n";
            http_request += "\n";
            http_request += "<html><p>你访问的资源不存在</p></html>";
            send(sock, http_request.c_str(), http_request.size(), 0);
        }
        else {
            struct stat st;
            stat(html_file.c_str(), &st);
            string http_request = "http/1.0 200 OK\n";
            http_request += "Content-Type: text/html; charset: utf-8\n";
            http_request += "Content-Length: ";
            http_request += to_string(st.st_size);
            http_request += "\n\n";

            string content;
            string line;
            while (getline(in, line)) {
                content += line;
            }
            http_request += content;
            in.close();
            send(sock, http_request.c_str(), http_request.size(), 0);
        }
    }

    close(sock);
    return nullptr;
}

void Usage(string proc)
{
    cout << "Usage: " << proc << " port" << endl;
    exit(1);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        Usage(argv[0]);
    }

    uint16_t port = atoi(argv[1]);
    int listen_sock = Sock::Socket();
    Sock::Bind(listen_sock, port);
    Sock::Listen(listen_sock);

    for( ; ; ) {
        int sock = Sock::Accept(listen_sock);
        if (sock > 0) {
            pthread_t pid;
            int* parm = new int(sock);
            pthread_create(&pid, nullptr, HandlerHttpRequest, parm);
        }
    }

    return 0;
}