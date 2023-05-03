#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <semaphore.h>

typedef struct {
    int indice;
    char strings[50][80];
} sharedStrings;

#define DATA_SIZE sizeof(sharedStrings)
#define FILE_NAME "/shmEx02a"
#define NUMBER_OF_CHILDREN 10
#define STRING_LIMIT 50

int main(void){

    int fd, status, i;
    pid_t pidList[NUMBER_OF_CHILDREN];
    time_t t;
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
    
    sharedStrings *shared_data = (sharedStrings*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    shared_data -> indice = 0;

    sem = sem_open("sem", O_CREAT | O_EXCL, 0644, 1);
    if(sem == SEM_FAILED){
        perror("Erro ao criar o semaforo");
        exit(-1);
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            int randomNum;
            pid_t pid = getpid();
            srand ((unsigned) time(&t) * pid);
            while(shared_data -> indice < STRING_LIMIT){
                sem_wait(sem);
                sprintf(shared_data -> strings[shared_data->indice], "I'm the Father - with PID %d", pid);
                printf("String escrita na zona de memória: %s\n", shared_data->strings[shared_data -> indice]);
                shared_data -> indice++;
                randomNum = (rand() % 5) + 1;
                sem_post(sem);
                sleep(randomNum);
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
    }
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        waitpid(pidList[i], &status, 0);
    }
    if(sem_close(sem) < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }
    if(sem_unlink("sem") < 0){
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