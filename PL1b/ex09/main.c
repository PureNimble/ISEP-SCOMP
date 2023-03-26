#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

void handler(int signo, siginfo_t *sinfo, void *context){
  if(signo == SIGINT){
    write(STDOUT_FILENO, "I won't let the process end with CTRL-C!\n", 41);
  }
  else if(signo == SIGQUIT){
    write(STDOUT_FILENO, "I won't let the process end with CTRL-\\!\n", 42);
  }
}

int main(void){
  struct sigaction act;
  memset(&act, 0, sizeof(struct sigaction));
  sigemptyset(&act.sa_mask);
  act.sa_sigaction = handler;
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGQUIT, &act, NULL);
  for( ; ; ){
    printf("I like signals\n");
    sleep(1);
  }
  return 0;
}

/*
  a)
  output:
  intr = ^C; quit = ^\; susp = ^Z;

  b)
  output:
  make: *** [Makefile:11: run] Interrupt
  
  Tal como vimos na pergunta anterior, quando pressionamos CTRL+C 
  no terminal do programa para o qual pretendemos enviar o sinal, 
  estamos a enviar o sinal de interrupção (SIGINT), que é responsável 
  pelo envio de um sinal de interrupção para o processo em execução, 
  solicitando que ele termine a execução imediatamente.

  c)✅

  d)
  output:
  make: *** [Makefile:11: run] Quit

  Quando pressionamos CTRL+\ no terminal do programa para o qual pretendemos 
  enviar o sinal, estamos a enviar o sinal de encerramento (SIGQUIT), 
  que é responsável pelo envio de um sinal para o processo em execução, 
  solicitando que ele encerre a execução imediatamente.

  e)✅

  f)
    input: kill -SIGINT process(pid)
    output: I won't let the process end with CTRL-C!
  
  g)
    input: kill -SIGINT process(pid)
    output: I won't let the process end with CTRL-\!

  h)
    i.
        output:
        [1]+  Stopped                 make run

    ii.
        output:
        F S   UID   PID  PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
        0 S  1000    32    31  0  80   0 -  3562 -      tty1     00:00:00 bash
        0 T  1000    43    32  0  80   0 -  2903 -      tty1     00:00:00 make
        0 T  1000    50    43  0  80   0 -  2703 -      tty1     00:00:00 main
        0 R  1000    51    32  0  80   0 -  3880 -      tty1     00:00:00 ps

        Processo PID 50 parado("T")

    iii.
        output:
        [1]+  Stopped                 make run

        Número: 1

    iv.
        output:
        make: *** [Makefile:11: run] Terminated

        O %1 refere-se ao ID do primeiro "job" que foi suspenso,
        neste caso o processo 50, sendo assim, o comando kill %1
        irá terminar esse "job".
*/