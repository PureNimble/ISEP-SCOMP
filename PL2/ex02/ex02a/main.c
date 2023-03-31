#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 50

int main(void){
    pid_t pid;
    int fd[2];
    if(pipe(fd) == -1){
        perror("Erro no Pipe");
        return 1;
    }
    pid = fork();
    if(pid < 0){
      perror("Erro ao criar o processo");
      exit(-1);
    }
    if(pid == 0){
        int readNum;
        char readStr[BUFFER_SIZE];
        close(fd[1]);
        read(fd[0], &readNum, sizeof(int));
        read(fd[0], readStr, BUFFER_SIZE);
        printf("Filho leu: %i & %s\n", readNum, readStr);
        close(fd[0]);
        exit(0);
    }
    int sendNum;
    char sendStr[BUFFER_SIZE];
    printf("Escreva um número: ");
    scanf("%d", &sendNum);
    printf("Escreva uma string: ");
    scanf("%s", sendStr);

    close(fd[0]);
    write(fd[1], &sendNum, sizeof(int));
    write(fd[1], sendStr, BUFFER_SIZE);
    close(fd[1]);

    return 0;
}