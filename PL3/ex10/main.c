#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define BUFFER_SIZE 10
#define MAX_VALUES 30

int main(void){
    int fd[2];
    int i, j, readValue;
    int buffer[BUFFER_SIZE];
    pid_t pid;

    if(pipe(fd) < 0){
        perror("Erro no pipe");
        exit(-1);
    }

    pid = fork();
    if(pid < 0){
        perror("Erro ao criar o processo");
        exit(-1);
    }else if(pid == 0){
        close(fd[0]);
        for(j = 0; j < MAX_VALUES; j++){
            buffer[j % BUFFER_SIZE] = j + 1;

            write(fd[1], &buffer[j % BUFFER_SIZE], sizeof(int));
        }
        close(fd[1]);

        exit(0);
    }

    close(fd[1]);
    for(i = 0; i < MAX_VALUES; i++){
        read(fd[0], &readValue, sizeof(int));

        printf("Valor lido: %d\n", readValue);
    }
    close(fd[0]);
    
    return 0;
}