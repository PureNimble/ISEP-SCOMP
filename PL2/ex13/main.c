#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define NUM_MACHINES 4
#define PIECES_PER_CYCLE 5
#define PIECES_PER_BATCH 10
#define PIECES_PER_BOX 100
#define NUM_BOXES 10
#define NUMBER_OF_PIECES 1000

void pidCheckError(pid_t pid){
  if(pid < 0){
    perror("Erro ao criar o processo");
    exit(-1);
  }
}

void m1(int pipe_m1m2[]){
  printf("Produção iniciada...\n");
  close(pipe_m1m2[0]);
  int cuttedPieces = 0, sendedPieces = PIECES_PER_CYCLE;
  while(cuttedPieces < NUMBER_OF_PIECES){
    cuttedPieces += PIECES_PER_CYCLE;
    write(pipe_m1m2[1], &sendedPieces, sizeof(sendedPieces));
    printf("M1 cortou as peças %d-%d e enviou-as para M2\n", cuttedPieces + 1 - PIECES_PER_CYCLE, cuttedPieces);
    usleep(1500000);
  }
  close(pipe_m1m2[1]);
  exit(0);
}

void m2(int pipe_m1m2[], int pipe_m2m3[]){
  close(pipe_m1m2[1]);
  close(pipe_m2m3[0]);
  int receivedPieces, foldedPieces = 0;
  while(foldedPieces < NUMBER_OF_PIECES){
    read(pipe_m1m2[0], &receivedPieces, sizeof(receivedPieces));
    printf("M2 recebeu as peças %d-%d de M1\n", foldedPieces + 1, foldedPieces + PIECES_PER_CYCLE);
    sleep(1);
    write(pipe_m2m3[1], &receivedPieces, sizeof(receivedPieces));
    printf("M2 dobrou as peças %d-%d e enviou-as para M3\n", foldedPieces + 1, foldedPieces + PIECES_PER_CYCLE);
    foldedPieces += receivedPieces;
  }
  close(pipe_m1m2[0]);
  close(pipe_m2m3[1]);
  exit(0);
}

void m3(int pipe_m2m3[], int pipe_m3m4[]){
  close(pipe_m2m3[1]);
  close(pipe_m3m4[0]);
  int weldedPieces = 0, receivedPieces, resetCount = 0, sendedPieces = PIECES_PER_BATCH;
  while(weldedPieces < NUMBER_OF_PIECES){
    read(pipe_m2m3[0], &receivedPieces, sizeof(receivedPieces));
    printf("M3 recebeu as peças %d-%d de M2\n", weldedPieces + 1, weldedPieces + PIECES_PER_CYCLE);
    weldedPieces += receivedPieces;
    resetCount += receivedPieces;
    if(resetCount == PIECES_PER_BATCH){
      sleep(1);
      write(pipe_m3m4[1], &sendedPieces, sizeof(sendedPieces));
      printf("M3 soldou as peças %d-%d e enviou-as para M4\n", weldedPieces - PIECES_PER_BATCH + 1, weldedPieces);
      resetCount = 0;
    }
  }
  close(pipe_m2m3[0]);
  close(pipe_m3m4[1]);
  exit(0);
}


void m4(int pipe_m3m4[], int pipe_m4Storage[]){
  close(pipe_m3m4[1]);
  close(pipe_m4Storage[0]);
  int packedPieces = 0, sendedBoxes = 0, receivedPieces, resetCount = 0;
  while(packedPieces < NUMBER_OF_PIECES){
    read(pipe_m3m4[0], &receivedPieces, sizeof(receivedPieces));
    packedPieces += receivedPieces;
    printf("M4 recebeu as peças %d-%d de M3\n", packedPieces - PIECES_PER_BATCH + 1, packedPieces);
    resetCount += receivedPieces;
    if(resetCount == PIECES_PER_BOX){
      sendedBoxes++;
      sleep(1);
      write(pipe_m4Storage[1], &sendedBoxes, sizeof(sendedBoxes));
      printf("M4 empacotou as peças %d-%d na caixa %d e enviou-as para o Armazém\n", packedPieces - PIECES_PER_BOX + 1, packedPieces, sendedBoxes);
      resetCount = 0;
    }
  }
  close(pipe_m3m4[0]);
  close(pipe_m4Storage[1]);
  exit(0);
}

int main(void){
  int pipe_m1m2[2], pipe_m2m3[2], pipe_m3m4[2], pipe_m4Storage[2], status;
  pid_t pid_m1, pid_m2, pid_m3, pid_m4;

  if(pipe(pipe_m1m2) < 0 || pipe(pipe_m2m3) < 0 || pipe(pipe_m3m4) < 0 || pipe(pipe_m4Storage) < 0){
    perror("Erro num dos Pipes");
    return 1;
  }
  pid_m1 = fork();

  pidCheckError(pid_m1);
  if(pid_m1 == 0){
    m1(pipe_m1m2);
  }
  pid_m2 = fork();

  pidCheckError(pid_m2);
  if(pid_m2 == 0){
    m2(pipe_m1m2, pipe_m2m3);
  }
  pid_m3 = fork();

  pidCheckError(pid_m3);
  if(pid_m3 == 0){
    m3(pipe_m2m3, pipe_m3m4);
  }
  pid_m4 = fork();

  pidCheckError(pid_m4);
  if(pid_m4 == 0){
    m4(pipe_m3m4, pipe_m4Storage);
  }
  int i, boxes, storageA1 = 0;

  close(pipe_m4Storage[1]);
  for(i = 0; i < NUM_BOXES; i++){
    read(pipe_m4Storage[0], &boxes, sizeof(boxes));
    storageA1++;
    printf("Armazém recebeu um pacote, caixas no inventário disponíveis: %d\n", storageA1);
  }
  close(pipe_m4Storage[0]);
  for(i = 0; i < NUM_MACHINES; i++){
    wait(&status);
  }
  printf("Todas as peças foram finalizadas\n");

  return 0;
}