#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <time.h>

int main(int argc, char *argv[]){

    if (argc != 3) {
        printf("Erro: Número de argumentos invalido!\n");
        exit(-1);
    }
    int NUMBER_OF_CHILDREN = atoi(argv[1]), time = atoi(argv[2]);
    if(time < 100 || time > 1000000){
        printf("Erro: Tempo deveria ser entre 100 us e 1s.\n");
        exit(-1);
    }
    int status, i;
    long seconds, ns, us;
    pid_t pidList[NUMBER_OF_CHILDREN];
    struct timespec start, end;

    clock_gettime(CLOCK_REALTIME, &start);
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            usleep(time);
            exit(0);
        }
    }
    
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        waitpid(pidList[i], &status, 0);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    seconds = end.tv_sec - start.tv_sec;
    ns = end.tv_nsec - start.tv_nsec;
    us = seconds * 1000000 + ns / 1000;

    printf("Tempo de processamento: %ld us\n", us);

    return 0;
}