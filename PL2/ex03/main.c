#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 20

int main(void){
  pid_t pid;
  int fd[2];
  int status;
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
    char readStr[BUFFER_SIZE];
    char readStr2[BUFFER_SIZE];
    close(fd[1]);
    read(fd[0], readStr, BUFFER_SIZE);
    printf("%s\n", readStr);
    read(fd[0], readStr2, BUFFER_SIZE);
    printf("%s\n", readStr2);
    close(fd[0]);
    exit(0);
  }
  char sendStr[BUFFER_SIZE] = "Hello World";
  char sendStr2[BUFFER_SIZE] = "Goodbye";
  close(fd[0]);
  write(fd[1], sendStr, BUFFER_SIZE);
  write(fd[1], sendStr2, BUFFER_SIZE);
  close(fd[1]);
  int sonPid = wait(&status);
  printf("Pid do filho:%d\n", sonPid);
  if(WIFEXITED(status)){
    printf("Exit value = %d\n", WEXITSTATUS(status));
  }

  return 0;
}