#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NUMBER_OF_THREADS 16
#define NUMBER_OF_NUMBERS 49
#define KEY 5
#define NUMBER_OF_KEYS 8000

int keys[NUMBER_OF_KEYS][KEY];
int results[NUMBER_OF_NUMBERS];

pthread_mutex_t mutex[NUMBER_OF_NUMBERS];

void* statisticalInf(void *arg) {

    int id = *((int*) arg), i, j, start = id * 500, end = start + 500, keyIndex;

    printf("Thread(%d) ID: %lu em execução...\n", id + 1, pthread_self());

    for (i = start; i < end; i++) {
        for (j = 0; j < KEY; j++) {
            keyIndex = keys[i][j] - 1;
            pthread_mutex_lock(&mutex[keyIndex]);
            results[keyIndex]++;
            pthread_mutex_unlock(&mutex[keyIndex]);
        }
    }

    pthread_exit(NULL);
}

int main(void){
    
    int i, j, index[NUMBER_OF_THREADS];
    time_t t;
    pthread_t threads[NUMBER_OF_THREADS];

    srand((unsigned)time(&t));

    for (i = 0; i < NUMBER_OF_KEYS; i++) {
        for (j = 0; j < KEY; j++) {
            keys[i][j] = (rand() % 49) + 1;
        }
    }

    for (i = 0; i < NUMBER_OF_NUMBERS; i++) {
        results[i] = 0;
    }

    for (i = 0; i < NUMBER_OF_NUMBERS; i++) {
        if (pthread_mutex_init(&mutex[i], NULL) != 0) {
            perror("Erro ao criar mutex");
            exit(-1);
        }
    }

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        index[i] = i;
        if (pthread_create(&threads[i], NULL, statisticalInf, (void*) &index[i]) != 0) {
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
        printf("%d - %d\n", i + 1, results[i]);
    }

    return 0;
}