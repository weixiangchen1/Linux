#include "Sock.hpp"
#include "Protocol.hpp"

static void Usage(string proc)
{
    cout << "Usage: " << proc << " ip port" << endl;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    int sock = Sock::Socket();
    Sock::Connect(sock, argv[1], atoi(argv[2]));

    request_t req;
    memset(&req, 0, sizeof(req));
    cout << "Please Enter Data one:# ";
    cin >> req.x;
    cout << "Please Enter Data two:# ";
    cin >> req.y;
    cout << "Please Enter opreator:# ";
    cin >> req.op;

    // 序列化
    string josn_string = SerializeRequest(req);
    ssize_t s = write(sock, josn_string.c_str(), josn_string.size());

    char buffer[1024];
    s = read(sock, buffer, sizeof(buffer) - 1);
    if (s > 0)
    {
        response_t resp;
        buffer[s] = 0;
        string str = buffer;
        // 反序列化
        DeserializeResponse(str, resp);
        if (resp.code == 0)
        {
            cout << "cal code: " << resp.code << endl;
            cout << "cal result: " << resp.result << endl;
        }
        else if (resp.code == -1)
        {
            cout << "Divide by 0 errors!" << endl;
        }
        else if (resp.code == -2)
        {
            cout << "Modulo 0 error!" << endl;
        }
        else
        {
            cout << "Unknown error!" << endl;
        }
    }

    // ssize_t s = write(sock, &req, sizeof(req));
    // response_t resp;
    // s = read(sock, &resp, sizeof(resp));
    // if (s > 0) {
    //     cout << "cal code: " << resp.code << endl;
    //     cout << "cal result: " << resp.result << endl;
    // }

    return 0;
}