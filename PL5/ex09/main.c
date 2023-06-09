#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NUMBER_OF_THREADS 20
#define NUMBER_OF_TRAILS 3

pthread_mutex_t mutex[NUMBER_OF_TRAILS];

void nextCity(char currentCity, int id) {
    int randomNum;
    time_t t;
    srand((unsigned)time(&t) * id);

    if (currentCity == 'A') {
        pthread_mutex_lock(&mutex[0]);
        printf("Comboio(%d) partiu da cidade A em direção á cidade B...\n", id + 1);
        sleep(4);
        printf("Comboio(%d) chegou á cidade B (tempo de viagem: 4s)\n", id + 1);
        pthread_mutex_unlock(&mutex[0]);
    } else if (currentCity == 'B') {
        randomNum = rand() % 3;
        if (randomNum == 0) {
            pthread_mutex_lock(&mutex[0]);
            printf("Comboio(%d) partiu da cidade B em direção á cidade A...\n", id + 1);
            fflush(stdout);
            sleep(4);
            printf("Comboio(%d) chegou á cidade A (tempo de viagem: 4s)\n", id + 1);
            fflush(stdout);
            pthread_mutex_unlock(&mutex[0]);
        }
        else if (randomNum == 1) {
            pthread_mutex_lock(&mutex[1]);
            printf("Comboio(%d) partiu da cidade B em direção á cidade C...\n", id + 1);
            fflush(stdout);
            sleep(6);
            printf("Comboio(%d) chegou á cidade C (tempo de viagem: 6s)\n", id + 1);
            fflush(stdout);
            pthread_mutex_unlock(&mutex[1]);
        } else if (randomNum == 2) {
            pthread_mutex_lock(&mutex[2]);
            printf("Comboio(%d) partiu da cidade B em direção á cidade D...\n", id + 1);
            fflush(stdout);
            sleep(2);
            printf("Comboio(%d) chegou á cidade D (tempo de viagem: 2s)\n", id + 1);
            fflush(stdout);
            pthread_mutex_unlock(&mutex[2]);
        }
    } else if (currentCity == 'C') {
        pthread_mutex_lock(&mutex[1]);
        printf("Comboio(%d) partiu da cidade C em direção á cidade B...\n", id + 1);
        fflush(stdout);
        sleep(6);
        printf("Comboio(%d) chegou á cidade B (tempo de viagem: 6s)\n", id + 1);
        fflush(stdout);
        pthread_mutex_unlock(&mutex[1]);
    } else {
        pthread_mutex_lock(&mutex[2]);
        printf("Comboio(%d) partiu da cidade D em direção á cidade B...\n", id + 1);
        fflush(stdout);
        sleep(2);
        printf("Comboio(%d) chegou á cidade B (tempo de viagem: 2s)\n", id + 1);
        fflush(stdout);
        pthread_mutex_unlock(&mutex[2]);
    }
    
}

void* train(void *arg) {

    int id = *((int*) arg), randomNum;
    time_t t;
    srand((unsigned)time(&t) * id);

    randomNum = rand() % 3;

    if (randomNum == 0) {
        nextCity('A', id);
    } else if (randomNum == 1) {
        nextCity('C', id);
    } else if (randomNum == 2){
        nextCity('D', id);
    }
    nextCity('B', id);

    pthread_exit(NULL);
}

int main(void){
    
    int i, index[NUMBER_OF_THREADS];
    pthread_t threads[NUMBER_OF_THREADS];

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        if (pthread_mutex_init(&mutex[i], NULL) != 0) {
            perror("Erro ao criar mutex");
            exit(-1);
        }
    }
    

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        index[i] = i;
        if (pthread_create(&threads[i], NULL, train, (void*) &index[i]) != 0) {
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
        pthread_mutex_destroy(&mutex[i]);
    }

    return 0;
}