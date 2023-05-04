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

#define FULL_CAPACITY 10
#define NUMBER_OF_CHILDREN 25

typedef struct {
    int vipNum;
    int specialNum;
    int normalNum;
} bar;

#define DATA_SIZE sizeof(bar)
#define FILE_NAME "/shmEx15b"

int main(void){

    int fd, status, i;
    pid_t pidList[NUMBER_OF_CHILDREN];
    sem_t *semClub, *semVip, *semSpecial, *semNormal;

    fd = shm_open(FILE_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}

    if (ftruncate (fd, DATA_SIZE) < 0) {
        perror("Erro ao alocar espaço na memória");
        exit(-1);
    }
    
    bar *shared_data = (bar*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    shared_data -> vipNum = 0;
    shared_data -> specialNum = 0;
    shared_data -> normalNum = 0;

    semClub = sem_open("semaClub", O_CREAT | O_EXCL, 0644, FULL_CAPACITY);
    semVip = sem_open("semaVip", O_CREAT | O_EXCL, 0644, 0);
    semSpecial = sem_open("semaSpecial", O_CREAT | O_EXCL, 0644, 0);
    semNormal = sem_open("semaNormal", O_CREAT | O_EXCL, 0644, 0);
    if(semClub == SEM_FAILED || semVip == SEM_FAILED || semSpecial == SEM_FAILED || semNormal == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }
    
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            time_t t;
            char client[10];
            srand ((unsigned) time(&t) * getpid());
            int type = (rand() % 3) + 1;

            if(type == 1){
                strcpy(client, "Vip");
                shared_data -> vipNum++;
                sem_wait(semVip);
            }else if(type == 2){
                strcpy(client, "Especial");
                shared_data -> specialNum++;
                sem_wait(semSpecial);
            }else{
                strcpy(client, "Normal");
                shared_data -> normalNum++;
                sem_wait(semNormal);
            }
            sem_wait(semClub);
            printf("Cliente %d [%s] entrou no clube\n", i + 1, client);
            fflush(stdout);
            int time = (rand() % 7) + 5;
            sleep(time);
            sem_post(semClub);
            printf("Cliente %d [%s] saiu do clube (permaneceu lá %d segundos)\n", i + 1, client, time);
            fflush(stdout);

            exit(0);
        }
    }

    sleep(2);
    for(i = 0; i < shared_data -> vipNum; i++){
        sem_post(semVip);
    }
    sleep(1);
    for(i = 0; i < shared_data -> specialNum; i++){
        sem_post(semSpecial);
    }
    sleep(1);
    for(i = 0; i < shared_data -> normalNum; i++){
        sem_post(semNormal);
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        waitpid(pidList[i], &status, 0);
    }

    if(sem_close(semClub) < 0 || sem_close(semVip) < 0 || sem_close(semSpecial) < 0 || sem_close(semNormal) < 0){ 
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }
    if(sem_unlink("semaClub") < 0 || sem_unlink("semaVip") < 0 || sem_unlink("semaSpecial") < 0 || sem_unlink("semaNormal") < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }

    return 0;
}