#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>

#define ARRAY_SIZE 1000
int main(void){
  int numbers[ARRAY_SIZE];    /* array to lookup */
  int result[ARRAY_SIZE];
  int halfSize = ARRAY_SIZE/2;
  int i, j, min = -200, max = 0, nMax = -1, status;            /* the number to find */
  time_t t;            /* needed to initialize random number generator (RNG) */
  pid_t pid;

  /* intializes RNG (srand():stdlib.h; time(): time.h) */
  srand ((unsigned) time(&t));

  /* initialize array with random numbers (rand(): stdlib.h) */
  for (i = 0; i < ARRAY_SIZE; i++)
    numbers[i] = rand() % 256;

  for(i = 0; i < 5; i++){
    min += 200;
    max += 200;

    pid = fork();
    if(pid < 0){
      perror("Erro ao criar o processo");
      exit(-1);
    }
    if(pid == 0){
      for(j = min;j < max; j++){
        if(numbers[j] > nMax){
          nMax = numbers[j];
        }
      }
      exit(nMax);
    }
  }
  for(i = 0; i < 5; i++){
    wait(&status);
    if(WIFEXITED(status)){
      printf("Número máximo do %dº filho = %d\n", i+1, WEXITSTATUS(status));
      if(WEXITSTATUS(status) > nMax){
        nMax = WEXITSTATUS(status);
      }
    }
  }
  printf("Maior número = %d\n", nMax);
  pid = fork();
  if(pid < 0){
    perror("Erro ao criar o processo");
    exit(-1);
  }
  if(pid == 0){
    for(i = 0; i < halfSize; i++){
      result[i] = ((int) numbers[i]/nMax)*100;
      printf("result[%d] = %d\n", i, result[i]);
    }
    exit(0);
  }
  for(i = halfSize; i < ARRAY_SIZE; i++){
    result[i] = ((int) numbers[i]/nMax)*100;
  }
  waitpid(pid, NULL, 0);
  for(i = halfSize; i < ARRAY_SIZE; i++){
    printf("result[%d] = %d\n", i, result[i]);
  }
  return 0;
}