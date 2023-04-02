#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define ARRAY_SIZE 50000
#define NUMBER_OF_CHILDREN 10

typedef struct{
  int customer_code;
  int product_code;
  int quantity;
} SalesStruct;

void fillRandomStruct(SalesStruct *sales){
  int i;
  time_t t;
  srand ((unsigned) time(&t));

  for (i = 0; i < ARRAY_SIZE; i++){
    sales[i].customer_code = rand() % 50000;
    sales[i].product_code = rand() % 25000;
    sales[i].quantity = rand() % 25;
  }
}

int main(void){
  pid_t pidList[NUMBER_OF_CHILDREN];
  int fd[NUMBER_OF_CHILDREN][2], status, i;

  SalesStruct *sales = (SalesStruct *)calloc(ARRAY_SIZE, sizeof(SalesStruct));

  fillRandomStruct(sales);

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
      int j;
      int min = i * (ARRAY_SIZE / NUMBER_OF_CHILDREN);
      int max = (i + 1) * (ARRAY_SIZE / NUMBER_OF_CHILDREN);
      close(fd[i][0]);
      for(j = min; j < max; j ++){
        if(sales[j].quantity > 20){
          write(fd[i][1], &sales[j].product_code, sizeof(sales[j].product_code));
        }
      }
      close(fd[i][1]);
      exit(0);
    }
  }
  int *products, *checkTmp, counter = 0;
  products = (int *) calloc(ARRAY_SIZE, sizeof(int));
  if (products == NULL){
    perror("Erro a criar malloc");
    return 1;
  }
  for(i = 0; i < NUMBER_OF_CHILDREN; i++){
    wait(&status);
  }

  for(i = 0; i < NUMBER_OF_CHILDREN; i++) {
    close(fd[i][1]);
    while(read(fd[i][0], &products[counter], sizeof(products[counter])) > 0){
      counter++;
    }
    close(fd[i][0]);
  }
  checkTmp =( int *) realloc(products ,counter * sizeof(int));
  if(checkTmp != NULL){
    products = checkTmp;
    checkTmp = NULL;
  }
  free(sales);
  for(i = 0; i < counter; i++){
    printf("Codigo do produto: #%d\n", products[i]);
  }
  free(products);

  return 0;
}