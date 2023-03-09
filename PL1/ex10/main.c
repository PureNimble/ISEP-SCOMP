#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>

#define ARRAY_SIZE 2000
int main(void){
  int numbers[ARRAY_SIZE];    /* array to lookup */
  int i, j, n, status;            /* the number to find */
  time_t t;            /* needed to initialize random number generator (RNG) */
  pid_t pid;
  int min = -200;
  int max = 0;

  /* intializes RNG (srand():stdlib.h; time(): time.h) */
  srand ((unsigned) time(&t));

  /* initialize array with random numbers (rand(): stdlib.h) */
  for (i = 0; i < ARRAY_SIZE; i++)
    numbers[i] = rand() % 1000;

  /* initialize n */
  n = rand() % 1000;
  printf("Número a procurar %d\n", n);

  for(i = 0; i < 10; i++){
    min += 200;
    max += 200;
    pid = fork();

    if(pid < 0){
      perror("Erro ao criar o processo");
      exit(-1);
    }
    if(pid == 0){
      for(j = min;j < max; j++){
        if(numbers[j] == n){
            exit(j - min);
        }
      }
      exit(255);
    }
  }
  for(i = 0; i < 10; i++){
    wait(&status);
    if(WIFEXITED(status)){
      printf("Index = %d\n", WEXITSTATUS(status));
    }
  }
  return 0;
}