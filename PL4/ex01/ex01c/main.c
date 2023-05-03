#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>

#define OUTPUT_FILE "Output.txt"
#define NUMBERS_FILE "Numbers.txt"
#define NUMBER_OF_CHILDREN 8

int main(void){

    int status, num, i;
    pid_t pidList[NUMBER_OF_CHILDREN];
    sem_t *sem[NUMBER_OF_CHILDREN];
    char semName[50];

    if(remove(OUTPUT_FILE) == 0){
        printf("O conteudo do ficheiro %s foi apagado com sucesso\n", OUTPUT_FILE);
    }else{
        perror("Erro ao apagar ficheiro Output.txt\n");
        exit(-1);
    }

    FILE *outputFile = fopen(OUTPUT_FILE, "w");

    if(outputFile == NULL){
        perror("Erro ao criar ficheiro Output.txt\n");
        exit(-1);
    }
    fclose(outputFile);

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        snprintf(semName, sizeof(semName), "numbersSema%d", i);
        sem[i] = sem_open(semName, O_CREAT | O_EXCL, 0644, 0);
        if(sem[i] == SEM_FAILED){
            perror("Erro no criar/abrir semaforo");
            exit(-1);
        }
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            sem_wait(sem[i]);
            FILE *numFile;
            numFile = fopen(NUMBERS_FILE, "r");
            if(numFile == NULL){
                perror("Erro ao procurar ficheiro");
                exit(-1);
            }
            while(fscanf(numFile, "%d", &num) != EOF);
            fclose(numFile);

            outputFile = fopen(OUTPUT_FILE, "a");
            if (outputFile == NULL) {
                perror("Erro ao abrir ficheiro Output.txt\n");
                exit(-1);
            }
            fprintf(outputFile,"[%d]%d\n" , getpid(), num);
            fclose(outputFile);
            sem_post(sem[i+1]);
            exit(0);
        }
    }
    sem_post(sem[0]);
    for (i = 0; i < NUMBER_OF_CHILDREN; i++) {
        waitpid(pidList[i], &status, 0);
    }

    outputFile = fopen(OUTPUT_FILE, "a");
    if (outputFile == NULL) {
        perror("Erro ao abrir ficheiro Output.txt\n");
        exit(-1);
    }
    while (fscanf(outputFile, "%d", &num) != EOF) {
        printf("Número lido do ficheiro Output.txt:%d\n", num);
    }
    fclose(outputFile);
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        if(sem_close(sem[i]) < 0){
            perror("Erro ao fechar semaforo\n");
            exit(-1);
        }
        snprintf(semName, sizeof(semName), "numbersSema%d", i);
        if(sem_unlink(semName) < 0){
            perror("Erro ao fechar semaforo\n");
            exit(-1);
        }
    }

    return 0;
}