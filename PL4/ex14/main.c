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

#define SET_A 3
#define SET_B 2
#define ARRAY_SIZE 10

typedef struct {
    int array[ARRAY_SIZE];
} sharedValues;

#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx14"

int main(void){

    int fd, i;
    pid_t pid;
    time_t t;
    sem_t *semA, *semB, *semWrite;

    fd = shm_open(FILE_NAME, O_CREAT/*| O_EXCL*/|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}

    if (ftruncate (fd, DATA_SIZE) < 0) {
        perror("Erro ao alocar espaço na memória");
        exit(-1);
    }
    
    sharedValues *shared_data = (sharedValues*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    semA = sem_open("semaA", O_CREAT /*| O_EXCL*/, 0644, 1);
    semB = sem_open("semaB", O_CREAT /*| O_EXCL*/, 0644, 0);
    semWrite = sem_open("semaWrite", O_CREAT /*| O_EXCL*/, 0644, 1);

    if(semA == SEM_FAILED || semB == SEM_FAILED || semWrite == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }

    for(i = 0; i < SET_A; i++){
        pid = fork();
        if(pid < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pid == 0){
            int sval, j;
            srand ((unsigned) time(&t) * getpid());
            while(1){

                sem_wait(semA);
                sem_wait(semWrite);
                printf("A%d a escrever na zona de memoria partilhada\n", i+1);
                for (j = 0; j < ARRAY_SIZE; j++) {
                    shared_data -> array[j] = rand() % 100;
                }
                sleep(1);
                printf("A%d terminou de escrever na zona de memoria partilhada e notificou outros sets.\n", i + 1);
                sem_post(semWrite);
                sem_post(semA);
                
                sem_getvalue(semA, &sval);
                if (sval == 0) {
                    sem_post(semB);
                }

                sleep(5);
            }
        }
    }

    for(i = 0; i < SET_B; i++){
        pid = fork();
        if(pid < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pid == 0){
            int sval, j;
            srand ((unsigned) time(&t) * getpid());
            while(1){
                sem_wait(semB);
                sem_wait(semWrite);
                printf("B%d a escrever na zona de memoria partilhada\n", i+1);
                for (j = 0; j < ARRAY_SIZE; j++) {
                    shared_data -> array[j] = rand() % 100;
                }
                sleep(1);
                printf("B%d terminou de escrever na zona de memoria partilhada e notificou outros sets.\n", i + 1);
                sem_post(semWrite);

                sem_post(semB);
                
                sem_getvalue(semB, &sval);
                if (sval == 0) {
                    sem_post(semA);
                }
                
                sleep(6);
            }
        }
    }

    return 0;
}