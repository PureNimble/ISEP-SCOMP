#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NUMBER_OF_THREADS 3
#define ARRAY_SIZE 1000

typedef struct data {
    int values[ARRAY_SIZE];
} data;

int lowestVal = ARRAY_SIZE, highestVal = 0, averageVal;

void* searchLow(void* arg) {

    data* dataPoint = (data*) arg;
    int i;

    printf("Thread [%lu] a procurar pelo menor número...\n", pthread_self());

    for (i = 0; i < ARRAY_SIZE; i++) {
        if (dataPoint -> values[i] < lowestVal) {
            lowestVal = dataPoint -> values[i];
        }
    }

    printf("Menor número encontrado!\n");

    pthread_exit(NULL);
}

void* searchHigh(void* arg) {

    data* dataPoint = (data*) arg;
    int i;

    printf("Thread [%lu] a procurar pelo maior número...\n", pthread_self());

    for (i = 0; i < ARRAY_SIZE; i++) {
        if (dataPoint -> values[i] > highestVal) {
            highestVal = dataPoint -> values[i];
        }
    }

    printf("Maior número encontrado!\n");

    pthread_exit(NULL);
}

void* searchAvg(void* arg) {

    data* dataPoint = (data*) arg;
    int i, sum = 0;

    printf("Thread [%lu] a calcular a média...\n", pthread_self());

    for (i = 0; i < ARRAY_SIZE; i++) {
        sum += dataPoint -> values[i];
    }

    averageVal = sum / ARRAY_SIZE;

    printf("Média calculada!\n");

    pthread_exit(NULL);
}

int main(void){
    
    int i;
    time_t t;
    pthread_t lowThread, highThread, avgThread;

    data searchData;

    srand((unsigned)time(&t));

    for (i = 0; i < ARRAY_SIZE; i++) {
        searchData.values[i] = (rand() % ARRAY_SIZE) + 1;
    }

    if (pthread_create(&lowThread, NULL, searchLow, (void*) &searchData) != 0) {
        perror("Erro ao criar thread");
        exit(-1);
    }

    if (pthread_create(&highThread, NULL, searchHigh, (void*) &searchData) != 0) {
        perror("Erro ao criar thread");
        exit(-1);
    }

    if (pthread_create(&avgThread, NULL, searchAvg, (void*) &searchData) != 0) {
        perror("Erro ao criar thread");
        exit(-1);
    }

    if (pthread_join(lowThread, NULL) != 0) {
        perror("Erro ao dar join das threads");
        exit(-1);
    }

    if (pthread_join(highThread, NULL) != 0) {
        perror("Erro ao dar join das threads");
        exit(-1);
    }

    if (pthread_join(avgThread, NULL) != 0) {
        perror("Erro ao dar join das threads");
        exit(-1);
    }

    printf("Menor número: %d\n", lowestVal);
    printf("Maior número: %d\n", highestVal);
    printf("Maior número: %d\n", averageVal);

    return 0;
}