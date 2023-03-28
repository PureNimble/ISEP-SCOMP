#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

volatile sig_atomic_t process = 2;

void handler(int signo, siginfo_t *sinfo, void *context){
  process--;
}

int main(void){

  int status;
  pid_t pid;

  pid = fork();
  if(pid < 0){
    perror("Erro ao criar o processo");
    exit(-1);
  }
  if(pid == 0){
    int n;                /* the number to find */
    time_t t;            /* needed to initialize random number generator (RNG) */
    /* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand ((unsigned) time(&t));

    /* initialize n */
    n = rand() % 5 + 1;

    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handler;
    sigaction(SIGUSR1, &act, NULL);

    printf("Processo B em execução...(%ds)\n", n);
    sleep(n);
    printf("Task B: done!\n");
    process--;
    while(process > 0);
    printf("Processo C em execução...(1s)\n");
    sleep(1);
    printf("Task C: done!\n");
    exit(0);
  }
  printf("Processo A em execução...(3s)\n");
  sleep(3);
  printf("Task A: done!\n");
  kill(pid, SIGUSR1);
  while (waitpid(pid, &status, 0) == -1);
  printf("Job is complete!\n");

  return 0;
}