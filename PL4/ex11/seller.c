#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include "asm.h"

#define NUMBER_OF_TICKETS 15

int main(void){

    int i, random;
    time_t t;

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
    shared_data -> ticketsNum = 15;

    semCostumer = sem_open("semaCostumer", O_CREAT | O_EXCL, 0644, 0);
    semSeller = sem_open("semaSeller", O_CREAT | O_EXCL, 0644, 0);

    if(semCostumer == SEM_FAILED || semSeller == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }

    for(i = 0; i < NUMBER_OF_TICKETS; i++){
        srand ((unsigned) time(&t) * getpid());
        random = (rand() % 10) + 1;

        printf("A vender bilhete %d: ", i + 1);
        printf("Transação em curso...\n");
        sleep(random);
        shared_data -> ticket = i + 1;
        shared_data -> ticketsNum--;
        sem_post(semCostumer);
        sem_wait(semSeller);
    }


    if(sem_close(semCostumer) < 0 || sem_close(semSeller) < 0){
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

    return 0;
}