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
    int stationA;
    int stationB;
    int stationC;
} train;

#define DATA_SIZE sizeof(train)
#define FILE_NAME "/shmEx10"

int main(void){

    int fd, status, i;
    pid_t pidList[NUMBER_OF_CHILDREN];
    time_t t;
    sem_t *semTrain, *semDoor[NUMBER_OF_DOORS], *semSafe;

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
    shared_data -> stationA = 0;
    shared_data -> stationB = 0;
    shared_data->stationC = 0;

    semTrain = sem_open("semaTrain", O_CREAT | O_EXCL, 0644, 20);
    semSafe = sem_open("semaSafe", O_CREAT | O_EXCL, 0644, 1);
    semDoor[0] = sem_open("semaDoor1", O_CREAT | O_EXCL, 0644, 1);
    semDoor[1] = sem_open("semaDoor2", O_CREAT | O_EXCL, 0644, 1);
    semDoor[2] = sem_open("semaDoor3", O_CREAT | O_EXCL, 0644, 1);

    if(semTrain == SEM_FAILED || semDoor[0] == SEM_FAILED || semDoor[1] == SEM_FAILED || semDoor[2] == SEM_FAILED || semSafe == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }
    printf("Metro parte em direção á estação A com os passageiros...\n");
    fflush(stdout);
    sleep(5);

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            srand((unsigned)time(&t) * getpid());
            int entered, door = rand() % 3;

            sem_wait(semSafe);
            if(shared_data -> stationA < 15){
                sem_wait(semTrain);
                shared_data -> stationA++;
                printf("Passageiro %d chega á estação A\n", i + 1);
                fflush(stdout);
                entered = 1;
            }
            sem_post(semSafe);

            sleep(1);

            if(entered == 1){
                sem_wait(semSafe);
                if(shared_data -> stationA > 10){
                    shared_data -> stationA--;
                    sem_wait(semDoor[door]);
                    printf("Passageiro %d sai na estação A pela porta %d\n", i + 1, door + 1);
                    fflush(stdout);
                    usleep(200000);
                    sem_post(semDoor[door]);
                    sem_post(semTrain);
                    sem_post(semSafe);
                    exit(0);
                }
                sem_post(semSafe);
            }else{
                printf("Passageiro %d á espera na estação A\n", i + 1);
                fflush(stdout);
                sem_wait(semSafe);
                if(shared_data -> stationB < 10){
                    shared_data -> stationB++;
                    sem_wait(semDoor[door]);
                    printf("Passageiro %d entra no metro na estação A pela porta %d\n", i + 1, door + 1);
                    fflush(stdout);
                    usleep(200000);
                    sem_post(semDoor[door]);
                    sem_wait(semTrain);
                    entered = 1;
                }
                sem_post(semSafe);
            }

            sleep(2);

            printf("Passageiro %d parte em direção á estação B\n", i + 1);
            fflush(stdout);
            sleep(5);

            if(entered == 1){
                sem_wait(semSafe);
                if(shared_data -> stationB > 0){
                    shared_data -> stationB--;
                    sem_wait(semDoor[door]);
                    printf("Passageiro %d sai na estação B pela porta %d\n", i + 1, door + 1);
                    fflush(stdout);
                    usleep(200000);
                    sem_post(semDoor[door]);
                    sem_post(semTrain);
                    sem_post(semSafe);
                    exit(0);
                }
                sem_post(semSafe);
            }else{
                printf("Passageiro %d á espera na estação B\n", i + 1);
                fflush(stdout);
                sem_wait(semSafe);
                if (shared_data -> stationC < 5) {
                    shared_data -> stationC++;
                    sem_wait(semDoor[door]);
                    printf("Passageiro %d entra no metro na estação B pela porta %d\n", i + 1, door + 1);
                    fflush(stdout);
                    usleep(200000);
                    sem_post(semDoor[door]);
                    sem_wait(semTrain);
                }
                sem_post(semSafe);
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

    if(sem_close(semTrain) < 0 || sem_close(semSafe) < 0 || sem_close(semDoor[0]) < 0 || sem_close(semDoor[1]) < 0 || sem_close(semDoor[2]) < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }
    if(sem_unlink("semaTrain") < 0 || sem_unlink("semaSafe") < 0 || sem_unlink("semaDoor1") < 0 || sem_unlink("semaDoor2") < 0 || sem_unlink("semaDoor3") < 0){
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