#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define NUMBER_OF_CHILDREN 10

struct Message {
  char str[10];
  int num;
};
struct Message message;

int main(void){
  pid_t pidList[NUMBER_OF_CHILDREN];
  int fd[2], status, i;

  if(pipe(fd) == -1){
    perror("Erro no Pipe");
    return 1;
  }
  for(i = 0; i < NUMBER_OF_CHILDREN; i++){
    pidList[i] = fork();
    if(pidList[i] < 0){
      perror("Erro ao criar o processo");
      exit(-1);
    }
    if(pidList[i] == 0){
      close(fd[1]);
      read(fd[0], &message, sizeof(message));
      close(fd[0]);
      printf("Mensagem: %s Ronda: %d\n", message.str, message.num);
      exit(message.num);
    }
  }
  int winners[2][10];
  strcpy(message.str, "Win!");
  close(fd[0]);
  for(i = 0; i < NUMBER_OF_CHILDREN; i++){
    message.num = i+1;
    write(fd[1], &message, sizeof(message));
    sleep(2);
  }
  close(fd[1]);

  for (i = 0; i < NUMBER_OF_CHILDREN; i++) {
    winners[0][i] = wait(&status);
    if(WIFEXITED(status)){
      winners[1][i] = WEXITSTATUS(status);
    }
  }

  for (i = 0; i < NUMBER_OF_CHILDREN; i++) {
    printf("PID: %d ganhou a %dº ronda\n", winners[0][i], winners[1][i]);
  }

  return 0;
}