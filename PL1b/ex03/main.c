#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

void handler(int signum) {
  printf("Sinal recebido");
}

int main(void){
  struct sigaction act;
  memset(&act, 0, sizeof(struct sigaction));
  sigemptyset(&act.sa_mask);
  act.sa_sigaction = handler;
  sigaction(SIGUSR1, &act, NULL);
  for( ; ; ){
    printf("I love SCOMP ! O meu PID:%i Ataca-me\n", getpid());
    sleep(2);
  }
  return 0;
}