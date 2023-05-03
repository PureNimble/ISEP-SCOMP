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
#include <time.h>

#define OUTPUT_FILE "Output.txt"
#define NUMBERS_FILE "Numbers.txt"
#define NUMBER_OF_CHILDREN 8

int main(void){

    int status, num, i;
    pid_t pidList[NUMBER_OF_CHILDREN];
    time_t t;
    sem_t *sem[2];

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

    sem[0] = sem_open("numbersSema", O_CREAT | O_EXCL, 0644, 1);
    sem[1] = sem_open("outputSema", O_CREAT | O_EXCL, 0644, 1);

    if(sem[0] == SEM_FAILED || sem[1] == SEM_FAILED){
        perror("Erro no criar/abrir semaforo");
        exit(-1);
    }

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            int j, randomNum, opt = i + 1;
            srand ((unsigned) time(&t) * getpid());
            randomNum = (rand() % 5) + 1;
            sem_wait(sem[0]);
            FILE *numFile;
            numFile = fopen(NUMBERS_FILE, "r");
            if(numFile == NULL){
                perror("Erro ao procurar ficheiro");
                exit(-1);
            }
            for (j = 0; j < opt; j++) {
                fscanf(numFile, "%d", &num);
            }
            fclose(numFile);
            sem_post(sem[0]);
            sleep(randomNum);
            sem_wait(sem[1]);
            outputFile = fopen(OUTPUT_FILE, "a");
            if (outputFile == NULL) {
                perror("Erro ao abrir ficheiro Output.txt\n");
                exit(-1);
            }
            fprintf(outputFile,"[%d]%d\n" , getpid(), num);
            fclose(outputFile);
            sem_post(sem[1]);
            exit(0);
        }
    }
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

    if(sem_close(sem[0]) < 0 || sem_close(sem[1]) < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }

    if(sem_unlink("numbersSema") < 0 || sem_unlink("outputSema") < 0){
        perror("Erro ao fechar semaforo\n");
        exit(-1);
    }

    return 0;
}