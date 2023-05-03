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
#define FILE_NAME "/shmEx07a"
#define NUMBER_OF_PROCESSES 2

void buy_beer(){
    printf("P2 a comprar cerveja\n");
}

void buy_chips(){
    printf("P1 a comprar batatas\n");
}

void eat_and_drink(){
    printf("P1 e P2 a comer e a beber\n");
}

int main(void){

    int fd, status, random;
    pid_t pid;
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

    pid = fork();
    if(pid < 0){
        perror("Erro ao criar o processo");
        exit(-1);
    }else if(pid == 0){

        srand ((unsigned) time(&t) * getpid());
        random = (rand() % 5) + 1;
        sleep(random);
        buy_beer();
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
    srand ((unsigned) time(&t) * getpid());
    random = (rand() % 5) + 1;
    sleep(random);
    buy_chips();
    sem_wait(semMem);
    shared_data -> barrier++;
    sem_post(semMem);
    if (shared_data -> barrier == NUMBER_OF_PROCESSES) sem_post(semBar);
    sem_wait(semBar);
    sem_post(semBar);
    eat_and_drink();
    waitpid(pid, &status, 0);
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