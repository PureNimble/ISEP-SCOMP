#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int main(void) {
  pid_t pid;
  int sig_num;

  printf("Insira o PID para o qual deseja enviar o sinal:");
  scanf("%d", &pid);

  printf("Insira o número do sinal:");
  scanf("%d", &sig_num);

  if (kill(pid, sig_num) == -1) {
    perror("kill");
    exit(EXIT_FAILURE);
  }

  printf("Sinal %d enviado para o processo %d\n", sig_num, pid);

  return 0;
}