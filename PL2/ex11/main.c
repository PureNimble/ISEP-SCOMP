#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define NUMBER_OF_CHILDREN 5
#define NUMBER_OF_PIPES 6

int main(void){
  int fd[NUMBER_OF_PIPES][2], i, status, myNum, numberReceived;
  pid_t pidList[NUMBER_OF_CHILDREN];
  time_t t;

  for(i = 0; i < NUMBER_OF_PIPES; i++){
    if(pipe(fd[i]) == -1){
      perror("Erro no Pipe");
      return 1;
    }
  }

  for(i = 0; i < NUMBER_OF_CHILDREN; i++){
    pidList[i] = fork();
    if(pidList[i] < 0){
      perror("Erro ao criar o processo");
      exit(-1);
    }
    if(pidList[i] == 0){
      int sendTo = i + 1, numberToSend;
      srand ((unsigned) time(&t) * getpid());
      myNum = (rand() % 500) + 1;
      printf("O número gerado pelo %dº filho foi: %d\n", i+1, myNum);
      close(fd[i][1]);
      read(fd[i][0], &numberReceived, sizeof(numberReceived));
      close(fd[i][0]);
      if(myNum > numberReceived){
        numberToSend = myNum;
      }else numberToSend = numberReceived;
      close(fd[sendTo][0]);
      write(fd[sendTo][1], &numberToSend, sizeof(numberToSend));
      close(fd[sendTo][1]);
      printf("O %dº filho recebeu o número %d e enviou %d\n", i+1, numberReceived, numberToSend);
      exit(0);
    }
  }
  srand ((unsigned) time(&t) * getpid());
  myNum = (rand() % 500) + 1;
  printf("Numero do pai: %d\n", myNum);
  close(fd[0][0]);
  write(fd[0][1], &myNum, sizeof(myNum));
  close(fd[0][1]);
  for (i = 0; i < NUMBER_OF_CHILDREN; i++) {
    waitpid(pidList[i], &status, 0);
  }
  close(fd[5][1]);
  read(fd[5][0], &numberReceived, sizeof(numberReceived));
  close(fd[5][0]);
  printf("O maior número gerado foi: %d\n", numberReceived);
  return 0;
}