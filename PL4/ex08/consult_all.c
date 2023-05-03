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

int main(void){

    int i, opt, max;

    fd = shm_open(FILE_NAME, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}
    
    sharedValues *shared_data = (sharedValues*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    mutex1 = sem_open("mutex1", O_CREAT);
    mutex2 = sem_open("mutex2", O_CREAT);
    mutex3 = sem_open("mutex3", O_CREAT);
    semRead = sem_open("semaRead", O_CREAT);
    semWrite = sem_open("semaWrite", O_CREAT);

    if(mutex1 == SEM_FAILED || mutex2 == SEM_FAILED || mutex3 == SEM_FAILED || semRead == SEM_FAILED || semWrite == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }
    
    do{
        sem_wait(mutex3);
        sem_wait(semRead);
        sem_wait(mutex1);
        shared_data -> readCount++;
        if(shared_data -> readCount == 1) sem_wait(semWrite);
        sem_post(mutex1);
        sem_post(semRead);
        sem_post(mutex3);
        
        printf("A procurar clientes no sistema...\n");
        sleep(2);
        max = shared_data -> index;
        for(i = 0; i < max; i++){
            printf("------------------\n");
            printf("Número: %d\n", shared_data -> data[i].clientNum);
            printf("Nome: %s\n", shared_data -> data[i].clientName);
            printf("Enderesso: %s\n", shared_data -> data[i].clientAddress);
            printf("------------------\n");
        }

        sem_wait(mutex1);
        shared_data -> readCount--;
        if (shared_data -> readCount == 0) sem_post(semWrite);
        sem_post(mutex1);
        
        do{
            printf("Deseja rever a lista de clientes?\n");
            scanf("%d", &opt);
        }while(opt < 0 || opt > 1);

    }while(opt == 1);

    shared_data -> run--;

    if(sem_close(mutex1) < 0 || sem_close(mutex2) < 0 || sem_close(mutex3) < 0 || sem_close(semRead) < 0 || sem_close(semWrite) < 0){
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

    if(shared_data -> run == 0){
        if(sem_unlink("mutex1") < 0 || sem_unlink("mutex2") < 0 || sem_unlink("mutex3") < 0 || sem_unlink("semaRead") < 0 || sem_unlink("semaWrite") < 0){
            perror("Erro ao fechar semaforo\n");
            exit(-1);
        }

        if (shm_unlink(FILE_NAME) < 0) {
            perror("Erro ao remover o Ficheiro!");
            exit(-1);
        }
    }

    return 0;
}