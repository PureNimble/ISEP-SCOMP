#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

typedef struct {
    int value;
} sharedValues;

#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx05"
#define NUMBER_OF_OPERATIONS 1000000

int main(void){

    int fd, status, i;
    pid_t childPid;

    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);

    fd = shm_open(FILE_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}
    ftruncate (fd, DATA_SIZE);
    sharedValues *shared_data = (sharedValues*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    shared_data -> value = 100;

    childPid = fork();
    if(childPid < 0){
        perror("Erro ao criar o processo");
        exit(-1);
    }else if(childPid == 0){
        pid_t parentPid = getppid();
        for(i = 0; i < NUMBER_OF_OPERATIONS; i++){
            pause();
            shared_data -> value++;
            shared_data -> value--;
            kill(parentPid, SIGUSR1);
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
    for(i = 0; i < NUMBER_OF_OPERATIONS; i++){
        shared_data -> value++;
        shared_data -> value--;
        kill(childPid, SIGUSR1);
        pause();
    }

    waitpid(childPid, &status, 0);
    printf("valor final: %d\n", shared_data -> value);

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