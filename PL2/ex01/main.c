#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 80

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
        char read_msg[BUFFER_SIZE];
        close(fd[1]);
        read(fd[0], read_msg, BUFFER_SIZE);
        printf("Filho leu: %s", read_msg);
        close(fd[0]);
        exit(0);
    }
    pid_t parentPid = getpid();
    char write_msg[BUFFER_SIZE];
    sprintf(write_msg, "Se me estiveres a ouvir o meu pid é: %d\n", parentPid);
    printf("PID: %d\n", parentPid);
    close(fd[0]);
    write(fd[1],write_msg,strlen(write_msg)+1);
    close(fd[1]);
    return 0;
}