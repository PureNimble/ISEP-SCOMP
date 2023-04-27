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
} sharedValues;

#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx10"
#define NUMBER_OF_OPERATIONS 30

int main(void){

    int fd, sync[2], status, allow = 1, i;
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

    if(pipe(sync) < 0){
        perror("Erro no Pipe");
        exit(-1);
    }

    pid = fork();
    if(pid < 0){
        perror("Erro ao criar o processo");
        exit(-1);
    }else if(pid == 0){
        int readIndex = 0;
        close(sync[0]);
        write(sync[1], &allow, sizeof(allow));
        close(sync[1]);
        for(i = 0; i < NUMBER_OF_OPERATIONS; i++){
            close(sync[1]);
            while(!read(sync[0], &allow, sizeof(allow)));
            close(sync[0]);
            if(readIndex >= BUFFER_SIZE){
                readIndex = 0;
            }
            printf("O valor lido pelo processo filho foi: %d\n", shared_data -> buffer[readIndex]);
            close(sync[0]);
            write(sync[1], &allow, sizeof(allow));
            close(sync[1]);
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
        close(sync[1]);
        while(!read(sync[0], &allow, sizeof(allow)));
        close(sync[0]);
        if(writeIndex >= BUFFER_SIZE){
            writeIndex = 0;
        }
        shared_data -> buffer[writeIndex] = i+1;
        close(sync[0]);
        write(sync[1], &allow, sizeof(allow));
        close(sync[1]);
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