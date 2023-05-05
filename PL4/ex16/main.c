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

#define NUMBER_OF_CHILDREN 6
#define INITIAL_SIZE 10000
#define FINAL_SIZE 1000
#define AVERAGE_NUM 10

typedef struct {
    int initialVec[INITIAL_SIZE];
    int finalVec[FINAL_SIZE];
    int largeNum;
    int barrier;
} sharedValues;

#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx16"

int main(void){

    int fd, status, i;
    time_t t;
    pid_t pidList[NUMBER_OF_CHILDREN];
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

    srand ((unsigned) time(&t));

    for(i = 0; i < INITIAL_SIZE; i++) {
        shared_data -> initialVec[i] = (rand() % 10000) + 1;
    }

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
        }else if(pidList[i] == 0 && i != 5){
            int j, k, sum, start, end, index;
            start = i * (INITIAL_SIZE / 5);
            end = (i + 1) * (INITIAL_SIZE / 5) - 1;

            for(j = start; j < end; j++){
                sum = 0;
                index = j + AVERAGE_NUM;
                for (k = j; k < index; k++) {
                    sum += shared_data -> initialVec[k];
                }
                shared_data -> finalVec[j/AVERAGE_NUM] = sum / AVERAGE_NUM;
            }

            sem_wait(semMem);
            shared_data -> barrier++;
            sem_post(semMem);
            if (shared_data -> barrier == NUMBER_OF_CHILDREN) sem_post(semBar);
            sem_wait(semBar);
            sem_post(semBar);

            if(munmap(shared_data, DATA_SIZE) < 0){
            perror("Erro ao remover mapping");
            exit(-1);
            }

            if(close(fd) < 0){
                perror("Erro ao fechar file descriptor");
                exit(-1);
            }

            exit(0);
        }else if(pidList[i] == 0){
            int max = 0, j;
            sem_wait(semMem);
            shared_data -> barrier++;
            sem_post(semMem);
            if (shared_data -> barrier == NUMBER_OF_CHILDREN) sem_post(semBar);
            sem_wait(semBar);
            sem_post(semBar);

            for(j = 0; j < FINAL_SIZE; j++){
                if(shared_data -> finalVec[j] > max){
                    max = shared_data -> finalVec[j];
                    printf("Atenção!! novo valor mais alto encontrado: %d\n", max);
                    fflush(stdout);
                }
            }
            shared_data -> largeNum = max;

            exit(0);
        }
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        waitpid(pidList[i], &status, 0);
    }

    for(i = 0; i < FINAL_SIZE; i++){
        printf("vec[%d]: %d\n", i, shared_data -> finalVec[i]);
        fflush(stdout);
    }
    printf("Maior número encontrado: %d\n", shared_data -> largeNum);

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