#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFFER_SIZE 10

typedef struct {
    int buffer[BUFFER_SIZE];
    volatile int counter;
} sharedValues;

#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx09"
#define NUMBER_OF_OPERATIONS 30

int main(void){

    int fd, status, i;
    pid_t pid;
    
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
    shared_data -> counter = 0;
    
    pid = fork();
    if(pid < 0){
        perror("Erro ao criar o processo");
        exit(-1);
    }else if(pid == 0){
        int nextConsumed, readIndex = 0;
        for(i = 0; i < NUMBER_OF_OPERATIONS; i++){
            while (shared_data -> counter == 0);
            nextConsumed = shared_data -> buffer[readIndex];
            readIndex = (readIndex + 1) % BUFFER_SIZE;
            shared_data -> counter--;
            printf("O valor lido pelo processo filho foi: %d\n", nextConsumed);
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
    int writeIndex = 0;
    for(i = 0; i < NUMBER_OF_OPERATIONS; i++){
        while (shared_data -> counter == BUFFER_SIZE);
        shared_data -> buffer[writeIndex] = i + 1;
        writeIndex = (writeIndex + 1) % BUFFER_SIZE;
        shared_data -> counter++;
    }

    waitpid(pid, &status, 0);

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