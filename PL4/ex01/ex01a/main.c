#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define OUTPUT_FILE "Output.txt"
#define NUMBERS_FILE "Numbers.txt"
#define NUMBER_OF_CHILDREN 8

int main(void){

    int status, i;
    pid_t pidList[NUMBER_OF_CHILDREN];

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
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            int j, num, opt = i + 1;
            FILE *numFile;
            
            numFile = fopen(NUMBERS_FILE, "r");
            if(numFile == NULL){
                perror("Erro ao abrir ficheiro Numbers.txt\n");
                exit(-1);
            }
            for (j = 0; j < opt; j++) {
                fscanf(numFile, "%d", &num);
            }
            fclose(numFile);

            outputFile = fopen(OUTPUT_FILE, "a");
            if (outputFile == NULL) {
                perror("Erro ao abrir ficheiro Output.txt\n");
                exit(-1);
            }
            fprintf(outputFile,"[%d]%d\n" , getpid(), num);
            fclose(outputFile);
            exit(0);
            
        }
    }

    for (i = 0; i < NUMBER_OF_CHILDREN; i++) {
        waitpid(pidList[i], &status, 0);
    }

    return 0;
}