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
#include <stdbool.h>
#include <time.h>

typedef struct {
    int diff;
} sharedValues;

#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx06"

int main(void){

    int fd, status;
    pid_t pid;
    bool run = true;
    sem_t *sem;

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
    shared_data -> diff = 0;

    sem = sem_open("sema", O_CREAT | O_EXCL, 0644, 1);
    if(sem == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }

    pid = fork();
    if(pid < 0){
        perror("Erro ao criar o processo");
        exit(-1);
    }else if(pid == 0){
        while(run){
            if(shared_data -> diff > -2){
                sem_wait(sem);
                printf("C");
                fflush(stdout);
                shared_data -> diff--;
                sem_post(sem);
                usleep(500000);
            }else run = false;
        }

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
    time_t t;
    int random;
    while(run){
        srand ((unsigned) time(&t));
        random = (rand() % 2) + 1;
        if(shared_data -> diff < 2){
            sem_wait(sem);
            printf("S");
            fflush(stdout);
            shared_data -> diff++;
            sem_post(sem);
            sleep(random);
        }else run = false;
    }
    waitpid(pid, &status, 0);
    printf("\n");

    if(sem_close(sem) < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }
    if(sem_unlink("sema") < 0){
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