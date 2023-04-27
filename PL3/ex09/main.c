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
    int allowFather;
    int allowSon;
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

    shared_data -> allowFather = 1;
    shared_data -> allowSon = 0;

    pid = fork();
    if(pid < 0){
        perror("Erro ao criar o processo");
        exit(-1);
    }else if(pid == 0){
        int read = 0;
        for(i = 0; i < NUMBER_OF_OPERATIONS; i++){
            while(!shared_data -> allowSon);
            shared_data -> allowSon = 0;
            if(read >= BUFFER_SIZE){
                read = 0;
            }
            printf("O valor lido pelo processo filho foi: %d\n", shared_data -> buffer[read]);
            shared_data -> allowFather = 1;
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
    int write = 0;
    for(i = 0; i < NUMBER_OF_OPERATIONS; i++){
        while(!shared_data -> allowFather);
        shared_data -> allowFather = 0;
        if(write >= BUFFER_SIZE){
            write = 0;
        }
        shared_data -> buffer[write] = i+1;
        shared_data -> allowSon = 1;
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