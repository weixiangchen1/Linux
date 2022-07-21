#include <stdio.h>

int main()
{
    FILE* fp = fopen("./log.txt", "r");
    if(fp == NULL){
        perror("fopen");
        return 1;
    }
    
    char buffer[64];
    while(fgets(buffer, sizeof(buffer), fp)){
        printf("%s\n", buffer);
    }

    if(!feof(fp)){
        printf("fgets quit not normal\n");
    }
    else{
        printf("fgets quit normal\n");
    }

    fclose(fp);


    //FILE* fp = fopen("./log.txt", "w");
    //if(fp == NULL){
    //    perror("fopen");
    //    return 1;
    //}

    //const char* msg = "hello world\n";
    //int cnt = 5;
    //while(cnt--){
    //    fputs(msg, fp);
    //}
    //
    //fclose(fp);

    return 0;
}
