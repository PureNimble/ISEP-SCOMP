#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

volatile sig_atomic_t USR1_counter = 0;

void handle_USR1(int signo, siginfo_t *sinfo, void *context){
  USR1_counter++;
  printf("SIGUSR1 signal captured: USR1 counter = %d\n", USR1_counter);
}

int main(void){
  int i;
  pid_t childPid, parentPid;
  struct sigaction act;

  memset(&act, 0, sizeof(struct sigaction));
  sigfillset(&act.sa_mask); 
  act.sa_sigaction = handle_USR1;
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGUSR1, &act, NULL);

  childPid = fork();
  if(childPid < 0){
    perror("Erro ao criar o processo");
    exit(-1);
  }else if(childPid == 0){
    parentPid = getppid();
    for (i = 0; i < 12; i++) {
      kill(parentPid, SIGUSR1);
      usleep(10000);
    }
    kill(parentPid, SIGINT);
    exit(0);
  }
  for( ; ; ){
    printf("Im working!\n");
    sleep(1);
  }
  return 0;
}