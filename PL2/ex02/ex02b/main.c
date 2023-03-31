#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

struct Message {
    int num;
    char str[50];
};
struct Message message;

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
        close(fd[1]);
        read(fd[0], &message, sizeof(message));
        printf("Filho leu: %i & %s\n", message.num, message.str);
        close(fd[0]);
        exit(0);
    }
    printf("Escreva um número: ");
    scanf("%d", &message.num);
    printf("Escreva uma string: ");
    scanf("%s", message.str);

    close(fd[0]);
    write(fd[1], &message, sizeof(message));
    close(fd[1]);

    return 0;
}