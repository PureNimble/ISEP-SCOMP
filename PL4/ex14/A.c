#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <time.h>
#include "asm.h"

#define NUMBER_OF_CHILDREN 3

int main(void){

    int i, random, status;
    pid_t pidList[NUMBER_OF_CHILDREN];
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
    shared_data -> run = 2;
    semA = sem_open("semaA", O_CREAT /*| O_EXCL*/, 0644, 1);
    semB = sem_open("semaB", O_CREAT /*| O_EXCL*/, 0644, 1);
    semWrite = sem_open("semaWrite", O_CREAT /*| O_EXCL*/, 0644, 1);

    if(semA == SEM_FAILED || semB == SEM_FAILED || semWrite == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            int j, k, l;
            srand ((unsigned) time(&t) * getpid());

            for(j = 0; j < ARRAY_SIZE; j++){
                data[j] = (rand() % 101) + 1;
            }
            sleep(5);

            sem_wait(semA);
            sem_wait(semWrite);
            
            printf("Processo %d do set A a escrever na zona de memoria partilhada.\n", i + 1);
            for(k = 0; k < ARRAY_SIZE; k++){
                shared_data -> array[k] = data[k];
            }
            sleep(1);
            printf("Processo %d do set A terminou de escrever na zona de memoria partilhada e notificou outros sets.\n", i + 1);

            sem_post(semWrite);
            sem_post(semA);
            sem_post(semB);
            
            exit(0);
        }
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        waitpid(pidList[i], &status, 0);
    }

    shared_data -> run--;
    if(shared_data -> run == 0){
        if(sem_close(semA) < 0 || sem_close(semB) < 0 || sem_close(semWrite) < 0){
            perror("Erro ao fechar semaforo\n");
            exit(-1);
        }

        if(sem_unlink("semaA") < 0 || sem_unlink("semaB") < 0 || sem_unlink("semaWrite") < 0){
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
    }

    return 0;
}