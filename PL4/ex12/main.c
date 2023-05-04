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
#define BUFFER_SIZE 10
#define LIMIT 30

typedef struct {
    int buffer[BUFFER_SIZE];
    int index;
    int elementsNum;
} sharedValues;

#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx12"

int main(void){

    int fd, status, i;
    pid_t pidList[NUMBER_OF_CHILDREN];
    sem_t *semFull, *semEmpty, *sem3;

    fd = shm_open(FILE_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}

    if (ftruncate (fd, DATA_SIZE) < 0) {
        perror("Erro ao alocar espaço na memória");
        exit(-1);
    }
    
    sharedValues *shared_data = (sharedValues*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    shared_data -> index = 0;
    shared_data -> elementsNum = 0;

    semFull = sem_open("semaFull", O_CREAT | O_EXCL, 0644, 0);
    semEmpty = sem_open("semaEmpty", O_CREAT | O_EXCL, 0644, BUFFER_SIZE);
    sem3 = sem_open("sema3", O_CREAT | O_EXCL, 0644, 1);

    if(semFull == SEM_FAILED || semEmpty == SEM_FAILED || sem3 == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }
    
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            do{
                sem_wait(semEmpty);
                sem_wait(sem3);
                if(shared_data -> elementsNum < LIMIT){
                    shared_data -> elementsNum++;
                    shared_data -> buffer[shared_data -> index] = shared_data -> elementsNum;
                    shared_data -> index++;
                    if(shared_data -> index == 10){
                        shared_data -> index = 0;
                    }
                    printf("Processo %d produziu nº %d\n", i + 1, shared_data -> elementsNum);
                }
                sem_post(sem3);
                sem_post(semFull);
                sleep(1);
            }while(shared_data -> elementsNum < LIMIT);

            if(munmap(shared_data, DATA_SIZE) < 0){
            perror("Erro ao remover mapping");
            exit(-1);
            }

            if(close(fd) < 0){
                perror("Erro ao fechar file descriptor");
                exit(-1);
            }

            exit(0);
        }
    }
    int read = 0;
    for(i = 0; i < LIMIT; i++){
        sem_wait(semFull);
        sem_wait(sem3);

        printf("Número consumido: %d\n", shared_data -> buffer[read]);
        
        read++;
        if(read == BUFFER_SIZE){
            read = 0;
        }

        sem_post(sem3);
        sem_post(semEmpty);
        sleep(2);
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        waitpid(pidList[i], &status, 0);
    }

    if(sem_close(semFull) < 0 || sem_close(semEmpty) < 0 || sem_close(sem3) < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }
    if(sem_unlink("semaFull") < 0 || sem_unlink("semaEmpty") < 0 || sem_unlink("sema3") < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }

    if(munmap(shared_data, DATA_SIZE) < 0){
        perror("Erro ao remover mapping");
        exit(-1);
    }

    if(close(fd) < 0){
        perror("Erro ao fechar file descriptor");
        exit(-1);
    }

    if (shm_unlink(FILE_NAME) < 0) {
        perror("Erro ao remover o Ficheiro!");
        exit(-1);
    }

    return 0;
}