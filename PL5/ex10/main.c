#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NUMBER_OF_THREADS 6
#define HALF_NUMBER_OF_THREADS (NUMBER_OF_THREADS / 2)
#define NUMBER_OF_MARKETS 3
#define ARRAY_SIZE 10000

typedef struct {
    int id_h;
    int id_p;
    int p;
} data;

int vec1Index = 0, vec2Index = 0, vec3Index = 0, threadCount = 0, lowestMarket = 0;
double lowestCost = ARRAY_SIZE;

data vec[ARRAY_SIZE], vec1[ARRAY_SIZE], vec2[ARRAY_SIZE], vec3[ARRAY_SIZE];
pthread_mutex_t mutex, marketMutex[NUMBER_OF_MARKETS];
pthread_cond_t cond;

void* filtering(void* arg) {
    
    int id = *((int*) arg), i, start = id * 3333, end = start + 3333, marketTemp;

    if (id == 2){
        end++;
    }

    printf("Thread(%d) ID: %lu a filtrar os dados...\n", id + 1, pthread_self());

    for (i = start; i < end; i++) {
        marketTemp = vec[i].id_h;
        if (marketTemp == 1) {
            pthread_mutex_lock(&marketMutex[0]);
            vec1[vec1Index] = vec[i];
            vec1Index++;
            pthread_mutex_unlock(&marketMutex[0]);
        } else if (marketTemp == 2) {
            pthread_mutex_lock(&marketMutex[1]);
            vec2[vec2Index] = vec[i];
            vec2Index++;
            pthread_mutex_unlock(&marketMutex[1]);
        } else {
            pthread_mutex_lock(&marketMutex[2]);
            vec3[vec3Index] = vec[i];
            vec3Index++;
            pthread_mutex_unlock(&marketMutex[2]);
        }
    }

    printf("Thread(%d) ID: %lu terminou de filtrar os dados...\n", id + 1, pthread_self());

    pthread_mutex_lock(&mutex);
    threadCount++;
    if (threadCount == HALF_NUMBER_OF_THREADS) {
        pthread_cond_broadcast(&cond);
        printf("Thread(%d) ID: %lu notificou as threads 4, 5 e 6!\n", id + 1, pthread_self());
    }
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void* computing(void* arg) {

    int id = *((int*) arg), prodCount[5] = {0}, size, i, prodId, storeId;
    double avg[5] = {0}, sum = 0;
    data* dataArray;

    pthread_mutex_lock(&mutex);
    while (threadCount < HALF_NUMBER_OF_THREADS) {
        printf("Thread(%d) ID: %lu á espera dos dados...\n", id + 1, pthread_self());
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);

    printf("Thread(%d) ID: %lu a executar os calculos..\n", id + 1, pthread_self());

    if (id == 3) {
        storeId = 1;
        dataArray = vec1;
        size = vec1Index;
    } else if (id == 4) {
        storeId = 2;
        dataArray = vec2;
        size = vec2Index;
    } else {
        storeId = 3;
        dataArray = vec3;
        size = vec3Index;
    }

    for (i = 0; i < size; i++) {
        prodId = dataArray[i].id_p;
        avg[prodId - 1] += dataArray[i].p;
        prodCount[prodId - 1]++;
    }

    for (i = 0; i < 5; i++) {
        if (prodCount[i] > 0) {
            avg[i] /= prodCount[i];
        }
        sum += avg[i];
    }

    pthread_mutex_lock(&mutex);
    if (sum < lowestCost) {
        lowestCost = sum;
        lowestMarket = storeId;
    }
    pthread_mutex_unlock(&mutex);

    printf("Thread(%d) ID: %lu terminou de efetuar os calculos..\n", id + 1, pthread_self());

    pthread_exit(NULL);
}

int main(void) {

    int i, tempMark, tempProd, index[NUMBER_OF_THREADS];
    time_t t;
    pthread_t threads[NUMBER_OF_THREADS];

    srand((unsigned)time(&t));

    for (i = 0; i < ARRAY_SIZE; i++) {
        tempMark = (rand() % NUMBER_OF_MARKETS) + 1;
        tempProd = (rand() % 5) + 1;
        vec[i].id_h = tempMark;
        vec[i].id_p = tempProd;
        vec[i].p = (rand() % 20) + 1;
    }

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        perror("Erro ao criar mutex");
        exit(-1);
    }

    for (i = 0; i < NUMBER_OF_MARKETS; i++) {
        if (pthread_mutex_init(&marketMutex[i], NULL) != 0) {
            perror("Erro ao criar mutex");
            exit(-1);
        }
    }

    if (pthread_cond_init(&cond, NULL) != 0) {
        perror("Erro ao criar variavel condicional");
        exit(-1);
    }

    for (i = 0; i < HALF_NUMBER_OF_THREADS; i++) {
        index[i] = i;
        if (pthread_create(&threads[i], NULL, filtering, (void*) &index[i]) != 0) {
            perror("Erro ao criar thread");
            exit(-1);
        }
    }

    for (i = HALF_NUMBER_OF_THREADS; i < NUMBER_OF_THREADS; i++) {
        index[i] = i;
        if (pthread_create(&threads[i], NULL, computing, (void*) &index[i]) != 0) {
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

    pthread_cond_destroy(&cond);
    for (i = 0; i < NUMBER_OF_MARKETS; i++) {
        pthread_mutex_destroy(&marketMutex[i]);
    }
    pthread_mutex_destroy(&mutex);

    printf("A loja %d teve o menor custo de %.2f\n", lowestMarket, lowestCost);

    return 0;
}
