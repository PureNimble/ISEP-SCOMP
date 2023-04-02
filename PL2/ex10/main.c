#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

int main(void){
  int fdDown[2], fdUp[2], randomNum, able = 1, credit = 20, status;
  pid_t pid;
  time_t t;

  if(pipe(fdDown) == -1 || pipe(fdUp) == -1){
    perror("Erro no Pipe");
    return 1;
  }

  pid = fork();
  if(pid < 0){
    perror("Erro ao criar o processo");
    exit(-1);
  }

  if(pid == 0){
    printf("Credito inicial: %d€\n", credit);
    srand ((unsigned) time(&t) * getpid());

    close(fdUp[0]);
    close(fdDown[1]);
    read(fdDown[0], &able, sizeof(able));

    while(able){
      randomNum = (rand() % 5) + 1;
      write(fdUp[1], &randomNum, sizeof(randomNum));
      read(fdDown[0], &credit, sizeof(credit));
      printf("Crédito disponivel: %d€\n", credit);
      read(fdDown[0], &able, sizeof(able));
    }
    close(fdUp[1]);
    close(fdDown[0]);
    printf("Lamentamos mas não tem mais crédito disponivel\n");
    printf("----------------------------------------------\n");
    exit(0);
  }
  int childBet;
  srand ((unsigned) time(&t) * getpid());
  printf("-------------------Betclic--------------------\n");
  close(fdUp[1]);
  close(fdDown[0]);

  while(credit > 0){
    write(fdDown[1], &able, sizeof(able));
    randomNum = (rand() % 5) + 1;
    read(fdUp[0], &childBet, sizeof(childBet));
    printf("Aposta do filho: %d\n", childBet);
    printf("Número Aleatório: %d\n", randomNum);
    if(childBet == randomNum){
      printf("Parabéns ganhou 10€\n");
      credit += 10;
    }else{
      printf("Errou, perdeu 5€\n");
      credit -= 5;
    }
    write(fdDown[1], &credit, sizeof(credit));
    printf("----------------------------------------------\n");
  }
  able = 0;
  write(fdDown[1], &able, sizeof(able));
  close(fdUp[0]);
  close(fdDown[1]);
  waitpid(pid, &status, 0);
  return 0;
}