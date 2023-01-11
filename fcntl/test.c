#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void SetNoBlock(int fd)
{
    int fl = fcntl(fd, F_GETFL);
    if (fl < 0) {
        perror("fcntl");
        return;
    }

    fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}

int main()
{
    SetNoBlock(0);
    while (1) {
        char buffer[1024];
        ssize_t s = read(0, buffer, sizeof(buffer) - 1);
        if (s > 0) {
            buffer[s] = 0;
            write(1, buffer, strlen(buffer));
            printf("read success: len: %d\n",  s);
        }
        else {
            sleep(1);
            printf("read failed, try again, errno: %d\n", errno);
        }
    }

    return 0;
}