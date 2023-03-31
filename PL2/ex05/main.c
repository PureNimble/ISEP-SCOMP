#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>

#define BUFFER_SIZE 256

int main(void){
  pid_t pid;
  int up[2], down[2], status;
  char sendStr[BUFFER_SIZE];
  char readStr[BUFFER_SIZE];
  if(pipe(up) == -1){
    perror("Erro no Pipe UP");
    return 1;
  }
  if(pipe(down) == -1){
    perror("Erro no Pipe DOWN");
    return 1;
  }
  pid = fork();
  if(pid < 0){
    perror("Erro ao criar o processo");
    exit(-1);
  }
  if(pid == 0){
    printf("Escreva uma string: ");
    fgets(sendStr, BUFFER_SIZE, stdin);
    close(up[0]);
    write(up[1], sendStr, BUFFER_SIZE);
    close(up[1]);
    close(down[1]);
    read(down[0], readStr, BUFFER_SIZE);
    printf("%s\n", readStr);
    close(down[0]);
    exit(0);
  }
  int i;
  close(up[1]);
  read(up[0], readStr, BUFFER_SIZE);
  for (i = 0; i < strlen(readStr); i++){
    if(islower(readStr[i])){
      sendStr[i] = toupper(readStr[i]);
    }else if(readStr[i] == ' '){
      sendStr[i] = readStr[i];
    }
  }
  close(up[0]);
  close(down[0]);
  write(down[1], sendStr, BUFFER_SIZE);
  close(down[1]);

  wait(&status);

  return 0;
}