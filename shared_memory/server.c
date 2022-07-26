#include "comm.h"

int main()
{
    // 获取key值
    key_t key = ftok(PATH_NAME, PROJ_ID);
    if(key < 0){
        perror("ftok");
        return 1;
    }
    
    // 申请共享内存
    int shmid = shmget(key, 4096, IPC_CREAT | IPC_EXCL | 0666);
    if(shmid < 0){
        perror("shmget");
        return 1;
    }  
    printf("key: %d, shmid: %d\n", key, shmid);
    //sleep(1);

    // 共享内存挂接
    char *mem = (char*)shmat(shmid, NULL, 0);
    printf("attches shm success\n");
    //sleep(5);

    // 业务逻辑
    while(1){
        printf("client# %s\n", mem);
        sleep(1);
    }


    // 共享内存脱离
    shmdt(mem);
    printf("detaches shm success\n");
    //sleep(5);
    
    // 删除共享内存
    shmctl(shmid, IPC_RMID, NULL);
    printf("delete shm success\n");

    return 0;
}
