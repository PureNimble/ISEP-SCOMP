#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NUMBER_OF_THREADS 5
#define ARRAY_SIZE 1000

int data[ARRAY_SIZE];
int results[ARRAY_SIZE];
int counter = 0, turn = 0;

pthread_mutex_t mutex;
pthread_cond_t cond;
pthread_cond_t condPrinter[NUMBER_OF_THREADS];

void* calcFunc(void *arg) {

    int id = *((int*) arg), i, start = id * 200, end = start + 200;

    printf("Thread(%d) ID: %lu em execução...\n", id + 1, pthread_self());

    for (i = start; i < end; i++) {
        results[i] = data[i] * 10 + 2;
    }

    printf("Thread(%d) ID: %lu terminou os calculos!\n", id + 1, pthread_self());

    pthread_mutex_lock(&mutex);
    counter++;
    while (counter < NUMBER_OF_THREADS) {
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    pthread_mutex_lock(&mutex);
    while (id != turn) {
        pthread_cond_wait(&condPrinter[id], &mutex);
    }

    printf("\nThread(%d) ID: %lu [%d - %d]\n", id + 1, pthread_self(), start, end - 1);
    for (i = start; i < end; i++) {
        printf("%d: %d\n", i, results[i]);
        fflush(stdout);
    }

    turn++;
    pthread_cond_signal(&condPrinter[id + 1]);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main(void){
    
    int i, index[NUMBER_OF_THREADS];
    time_t t;
    pthread_t threads[NUMBER_OF_THREADS];

    srand((unsigned)time(&t));

    for (i = 0; i < ARRAY_SIZE; i++) {
        data[i] = (rand() % ARRAY_SIZE) + 1;
    }

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Erro ao criar mutex");
        exit(-1);
    }

    if (pthread_cond_init(&cond, NULL) != 0) {
        perror("Erro ao criar variavel condicional");
        exit(-1);
    }

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        if (pthread_cond_init(&condPrinter[i], NULL) != 0) {
            perror("Erro ao criar variavel condicional");
            exit(-1);
        }
    }
    

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        index[i] = i;
        if (pthread_create(&threads[i], NULL, calcFunc, (void*) &index[i]) != 0) {
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

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        pthread_cond_destroy(&condPrinter[i]);
    }
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    return 0;
}