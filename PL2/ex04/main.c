#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 80

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
    exit (-1);
  }
  if(pid == 0){
    char readStr[BUFFER_SIZE];
    int n;
    close(fd[1]);
    while ((n = read(fd[0], readStr, BUFFER_SIZE))){
      printf("%s", readStr);
    }
    printf("\n");
    close(fd[0]);
    exit(0);
  }
  FILE *exFile;
  char filename[] = "example.txt";
  char sendStr[BUFFER_SIZE];

  exFile = fopen(filename, "r");
  if (exFile == NULL) {
    printf("Failed to open the file.\n");
    return 1;
  }
  close(fd[0]);
  while (fgets(sendStr, sizeof(sendStr), exFile)) {
    write(fd[1], sendStr, BUFFER_SIZE);
  }
  fclose(exFile);
  close(fd[1]);
  wait(&status);

  return 0;
}