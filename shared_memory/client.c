#include "comm.h"

int main()
{
    key_t key = ftok(PATH_NAME, PROJ_ID);
    if(key < 0){
        perror("ftok");
        return 1;
    }

    int shmid = shmget(key, 4096, IPC_CREAT);
    if(shmid < 0){
        perror("shmget");
        return 1;
    }

    char *mem = (char*)shmat(shmid, NULL, 0);
    printf("client process attches success\n");
    //sleep(3);
    
    char ch = 'A';
    while(ch <= 'Z'){
        mem[ch - 'A'] = ch;
        ch++;
        mem[ch - 'A'] = '\0';
        sleep(2);
    }


    shmdt(mem);
    printf("client process detaches success\n");


    return 0;
}
