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

#define NUMBER_OF_CHILDREN 5
#define READINGS 6
#define TOTAL_READ 5 * READINGS

typedef struct {
    int sensors[NUMBER_OF_CHILDREN][7];
    int sensorNum;
    int indice;
    int alarm;
} sensors;

#define DATA_SIZE sizeof(sensors)
#define FILE_NAME "/shmEx17"

int main(void){

    int fd, status, i;
    time_t t;
    pid_t pidList[NUMBER_OF_CHILDREN];
    sem_t *semRead, *semWrite;

    fd = shm_open(FILE_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}

    if (ftruncate (fd, DATA_SIZE) < 0) {
        perror("Erro ao alocar espaço na memória");
        exit(-1);
    }
    
    sensors *shared_data = (sensors*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    shared_data -> alarm = 0;

    semRead = sem_open("semaRead", O_CREAT | O_EXCL, 0644, 0);
    semWrite = sem_open("semaWrite", O_CREAT | O_EXCL, 0644, 1);

    if(semRead == SEM_FAILED || semWrite == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }
    
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            int reading, j;
            srand ((unsigned) time(&t) * getpid());

            for(j = 0; j < READINGS; j++){
                reading = rand() % 101;
                sem_wait(semWrite);
                printf("Sensor %d a ler...\n", i + 1);
                fflush(stdout);
                shared_data -> indice = j;
                shared_data -> sensorNum = i;
                shared_data -> sensors[i][j] = reading;
                if(reading > 50){
                    if(shared_data -> sensors[i][6] != 1){
                        shared_data -> alarm++;
                        shared_data -> sensors[i][6] = 1;
                    }
                }else if(reading <= 50 && j != 1){
                    if(shared_data -> sensors[i][j-1] < 50 && shared_data -> sensors[i][6] != 0){
                        shared_data -> alarm--;
                        shared_data -> sensors[i][6] = 0;
                    }
                }
                sem_post(semRead);
                sleep(1);
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
    int alarmStorage = 0;
    for(i = 0; i < TOTAL_READ; i++){
        sem_wait(semRead);
        printf("A %d leitura do sensor %d foi %d\n", shared_data -> indice + 1, shared_data -> sensorNum + 1, shared_data -> sensors[shared_data -> sensorNum][shared_data -> indice]);
        fflush(stdout);
        if(alarmStorage != shared_data -> alarm){
            alarmStorage = shared_data -> alarm;
            printf("Atenção!! O número de alarmes mudou para %d\n", shared_data -> alarm);
            fflush(stdout);
        }
        sem_post(semWrite);
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        waitpid(pidList[i], &status, 0);
    }

    if(sem_close(semRead) < 0 || sem_close(semWrite) < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }
    if(sem_unlink("semaRead") < 0 || sem_unlink("semaWrite") < 0){
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