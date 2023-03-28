#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

void checkBlocked_handler(int signo, siginfo_t *sinfo, void *context){
  int i;
  char msg [80];
  sigset_t sinalBloqueado;

  sigprocmask(SIG_BLOCK, NULL, &sinalBloqueado);
  for (i = 1; i < 32; i++) {
    if (sigismember(&sinalBloqueado, i) == 1) {
      sprintf(msg, "Sinal Bloqueado: #%d - %s\n", i, strsignal(i));
      write(STDOUT_FILENO, msg, strlen(msg));
    }
  }
}

int main(void){
  struct sigaction act;
  memset(&act, 0, sizeof(struct sigaction));
  sigfillset(&act.sa_mask);
  act.sa_sigaction = checkBlocked_handler;
  sigaction(SIGUSR1, &act, NULL);
  for( ; ; ){
    printf("Quem está bloqueado no processo: %d?\n", getpid());
    sleep(1);
  }
  return 0;
}

/*
  A: Os únicos sinais que não são possíveis de bloquear são os: SIGKILL e SIGSTOP. Ao executar o sigfillset, todos os
     sinais são bloqueados exceto estes dois.
*/