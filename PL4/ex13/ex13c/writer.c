#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/time.h>
#include "asm.h"

#define NUMBER_OF_CHILDREN 50

int main(void){

    int i, status;
    pid_t pidList[NUMBER_OF_CHILDREN];
    struct tm* timeinfo;
    struct timeval tv;

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
    shared_data -> readCount = 0;
    shared_data -> writeCount = 0;
    shared_data -> activeWriteCount = 0;

    semWriter = sem_open("semaWriter", O_CREAT | O_EXCL, 0644, 1);
    semReader = sem_open("semaReader", O_CREAT | O_EXCL, 0644, 1);
    sem1 = sem_open("sema1", O_CREAT | O_EXCL, 0644, 1);
    sem2 = sem_open("sema2", O_CREAT | O_EXCL, 0644, 1);
    sem3 = sem_open("sema3", O_CREAT | O_EXCL, 0644, 1);

    if(semWriter == SEM_FAILED || semReader == SEM_FAILED || sem1 == SEM_FAILED || sem2 == SEM_FAILED || sem3 == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            char buffer[20];
            int random;
            pid_t pid = getpid();
            time_t curtime, t;

            /*srand ((unsigned) time(&t) * pid);
            random = rand() % 400000;
            random += 800000;

            usleep(random);*/

            sem_wait(sem2);
            shared_data -> writeCount++;
            if(shared_data -> writeCount == 1) sem_wait(semReader);
            sem_post(sem2);

            sem_wait(semWriter);
            shared_data -> activeWriteCount++;

            shared_data -> pid = pid;

            gettimeofday(&tv, NULL);
            curtime = tv.tv_sec;
            timeinfo = localtime(&curtime);
            strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
            sprintf(shared_data -> currentTime, "%s.%03ld", buffer, tv.tv_usec / 1000);

            printf("Writer: Número de escritores: %d\n", shared_data -> writeCount);
            fflush(stdout);
             printf("Writer: Número de escritores ativos: %d\n", shared_data -> activeWriteCount);
            fflush(stdout);
            printf("Writer: Número de leitores: %d\n", shared_data -> readCount);
            fflush(stdout);

            shared_data -> activeWriteCount--;

            sem_post(semWriter);

            sem_wait(sem2);
            shared_data -> writeCount--;
            if(shared_data -> writeCount == 0) sem_post(semReader);
            sem_post(sem2);

            exit(0);
        }
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        waitpid(pidList[i], &status, 0);
    }

    if(sem_close(semWriter) < 0 || sem_close(semReader) < 0 || sem_close(sem1) < 0 || sem_close(sem2) < 0 || sem_close(sem3) < 0){
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