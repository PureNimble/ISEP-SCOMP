#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NUMBER_OF_THREADS 10
#define ARRAY_SIZE 1000

int randomNum;
int searchArray[ARRAY_SIZE];

typedef struct {
    int idx;
} winner;

void* threadFunc(void* arg) {
    int id = *((int*) arg), i, start = id * 100, end = start + 100;
    id++;

    for (i = start; i < end; i++) {
        if (searchArray[i] == randomNum) {
            winner* winnersPoint = (winner*) malloc(sizeof(winner));
            winnersPoint -> idx = id;
            printf("Thread(%d) ID: %lu encontrou o valor %d na posição %d\n", id, pthread_self(), randomNum, i);
            pthread_exit((void*) winnersPoint);
        }
    }
    printf("Thread(%d) ID: %lu\n", id, pthread_self());
    pthread_exit(NULL);
}

int main(void) {
    int i, finalWinner, index[NUMBER_OF_THREADS];
    pthread_t threads[NUMBER_OF_THREADS];
    time_t t;
    void* ret;

    srand((unsigned)time(&t));

    randomNum = (rand() % ARRAY_SIZE) + 1;

    for (i = 0; i < ARRAY_SIZE; i++) {
        searchArray[i] = i + 1;
    }

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        
        index[i] = i;
        if (pthread_create(&threads[i], NULL, threadFunc, (void*) &index[i]) != 0) {
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
