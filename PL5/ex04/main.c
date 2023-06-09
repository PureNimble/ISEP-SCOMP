#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define FILL_THREADS 2
#define MULT_THREADS 8
#define DIMENTIONS 8

int matrix1[DIMENTIONS][DIMENTIONS];
int matrix2[DIMENTIONS][DIMENTIONS];
int result[DIMENTIONS][DIMENTIONS];
pthread_mutex_t mutex;

void* fillMatrix(void* arg) {

    int (*matrix)[DIMENTIONS] = (int (*)[DIMENTIONS]) arg, i, j;
    time_t t;

    printf("Thread [%lu] a preencher a matriz...\n", pthread_self());
    srand((unsigned)time(&t));

    for (i = 0; i < DIMENTIONS; i++) {
        for (j = 0; j < DIMENTIONS; j++) {
            matrix[i][j] = (rand() % 70) + 1;
        }
    }

    printf("Matriz preenchida!\n");

    pthread_exit(NULL);
}

void* multMatrix(void *arg) {

    int line = *((int*) arg), i, j, k;

    printf("Thread(%d) ID: %lu a multiplicar as matrizes...\n", line + 1, pthread_self());

    for (i = line; i < DIMENTIONS; i++) {
        for (j = 0; j < DIMENTIONS; j++) {
            for (k = 0; k < DIMENTIONS; k++) {
                pthread_mutex_lock(&mutex);
                result[i][j] += matrix1[i][k] * matrix2[k][j];
                pthread_mutex_unlock(&mutex);
            }
        }
    }

    pthread_exit(NULL);
}

int main(void){
    
    int i, j, index[MULT_THREADS];
    pthread_t fillThreads[FILL_THREADS], multThreads[MULT_THREADS];

    if (pthread_create(&fillThreads[0], NULL, fillMatrix, (void*) &matrix1) != 0) {
        perror("Erro ao criar thread");
        exit(-1);
    }

    if (pthread_create(&fillThreads[1], NULL, fillMatrix, (void*) &matrix2) != 0) {
        perror("Erro ao criar thread");
        exit(-1);
    }

    for (i = 0; i < FILL_THREADS; i++) {

        if (pthread_join(fillThreads[i], NULL) != 0) {
            perror("Erro ao dar join das threads");
            exit(-1);
        }
    }

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Erro ao criar mutex");
        exit(-1);
    }

    for (i = 0; i < MULT_THREADS; i++) {
        index[i] = i;
        if (pthread_create(&multThreads[i], NULL, multMatrix, (void*) &index[i]) != 0) {
            perror("Erro ao criar thread");
            exit(-1);
        }
    }

    for (i = 0; i < MULT_THREADS; i++) {

        if (pthread_join(multThreads[i], NULL) != 0) {
            perror("Erro ao dar join das threads");
            exit(-1);
        }
    }

    pthread_mutex_destroy(&mutex);

    printf("\nMatriz final:\n\n");
    printf("       0       1       2       3       4       5       6       7\n\n");

    for (i = 0; i < DIMENTIONS; i++) {
        printf("%d   ", i);
        for (j = 0; j < DIMENTIONS; j++) {
            printf("[%d] ", result[i][j]);
        }
        printf("\n");
    }

    return 0;
}