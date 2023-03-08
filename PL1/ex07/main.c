#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>

#define ARRAY_SIZE 1000
int main(){
  int numbers[ARRAY_SIZE];    /* array to lookup */
  int i, n, status, total;            /* the number to find */
  time_t t;            /* needed to initialize random number generator (RNG) */
  int halfSize = ARRAY_SIZE/2;
  pid_t pid;
  int counter = 0;

  /* intializes RNG (srand():stdlib.h; time(): time.h) */
  srand ((unsigned) time(&t));

  /* initialize array with random numbers (rand(): stdlib.h) */
  for (i = 0; i < ARRAY_SIZE; i++)
    numbers[i] = rand() % 10000;

  /* initialize n */
  n = rand() % 10000;

  pid = fork();

  if (pid < 0){
    perror("Erro ao criar o processo:");
    exit(-1);
  }
  else if(pid == 0){
    for(i = 0; i < halfSize; i++){
      if(numbers[i] == n){
        counter++;
      }
    }
    printf("'%d'foi encontrado %dx no processo filho\n", n, counter);
    exit(counter);
  }
  for (i = halfSize; i < ARRAY_SIZE; i++){
    if(numbers[i] == n){
      counter++;
    }
  }
  waitpid(pid, &status, 0);
  printf("'%d'foi encontrado %dx no processo pai\n", n, counter);
  if(WIFEXITED(status)){
    total = WEXITSTATUS(status) + counter;
    printf("Total = %d\n", total);
  }
  return 0;
}