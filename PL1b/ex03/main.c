#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>

int main(void){
  pid_t pid, pidList[3];
  int i;
  char signal[];
  for(i = 0; i < 3; i++){
    if((pidList[i] = fork()) == 0){
      for( ; ; ){
        printf("\nEu sou o %d filho", i+1);
        sleep(4);
      }
    }
  }
  printf("\nA que processo deseja enviar o sinal?");
  for(i = 0; i < 3; i++){
    printf("\nPid:%d", pidList[i]);
  }
  printf("\nPid:");
  scanf("%i", &pid);
  printf("\nSinal:");
  scanf("%s", &signal);
  kill(pid, signal);
  return 0;
}