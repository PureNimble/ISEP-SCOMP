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

typedef struct {
    int barrier;
} sharedValues;

#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx07b"
#define NUMBER_OF_PROCESSES 6
#define NUMBER_OF_CHILDREN 5

void buy_beer(int n){
    printf("P%d a comprar cerveja\n", n);
}

void buy_chips(int n){
    printf("P%d a comprar batatas\n", n);
}

void eat_and_drink(){
    printf("P1, P2, P3, P4, P5 e P6 a comer e a beber\n");
}

int main(void){

    int fd, status, random, i;
    pid_t pidList[NUMBER_OF_CHILDREN];
    time_t t;
    sem_t *semMem, *semBar;

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
    shared_data -> barrier = 0;

    semMem = sem_open("semaMemory", O_CREAT | O_EXCL, 0644, 1);
    semBar = sem_open("semaBarrier", O_CREAT | O_EXCL, 0644, 0);
    if(semMem == SEM_FAILED || semBar == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){

            srand ((unsigned) time(&t) * getpid());
            random = (rand() % 5) + 1;
            sleep(random);
            if(i > 0){
                int prob;
                prob = (rand() % 10) + 1;
                if(prob > 5){
                    buy_beer(i+2);
                }else buy_chips(i+2);
            }else buy_beer(i+2);
            sem_wait(semMem);
            shared_data -> barrier++;
            sem_post(semMem);
            if (shared_data -> barrier == NUMBER_OF_PROCESSES) sem_post(semBar);
            sem_wait(semBar);
            sem_post(semBar);
            eat_and_drink();

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
    srand ((unsigned) time(&t) * getpid());
    random = (rand() % 5) + 1;
    sleep(random);
    buy_chips(1);
    sem_wait(semMem);
    shared_data -> barrier++;
    sem_post(semMem);
    if (shared_data -> barrier == NUMBER_OF_PROCESSES) sem_post(semBar);
    sem_wait(semBar);
    sem_post(semBar);
    eat_and_drink();
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        waitpid(pidList[i], &status, 0);
    }
    printf("\n");

    if(sem_close(semMem) < 0 || sem_close(semBar) < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }
    if(sem_unlink("semaMemory") < 0 || sem_unlink("semaBarrier") < 0){
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