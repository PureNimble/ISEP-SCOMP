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

#define NUMBER_OF_CHILDREN 3

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
            int nextId = i + 1;
            if(i == 0){
                printf("Sistemas ");
                fflush(stdout);
                sem_post(sem[nextId]);
                sem_wait(sem[i]);
                printf("a ");
                fflush(stdout);
                sem_post(sem[nextId]);
            }else if(i == 1){
                sem_wait(sem[i]);
                printf("de ");
                fflush(stdout);
                sem_post(sem[nextId]);
                sem_wait(sem[i]);
                printf("melhor ");
                fflush(stdout);
                sem_post(sem[nextId]);
            }else{
                sem_wait(sem[i]);
                printf("computadores - ");
                fflush(stdout);
                sem_post(sem[0]);
                sem_wait(sem[i]);
                printf("disciplina!\n");
                fflush(stdout);
            }

            exit(0);
        }
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