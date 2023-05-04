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

#define FULL_CAPACITY 10
#define NUMBER_OF_CHILDREN 25

int main(void){

    int status, i;
    pid_t pidList[NUMBER_OF_CHILDREN];
    sem_t *sem;

    sem = sem_open("sema", O_CREAT | O_EXCL, 0644, FULL_CAPACITY);
    if(sem == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }
    
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            time_t t;
            srand ((unsigned) time(&t) * getpid());

            sem_wait(sem);
            printf("Cliente %d entrou no clube\n", i + 1);
            fflush(stdout);
            int time = (rand() % 7) + 5;
            sleep(time);
            sem_post(sem);
            printf("Cliente %d saiu do clube\n", i + 1);
            fflush(stdout);

            exit(0);
        }
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        waitpid(pidList[i], &status, 0);
    }

    if(sem_close(sem) < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }
    if(sem_unlink("sema") < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }

    return 0;
}