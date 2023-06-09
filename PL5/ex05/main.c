#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NUMBER_OF_THREADS 3
#define ARRAY_SIZE 1000

int lowestVal = ARRAY_SIZE, highestVal = 0, averageVal;

void* searchLow(void* arg) {

    int* values = (int*) arg, i, lowValue = ARRAY_SIZE;

    printf("Thread [%lu] a procurar pelo menor número...\n", pthread_self());

    for (i = 0; i < ARRAY_SIZE; i++) {
        if (values[i] < lowValue) {
            lowValue = values[i];
        }
    }

    lowestVal = lowValue;

    printf("Menor número encontrado!\n");

    pthread_exit(NULL);
}

void* searchHigh(void* arg) {

    int* values = (int*) arg, i, highValue = 0;

    printf("Thread [%lu] a procurar pelo maior número...\n", pthread_self());

    for (i = 0; i < ARRAY_SIZE; i++) {
        if (values[i] > highValue) {
            highValue = values[i];
        }
    }

    highestVal = highValue;

    printf("Maior número encontrado!\n");

    pthread_exit(NULL);
}

void* searchAvg(void* arg) {

    int* values = (int*) arg, i, sum = 0;

    printf("Thread [%lu] a calcular a média...\n", pthread_self());

    for (i = 0; i < ARRAY_SIZE; i++) {
        sum += values[i];
    }

    averageVal = sum / ARRAY_SIZE;

    printf("Média calculada!\n");

    pthread_exit(NULL);
}

int main(void){
    
    int i, values[ARRAY_SIZE];
    time_t t;
    pthread_t lowThread, highThread, avgThread;

    srand((unsigned)time(&t));

    for (i = 0; i < ARRAY_SIZE; i++) {
        values[i] = (rand() % ARRAY_SIZE) + 1;
    }

    if (pthread_create(&lowThread, NULL, searchLow, (void*) &values) != 0) {
        perror("Erro ao criar thread");
        exit(-1);
    }

    if (pthread_create(&highThread, NULL, searchHigh, (void*) &values) != 0) {
        perror("Erro ao criar thread");
        exit(-1);
    }

    if (pthread_create(&avgThread, NULL, searchAvg, (void*) &values) != 0) {
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
    printf("Média: %d\n", averageVal);

    return 0;
}