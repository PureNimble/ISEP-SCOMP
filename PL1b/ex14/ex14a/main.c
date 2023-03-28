#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#define Max 300

void handler(int signo, siginfo_t *sinfo, void *context){
  write(STDOUT_FILENO, "You were too slow and this program will end!\n", 45);
  exit(0);
}

int main(void){
  char input[Max];
  struct sigaction act;
  memset(&act, 0, sizeof(struct sigaction));
  sigemptyset(&act.sa_mask);
  act.sa_sigaction = handler;
  sigaction(SIGALRM, &act, NULL);
  
  printf("Escreve! Tens 10 segundos...\n");
  alarm(10);
  fgets(input, Max, stdin);
  printf("Tamanho da string é %li\n", strlen(input)-1);
  return 0;
}