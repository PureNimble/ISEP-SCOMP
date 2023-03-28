#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

#define NUMBER_OF_CHILDREN 5
volatile sig_atomic_t childCounter = 5;

void handler(int signo, siginfo_t *sinfo, void *context){
  childCounter--;
}

int main(void){
  int i, j, nMax, nMin, status, parentPid = getpid();
  pid_t pidList[NUMBER_OF_CHILDREN];

  struct sigaction act;
  memset(&act, 0, sizeof(struct sigaction));
  sigemptyset(&act.sa_mask);
  act.sa_sigaction = handler;
  act.sa_flags = SA_NOCLDWAIT | SA_NOCLDSTOP;
  sigaction(SIGUSR1, &act, NULL);

  for(i = 0; i < NUMBER_OF_CHILDREN; i++){
    pidList[i] = fork();
    if(pidList[i] < 0){
      perror("Erro ao criar o processo");
      exit(-1);
    }
    if(pidList[i] == 0){
      nMin = i*200;
      nMax = (i+5)*200;
      for(j = nMin; j < nMax; j++){
        printf("%d\n", j);
      }
      kill(parentPid, SIGUSR1);
      exit(0);
    }
  }

  while(childCounter > 0){
    pause();
    printf("Em pausa...\n");
  }

  for(i = 0; i < NUMBER_OF_CHILDREN; i++){
    wait(&status);
  }

  return 0;
}