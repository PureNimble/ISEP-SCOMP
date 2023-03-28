#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

volatile sig_atomic_t USR1_counter = 0;

void handle_USR1(int signo, siginfo_t *sinfo, void *context){
  USR1_counter++;
  char msg [80];
  sprintf(msg, "SIGUSR1 signal captured: USR1 counter = %d\n", USR1_counter);
  write(STDOUT_FILENO, msg, strlen(msg));
  sleep(3);
}

int main(void){
  int i;
  pid_t childPid, parentPid;
  struct sigaction act;

  memset(&act, 0, sizeof(struct sigaction));
  sigemptyset(&act.sa_mask); 
  act.sa_sigaction = handle_USR1;
  act.sa_flags = SA_NODEFER;
  sigaction(SIGUSR1, &act, NULL);

  childPid = fork();
  if(childPid < 0){
    perror("Erro ao criar o processo");
    exit(-1);
  }else if(childPid == 0){
    parentPid = getppid();
    struct timespec sleep_time = {0, 10000000};
    for (i = 0; i < 12; i++) {
      kill(parentPid, SIGUSR1);
      nanosleep(&sleep_time, NULL);
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
/*
  e)
    output:
        ./main
        SIGUSR1 signal captured: USR1 counter = 1
        SIGUSR1 signal captured: USR1 counter = 2
        SIGUSR1 signal captured: USR1 counter = 3
        SIGUSR1 signal captured: USR1 counter = 4
        SIGUSR1 signal captured: USR1 counter = 5
        SIGUSR1 signal captured: USR1 counter = 6
        SIGUSR1 signal captured: USR1 counter = 7
        SIGUSR1 signal captured: USR1 counter = 8
        SIGUSR1 signal captured: USR1 counter = 9
        SIGUSR1 signal captured: USR1 counter = 10
        SIGUSR1 signal captured: USR1 counter = 11
        SIGUSR1 signal captured: USR1 counter = 12
        make: *** [Makefile:11: run] Interrupt

    A:
        Neste caso, uma vez que nenhum sinal é bloqueado enquanto o handler está em execução, 
        os seguintes sinais serão processados imediatamente, possivelmente de forma concorrente 
        ou recursiva, podendo mesmo interromper o tratamento de sinais anteriores. Isto pode levar 
        a uma contagem do USR1_counter incorreta em alguns casos, uma vez que o manipulador de sinais
        pode ser interrompido e retomado em momentos diferentes.
*/