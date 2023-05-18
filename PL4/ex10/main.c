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

#define NUMBER_OF_CHILDREN 40
#define MAX_PASSENGERS 20
#define NUMBER_OF_DOORS 3

typedef struct {
    int passengersNum;
    int passengersId[MAX_PASSENGERS];
    int doors[NUMBER_OF_DOORS];
    int index;
} train;

#define DATA_SIZE sizeof(train)
#define FILE_NAME "/shmEx10"

int main(void){

    int fd, status, i;
    pid_t pidList[NUMBER_OF_CHILDREN];
    time_t t;
    sem_t *semFull, *semEmpty, *sem3, *semDoor1, *semDoor2, *semDoor3;

    fd = shm_open(FILE_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}

    if (ftruncate (fd, DATA_SIZE) < 0) {
        perror("Erro ao alocar espaço na memória");
        exit(-1);
    }
    
    train *shared_data = (train*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    shared_data -> passengersNum = 0;
    shared_data -> index = 0;

    semFull = sem_open("semaFull", O_CREAT /*| O_EXCL*/, 0644, 0);
    semEmpty = sem_open("semaEmpty", O_CREAT /*| O_EXCL*/, 0644, 20);
    sem3 = sem_open("sema3", O_CREAT /*| O_EXCL*/, 0644, 1);
    semDoor1 = sem_open("semDoor1", O_CREAT /*| O_EXCL*/, 0644, 1);
    semDoor2 = sem_open("semDoor2", O_CREAT /*| O_EXCL*/, 0644, 1);
    semDoor3 = sem_open("semDoor3", O_CREAT /*| O_EXCL*/, 0644, 1);
    if(semFull == SEM_FAILED || semEmpty == SEM_FAILED || sem3 == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }
    printf("Metro parte em direção á estação A com os passageiros...\n");
    sleep(5);
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            srand ((unsigned) time(&t) * getpid());
            int entered = 0, door = (rand() % 3) + 1;

            sem_wait(semEmpty);
            sem_wait(sem3);
            if(shared_data -> passengersNum < 15){

                shared_data -> passengersNum++;
                shared_data -> passengersId[shared_data -> index] = i;
                shared_data -> index++;
                printf("Passageiro %d chega á estação A\n", i + 1);
                entered = 1;

            }
            sem_post(sem3);

            if(entered == 1){
                if(door == 1){
                    sem_wait(semDoor1);
                    sleep(1);
                    printf("Passageiro %d saiu pela porta %d\n", i + 1, door);
                    sem_post(semDoor1);
                }else if(door == 2){
                    sem_wait(semDoor2);
                    sleep(1);
                    printf("Passageiro %d saiu pela porta %d\n", i + 1, door);
                    sem_post(semDoor2);
                }else{
                    sem_wait(semDoor3);
                    sleep(1);
                    printf("Passageiro %d saiu pela porta %d\n", i + 1, door);
                    sem_post(semDoor3);
                }
                sem_post(semEmpty);
                exit(0);
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

    /*for(i = 0; i < 5; i++){
        sem_wait(semFull);
        sem_wait(sem3);
        
        shared_data -> passengersNum--;
        printf("boas\n");

        sem_post(sem3);
        sem_post(semEmpty);
    }*/

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        waitpid(pidList[i], &status, 0);
    }
    printf("%d\n", shared_data -> passengersNum);

    if(sem_close(semFull) < 0 || sem_close(semEmpty) < 0 || sem_close(sem3) < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }
    if(sem_unlink("semaFull") < 0 || sem_unlink("semaEmpty") < 0 || sem_unlink("sema3") < 0){
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