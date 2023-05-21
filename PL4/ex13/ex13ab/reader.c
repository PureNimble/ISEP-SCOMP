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
#include "asm.h"

#define NUMBER_OF_CHILDREN 100

int main(void){

    int status, i;
    pid_t pidList[NUMBER_OF_CHILDREN];

    fd = shm_open(FILE_NAME, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}
    
    sharedValues *shared_data = (sharedValues*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    semWriter = sem_open("semaWriter", O_CREAT);
    sem1 = sem_open("sema1", O_CREAT);

    if(semWriter == SEM_FAILED || sem1 == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }
   
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            
            sleep(1);

            sem_wait(sem1);

            shared_data -> readCount++;
            if(shared_data -> readCount == 1) sem_wait(semWriter);

            sem_post(sem1);

            printf("Reader: Número de leitores: %d\n", shared_data -> readCount);
            fflush(stdout);
            printf("Leitor leu: pid[%d] escrito ás %s\n", shared_data -> pid, shared_data -> currentTime);
            fflush(stdout);

            sem_wait(sem1);
            shared_data -> readCount--;
            if(shared_data -> readCount == 0) sem_post(semWriter);
            sem_post(sem1);

            exit(0);
        }
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        waitpid(pidList[i], &status, 0);
    }
    if(sem_close(semWriter) < 0 || sem_close(sem1) < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }

    if(sem_unlink("semaWriter") < 0 || sem_unlink("sema1") < 0){
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