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
    
    int opt;

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
    shared_data -> run = 3;
    shared_data -> readCount = 0;
    shared_data -> writeCount = 0;
    shared_data -> index = 0;

    sem1 = sem_open("sem1", O_CREAT | O_EXCL, 0644, 1);
    sem2 = sem_open("sem2", O_CREAT | O_EXCL, 0644, 1);
    sem3 = sem_open("sem3", O_CREAT | O_EXCL, 0644, 1);
    semRead = sem_open("semaRead", O_CREAT | O_EXCL, 0644, 1);
    semWrite = sem_open("semaWrite", O_CREAT | O_EXCL, 0644, 1);

    if(sem1 == SEM_FAILED || sem2 == SEM_FAILED || sem3 == SEM_FAILED || semRead == SEM_FAILED || semWrite == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }

    do{
        sem_wait(sem2);
        shared_data -> writeCount++;
        if(shared_data -> writeCount == 1) sem_wait(semRead);
        sem_post(sem2);

        sem_wait(semWrite);

        printf("------Insert------\nNúmero: ");
        scanf("%d", &shared_data -> data[shared_data -> index].clientNum);
        printf("Nome: ");
        scanf("%s", shared_data -> data[shared_data -> index].clientName);
        printf("Endereço: ");
        scanf("%s", shared_data -> data[shared_data -> index].clientAddress);
        printf("------------------\n");
        shared_data -> index++;
        printf("A inserir os dados no sistema...\n");
        sleep(2);
        printf("Sucesso\n");

        sem_post(semWrite);

        sem_wait(sem2);
        shared_data -> writeCount--;
        if (shared_data -> writeCount == 0) sem_post(semRead);
        sem_post(sem2);
        do{
            printf("Deseja adicionar mais algum cliente?\n");
            scanf("%d", &opt);
        }while(opt < 0 || opt > 1);
    }while(opt == 1 && shared_data -> index < LIMIT);
    shared_data -> run--;

    if(shared_data -> run == 0){
        if(sem_close(sem1) < 0 || sem_close(sem2) < 0 || sem_close(sem3) < 0 || sem_close(semRead) < 0 || sem_close(semWrite) < 0){
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
        if(sem_unlink("sem1") < 0 || sem_unlink("sem2") < 0 || sem_unlink("sem3") < 0 || sem_unlink("semaRead") < 0 || sem_unlink("semaWrite") < 0){
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