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
  act.sa_handler = handler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  for (int i = 1; i <= NSIG; i++) {
      sigaction(i, &act, NULL);
  }
  for( ; ; ){
    printf("I love SCOMP ! O meu PID:%i Ataca-me\n", getpid());
    sleep(2);
  }
  return 0;
}