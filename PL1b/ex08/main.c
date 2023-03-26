#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

void handler(int signo, siginfo_t *sinfo, void *context){
  char msg[80];
  sprintf(msg, "I captured a SIGUSR1 sent by the process with PID %d", sinfo->si_pid);
  write(STDOUT_FILENO, msg, strlen(msg));
}

int main(void){
  struct sigaction act;
  memset(&act, 0, sizeof(struct sigaction));
  sigemptyset(&act.sa_mask);
  act.sa_sigaction = handler;
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGUSR1, &act, NULL);
  for( ; ; ){
    printf("I love SCOMP ! O meu PID:%i Ataca-me\n", getpid());
    sleep(4);
  }
  return 0;
}