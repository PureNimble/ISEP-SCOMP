#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

#define NUMBER_OF_CHILDREN 5
#define NUMBER_OF_PIPES 6

struct ProductsStruct {
  int product_code;
  float product_price;
  char product_name[12];
};

struct BarCode {
  int product_code;
  int id;
};

int main(void){
  int fd[6][2], i, status;
  pid_t pidList[NUMBER_OF_CHILDREN];
  struct BarCode request;
  for(i = 0; i < NUMBER_OF_PIPES; i++){
    if(pipe(fd[i]) == -1){
      perror("Erro no Pipe");
      return 1;
    }
  }

  for(i = 0; i < NUMBER_OF_CHILDREN; i++){
    pidList[i] = fork();
    if(pidList[i] < 0){
      perror("Erro ao criar o processo");
      exit(-1);
    }
    if(pidList[i] == 0){
      char prodName[12];
      float price;
      time_t t;
      srand ((unsigned) time(&t) * getpid());
      request.product_code = (rand() % 5) + 1;
      request.id = i;
      close(fd[5][0]);
      write(fd[5][1], &request, sizeof(request));
      close(fd[5][1]);
      printf("O %dº Barcode requesitou o produto: %d\n", i+1, request.product_code);
      close(fd[i][1]);
      read(fd[i][0], prodName, sizeof(prodName));
      read(fd[i][0], &price, sizeof(price));
      close(fd[i][0]);
      printf("O %dº Barcode recebeu o produto: %s que tem o custo de %.2f€\n", i+1, prodName, price);
      exit(0);
    }
  }
  struct ProductsStruct products[5];
  int opt;
  products[0].product_code = 1;
  products[0].product_price = 1.50;
  strcpy(products[0].product_name, "Água");

  products[1].product_code = 2;
  products[1].product_price = 2.70;
  strcpy(products[1].product_name, "Farinha");

  products[2].product_code = 3;
  products[2].product_price = 1.99;
  strcpy(products[2].product_name, "Arroz");

  products[3].product_code = 4;
  products[3].product_price = 1.99;
  strcpy(products[3].product_name, "Massa");

  products[4].product_code = 5;
  products[4].product_price = 2.80;
  strcpy(products[4].product_name, "Açucar");
  
  close(fd[5][1]);
  while(read(fd[5][0], &request, sizeof(request)) > 0){
    opt = request.id;
    for(i = 0; i < 5; i++){
      if(products[i].product_code == request.product_code){
        close(fd[opt][0]);
        write(fd[opt][1], products[i].product_name, sizeof(products[i].product_name));
        write(fd[opt][1], &products[i].product_price, sizeof(products[i].product_price));
        close(fd[opt][1]);
      }
    }
  }
  close(fd[5][0]);

  for (i = 0; i < NUMBER_OF_CHILDREN; i++) {
    waitpid(pidList[i], &status, 0);
  }
  return 0;
}
