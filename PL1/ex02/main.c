#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
int main(void){
    
    pid_t pid = fork();
    if(pid == 0){
        printf("I'll never join you!\n");
        exit(0);
    }
    printf("Im..\n");
    pid = fork();
    if(pid == 0){
        printf("I'll never join you!\n");
        exit(0);
    }
    printf("the..\n");
    pid = fork();
    if(pid == 0){
        printf("I'll never join you!\n");
        exit(0);
    }
    printf("father!\n");
}