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

#define LOCALMAX_SIZE 10
#define ARRAYTOSEARCH_SIZE 1000

typedef struct {
    int localMax[LOCALMAX_SIZE];
    int number[LOCALMAX_SIZE];
    bool choosing[LOCALMAX_SIZE];
} sharedValues;

#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx06"

void fillRandomArrays(int *arrayToSearch) {
    int i;
    time_t t;
    srand ((unsigned) time(&t));

    for (i = 0; i < ARRAYTOSEARCH_SIZE; i++){
        *arrayToSearch = (rand() % 1000) + 1;
        arrayToSearch++;
    }
}

void bakeryAlgorithm(int id, sharedValues *shared_data) {
    int i;
    shared_data -> choosing[id] = true;
    int max_number = 0;
    for (i = 0; i < LOCALMAX_SIZE; i++) {
        if (shared_data -> number[i] > max_number) {
            max_number = shared_data -> number[i];
        }
    }
    shared_data -> number[id] = max_number + 1;
    shared_data -> choosing[id] = false;
    for (i = 0; i < LOCALMAX_SIZE; i++) {
        if (i != id) {
            while (shared_data -> choosing[i]);
            while (shared_data -> number[i] != 0 && ((shared_data -> number[i] < shared_data -> number[id]) || (shared_data -> number[i] == shared_data -> number[id] && i < id)));
        }
    }
}

int searchLocalMax(int *arrayToSearch, int childIndex){
    int max = 0, limitSup, limitInf = 100*childIndex, i;
    limitSup = limitInf + 100;
    for(i = limitInf; i < limitSup; i++){
        if(max < arrayToSearch[i]){
            max = arrayToSearch[i];
        }
    }
    return max;
}

int searchGlobalMax(int *ptr) {
    int i, max = 0;
    for (i = 0; i < LOCALMAX_SIZE; i++){
        if(max < *ptr){
            max = *ptr;
        }
        ptr++;
    }
    return max;
}

int main(void){

    int fd, status, *arrayToSearch, max = 0, i;
    pid_t pidList[LOCALMAX_SIZE];

    arrayToSearch = (int *) calloc(ARRAYTOSEARCH_SIZE, sizeof(int));

    if (arrayToSearch == NULL){
        perror("Erro a criar calloc");
        exit(-1);
    }

    fillRandomArrays(arrayToSearch);
    
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
    
    for (i = 0; i < LOCALMAX_SIZE; i++) {
        shared_data -> choosing[i] = false;
        shared_data -> number[i] = 0;
    }

    for(i = 0; i < LOCALMAX_SIZE; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }else if(pidList[i] == 0){
            max = searchLocalMax(arrayToSearch, i);
            
            bakeryAlgorithm(i, shared_data);
            shared_data -> localMax[i] = max;
            printf("Maior número encontrado pelo %dº filho: %d\n", i + 1, max);
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
    free(arrayToSearch);
    for (i = 0; i < LOCALMAX_SIZE; i++) {
        waitpid(pidList[i], &status, 0);
    }
    max = searchGlobalMax(shared_data -> localMax);

    printf("Maior número global: %d\n", max);

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