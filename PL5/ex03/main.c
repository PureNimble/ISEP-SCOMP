#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NUMBER_OF_THREADS 10
#define ARRAY_SIZE 1000

typedef struct data {
    int randomNum;
    int searchArray[ARRAY_SIZE];
} data;

typedef struct indexes {
    data* dataPtr;
    int index;
} indexes;

typedef struct {
    int idx;
} winner;

void* threadFunc(void* arg) {
    indexes* dataPoint = (indexes*) arg;
    int i, start = dataPoint -> index * 100, end = start + 100, id = dataPoint -> index + 1, search = dataPoint -> dataPtr -> randomNum;

    for (i = start; i < end; i++) {
        if (dataPoint -> dataPtr -> searchArray[i] == search) {
            winner* winnersPoint = (winner*) malloc(sizeof(winner));
            winnersPoint -> idx = id;
            printf("Thread(%d) ID: %lu encontrou o valor %d na posição %d\n", id, pthread_self(), search, i);
            pthread_exit((void*) winnersPoint);
        }
    }
    printf("Thread(%d) ID: %lu\n", id, pthread_self());
    pthread_exit(NULL);
}

int main(void) {
    int i, finalWinner;
    pthread_t threads[NUMBER_OF_THREADS];
    time_t t;
    void* ret;
    data searchData;
    indexes searchDataIndex[NUMBER_OF_THREADS];

    srand((unsigned)time(&t));

    searchData.randomNum = (rand() % ARRAY_SIZE) + 1;

    for (i = 0; i < ARRAY_SIZE; i++) {
        searchData.searchArray[i] = i + 1;
    }

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        searchDataIndex[i].dataPtr = &searchData;
        searchDataIndex[i].index = i;

        if (pthread_create(&threads[i], NULL, threadFunc, (void*) &searchDataIndex[i]) != 0) {
            perror("Erro ao criar thread");
            exit(-1);
        }
    }

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        if (pthread_join(threads[i], &ret) != 0) {
            perror("Erro ao dar join das threads");
            exit(-1);
        }

        if (ret != NULL) {
            winner* result = (winner*) ret;
            finalWinner = result -> idx;
            free(result);
        }
    }
    
    printf("Thread vencedora: %d\n", finalWinner);

    return 0;
}
