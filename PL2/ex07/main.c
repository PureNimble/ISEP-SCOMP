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

  for(i = 0; i < ARRAY_SIZE; i++){
    vec1[i] = rand() % 500;
    vec2[i] = rand() % 1000;
  }
}

int main(void){
  pid_t pidList[NUMBER_OF_CHILDREN];
  int fd[NUMBER_OF_CHILDREN][2], *vec1, *vec2, vec3[200], status, min, max, i, j;

  vec1 = (int *) calloc(ARRAY_SIZE, sizeof(int));
  vec2 = (int *) calloc(ARRAY_SIZE, sizeof(int));

  if(vec1 == NULL || vec2 == NULL){
    perror("Erro a criar calloc");
    return 1;
  }

  fillRandomArrays(vec1, vec2);
  
  for(i = 0; i < NUMBER_OF_CHILDREN; i++){
    if(pipe(fd[i]) == -1){
      perror("Erro no Pipe");
      return 1;
    }
    pidList[i] = fork();
    if(pidList[i] < 0){
      perror("Erro ao criar o processo");
      exit(-1);
    }
    if(pidList[i] == 0){
      min = i * (ARRAY_SIZE / NUMBER_OF_CHILDREN);
      max = (i + 1) * (ARRAY_SIZE / NUMBER_OF_CHILDREN);
      for(j = min; j < max; j++){
        vec3[j - min] = vec1[j] + vec2[j];
      }
      close(fd[i][0]);
      write(fd[i][1], &vec3, sizeof(vec3));
      close(fd[i][1]);
      exit(0);
    }
  }
  int *finalVec;
  finalVec = (int *) calloc(ARRAY_SIZE, sizeof(int));
  for(i = 0; i < NUMBER_OF_CHILDREN; i++){
    waitpid(pidList[i], &status, 0);
    close(fd[i][1]);
    read(fd[i][0], &vec3, sizeof(vec3));
    min = i * (ARRAY_SIZE / NUMBER_OF_CHILDREN);
    max = (i + 1) * (ARRAY_SIZE / NUMBER_OF_CHILDREN);
    for(j = min; j < max; j++){
      finalVec[j] = vec3[j - min];
    }
    close(fd[i][0]);
  }

  free(vec1);
  free(vec2);
  for(i = 0; i < ARRAY_SIZE; i++){
    printf("vec1[%d] + vec2[%d] = %d\n", i, i, finalVec[i]);
  }
  
  free(finalVec);
  return 0;
}