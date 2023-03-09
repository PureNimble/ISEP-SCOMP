#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>

int spawn_childs(int n){
  int i;
  pid_t pid;
  for(i = 0; i < n; i++){
    pid = fork();
    if(pid < 0){
      perror("Erro ao criar o processo");
      return -1;
    }
    if(pid == 0){
      return i+1;
    }
  }
  return 0;
}

int main(void){
  int n = 6, status, i;
  int id = spawn_childs(n);
  if(id > 0){
    exit(id*2);
  }
  for(i = 0; i < n; i++){
    wait(&status);
    if(WIFEXITED(status)){
      printf("%d\n", WEXITSTATUS(status));
    }
  }
  return 0;
}