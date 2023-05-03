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

#define NUMBER_OF_CHILDREN 2
#define NUMBER_OF_EXECUTIONS 15

int main(void){

    int status, i;
    pid_t pidList[NUMBER_OF_CHILDREN];
    char semName[50];
    sem_t *sem[NUMBER_OF_CHILDREN];

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        snprintf(semName, sizeof(semName), "sema%d", i);
        sem[i] = sem_open(semName, O_CREAT | O_EXCL, 0644, 0);
        if(sem[i] == SEM_FAILED){
            perror("Erro no criar/abrir semaforo");
            exit(-1);
        }
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            int j;
            for(j = 0; j < NUMBER_OF_EXECUTIONS; j++){
                if(i == 0){
                    printf("1st child\n");
                    sem_post(sem[0]);
                }else{
                    sem_wait(sem[1]);
                    printf("2nd child\n");
                }
                sleep(1);
            }

            exit(0);
        }
    }
    for(i = 0; i < NUMBER_OF_EXECUTIONS; i++){
        sem_wait(sem[0]);
        printf("Father\n");
        sem_post(sem[1]);
    }
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        waitpid(pidList[i], &status, 0);
    }
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        if(sem_close(sem[i]) < 0){
            perror("Erro ao fechar semaforo\n");
            exit(-1);
        }
        snprintf(semName, sizeof(semName), "sema%d", i);
        if(sem_unlink(semName) < 0){
            perror("Erro ao fechar semaforo\n");
            exit(-1);
        }
    }

    return 0;
}