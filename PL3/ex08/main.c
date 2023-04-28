#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>
#include <stdbool.h>

#define NUMBER_OF_CHILDREN 10
#define WORD_MAX_LENGHT 20

typedef struct {
    char filePath[NUMBER_OF_CHILDREN][36];
    char word[WORD_MAX_LENGHT];
    int occurences[NUMBER_OF_CHILDREN];
    int allowReading;
    int number[NUMBER_OF_CHILDREN];
    bool choosing[NUMBER_OF_CHILDREN];
} sharedValues;

#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx08"

void bakeryAlgorithm(int id, sharedValues *shared_data) {
    int i;
    shared_data -> choosing[id] = true;
    int max_number = 0;
    for (i = 0; i < NUMBER_OF_CHILDREN; i++) {
        if (shared_data -> number[i] > max_number) {
            max_number = shared_data -> number[i];
        }
    }
    shared_data -> number[id] = max_number + 1;
    shared_data -> choosing[id] = false;
    for (i = 0; i < NUMBER_OF_CHILDREN; i++) {
        if (i != id) {
            while (shared_data -> choosing[i]);
            while (shared_data -> number[i] != 0 && ((shared_data -> number[i] < shared_data -> number[id]) || (shared_data -> number[i] == shared_data -> number[id] && i < id)));
        }
    }
}

int main(void){

    int fd, status, i;
    pid_t pidList[NUMBER_OF_CHILDREN];
    
    fd = shm_open(FILE_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}

    if (ftruncate (fd, DATA_SIZE) < 0) {
        perror("Erro ao alocar espaço na memória");
        exit(-1);
    }
    
    sharedValues *shared_data = (sharedValues*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    shared_data -> allowReading = 0;
    for (i = 0; i < NUMBER_OF_CHILDREN; i++) {
        shared_data -> choosing[i] = false;
        shared_data -> number[i] = 0;
    }
    
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            while(!shared_data -> allowReading);
            int wordFound = 0;
            char wordToFind[WORD_MAX_LENGHT];
            char word[WORD_MAX_LENGHT];
            strcpy(wordToFind, shared_data->word);
            FILE *file;
            
            file = fopen(shared_data -> filePath[i], "r");
            if (file == NULL) {
                perror("Erro ao procurar ficheiro");
                exit(-1);
            }

            while (fscanf(file, "%s", word) == 1) {
                if (strcmp(word, wordToFind) == 0) {
                    wordFound++;
                }
            }
            
            fclose(file);
            bakeryAlgorithm(i, shared_data);
            shared_data -> occurences[i] = wordFound;
            shared_data -> number[i] = 0;

            if(munmap(shared_data, DATA_SIZE) < 0){
                perror("Erro ao remover mapping");
                exit(-1);
            }

            if(close(fd) < 0){
                perror("Erro ao fechar file descriptor");
                exit(-1);
            }
            exit(0);
        }
    }
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        sprintf(shared_data->filePath[i], "FilesToSearch/searchforchild%d.txt", i + 1);
    }
    strcpy(shared_data->word, "SCOMP");
    shared_data -> allowReading = 1;

    for (i = 0; i < NUMBER_OF_CHILDREN; i++) {
        waitpid(pidList[i], &status, 0);
    }

    for (i = 0; i < NUMBER_OF_CHILDREN; i++) {
        printf("Filho nº %d encontrou %d vezes a palavra %s\n", i + 1, shared_data -> occurences[i], shared_data -> word);
    }

    if(munmap(shared_data, DATA_SIZE) < 0){
        perror("Erro ao remover mapping");
        exit(-1);
    }

    if(close(fd) < 0){
        perror("Erro ao fechar file descriptor");
        exit(-1);
    }

    if (shm_unlink(FILE_NAME) < 0) {
        perror("Erro ao remover o Ficheiro!");
        exit(-1);
    }

    return 0;
}