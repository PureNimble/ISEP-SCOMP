#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NUMBER_OF_THREADS 5
#define ARRAY_SIZE 1000

typedef struct sharedData {
    int data[ARRAY_SIZE];
    int results[ARRAY_SIZE];
} sharedData;

typedef struct indexes {
    sharedData* dataPtr;
    int index;
} indexes;

pthread_mutex_t mutex;

void* calcFunc(void *arg) {

    indexes* values = (indexes*) arg;
    int i, result, id = values -> index, start = id * 500, end = start + 500, keyIndex;

    printf("Thread(%d) ID: %lu em execução...\n", id + 1, pthread_self());

    for (i = start; i < end; i++) {
        values -> dataPtr -> results[i] = values -> dataPtr -> data[i] * 10 + 2;
    }

    pthread_exit(NULL);
}

int main(void){
    
    int i, j;
    time_t t;
    pthread_t threads[NUMBER_OF_THREADS];

    sharedData calcData;
    indexes searchDataIndex[NUMBER_OF_THREADS];

    srand((unsigned)time(&t));

    for (i = 0; i < ARRAY_SIZE; i++) {
        calcData.data[i][j] = (rand() % 1000) + 1;
    }

    for (i = 0; i < NUMBER_OF_NUMBERS; i++) {
        if (pthread_mutex_init(&mutex[i], NULL) != 0) {
            perror("Erro ao criar mutex");
            exit(-1);
        }
    }

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        searchDataIndex[i].dataPtr = &calcData;
        searchDataIndex[i].index = i;
        if (pthread_create(&threads[i], NULL, calcFunc, (void*) &searchDataIndex[i]) != 0) {
            perror("Erro ao criar thread");
            exit(-1);
        }
    }

    for (i = 0; i < NUMBER_OF_THREADS; i++) {

        if (pthread_join(threads[i], NULL) != 0) {
            perror("Erro ao dar join das threads");
            exit(-1);
        }
    }

    for (i = 0; i < NUMBER_OF_NUMBERS; i++) {
        pthread_mutex_destroy(&mutex[i]);
    }

    printf("Fim de todas as execuções!\n");
    printf("Estatísticas:\nN | Número de aparições\n");
    for (i = 0; i < NUMBER_OF_NUMBERS; i++) {
        printf("%d - %d\n", i + 1, searchData.results[i]);
    }

    return 0;
}