#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define ARRAY_SIZE 1000
#define NUMBER_OF_CHILDREN 5

void fillRandomArrays(int *vec1, int *vec2) {
  int i;
  time_t t;
  srand ((unsigned) time(&t));

  for (i = 0; i < ARRAY_SIZE; i++){
    vec1[i] = rand() % 500;
    vec2[i] = rand() % 1000;
  }
}

int main(void){
  pid_t pidList[NUMBER_OF_CHILDREN];
  int fd[2], *vec1, *vec2, status, i;

  vec1 = (int *) calloc(ARRAY_SIZE, sizeof(int));
  vec2 = (int *) calloc(ARRAY_SIZE, sizeof(int));

  if (vec1 == NULL || vec2 == NULL){
    perror("Erro a criar calloc");
    return 1;
  }

  fillRandomArrays(vec1, vec2);

  if(pipe(fd) == -1){
    perror("Erro no Pipe");
    return 1;
  }
  for(i = 0; i < NUMBER_OF_CHILDREN; i++){
    pidList[i] = fork();
    if(pidList[i] < 0){
      perror("Erro ao criar o processo");
      exit(-1);
    }
    if(pidList[i] == 0){
      int j, tmp = 0;
      int min = i * (ARRAY_SIZE / NUMBER_OF_CHILDREN);
      int max = (i + 1) * (ARRAY_SIZE / NUMBER_OF_CHILDREN);
      for(j = min; j < max; j++){
        tmp += vec1[j] + vec2[j];
      }
      close(fd[0]);
      write(fd[1], &tmp, sizeof(tmp));
      close(fd[1]);
      exit(0);
    }
  }
  int readInt;
  int finalNum = 0;
  for (i = 0; i < NUMBER_OF_CHILDREN; i++) {
    waitpid(pidList[i], &status, 0);
  }

  close(fd[1]);
  for(i = 0; i < NUMBER_OF_CHILDREN; i++) {
    read(fd[0], &readInt, sizeof(readInt));
    finalNum += readInt;
  }
  close(fd[0]);

  free(vec1);
  free(vec2);
  printf("Total: %d\n", finalNum);
  return 0;
}



/*
a)

    Q: Explain why the there is no need for a synchronization mechanism? Take a look at the pipe function
       complete man pages.

    A: Não é necessário um mecanismo de sincronização, pois tal como diz no manual dos pipes, a função pipe() 
       cria um canal de dados unidirecional, que garante que os dados escritos numa extremidade do canal são 
       recebidos na mesma ordem na outra extremidade do canal. Ou seja, quando cada processo filho escreve a 
       sua soma parcial no canal, os dados são escritos de maneira sequencial e ordenada, logo oprocesso pai 
       lê os dados na mesma ordem, garantindo que recebe as somas parciais corretas de cada processo filho.


*/