#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NUMBER_OF_THREADS 5
#define ARRAY_SIZE 300

typedef struct {
    int number;
    int g1Result;
    int g2Result;
    int g3Result;
    int finalGrade;
} Exam;

Exam arrayProva[ARRAY_SIZE];

int pos = 0, neg = 0, canRead = 0, canWrite = 2, readPos = 0, processedInc = 0, posReady = 0, negReady = 0, posWaiting = 0, negWaiting = 0;

pthread_mutex_t mutexExames;
pthread_mutex_t mutexFinalGrades;

pthread_cond_t condGrade[4];
pthread_cond_t condNotEmpty;
pthread_cond_t condNotFull;

void* generateExames(void *arg) {

    int id = *((int*) arg), i;
    time_t t;

    srand((unsigned)time(&t));

    printf("Thread(%d) ID: %lu a gerar exames...\n", id + 1, pthread_self());
    for (i = 0; i < ARRAY_SIZE; i++) {

        pthread_mutex_lock(&mutexExames);
        while (canWrite == 0) pthread_cond_wait(&condNotFull, &mutexExames);
        canWrite--;

        arrayProva[i].number = (rand() % 10000) + 10000;
        arrayProva[i].g1Result = rand() % 101;
        arrayProva[i].g2Result = rand() % 101;
        arrayProva[i].g3Result = rand() % 101;

        printf("Exame nº %d gerado\n", i + 1);
        fflush(stdout);

        canRead++;
        pthread_cond_signal(&condNotEmpty);
        pthread_mutex_unlock(&mutexExames);

    }

    pthread_exit(NULL);
}

void* computeGrade(void *arg) {

    int id = *((int*) arg), tempG1, tempG2, tempG3, grade;

    printf("Thread(%d) ID: %lu a calcular as notas finais...\n", id + 1, pthread_self());

    while (readPos < ARRAY_SIZE) {
        pthread_mutex_lock(&mutexExames);
        while (canRead == 0 && readPos != ARRAY_SIZE) pthread_cond_wait(&condNotEmpty, &mutexExames);
        if (readPos != ARRAY_SIZE){
            canRead--;

            tempG1 = arrayProva[readPos].g1Result;
            tempG2 = arrayProva[readPos].g2Result;
            tempG3 = arrayProva[readPos].g3Result;
            grade = (tempG1 + tempG2 + tempG3)/3;
            arrayProva[readPos].finalGrade = grade;
            readPos++;

            printf("Thread(%d) calculou a nota final do %dº exame!\n", id + 1, readPos);
            fflush(stdout);

            pthread_mutex_lock(&mutexFinalGrades);
            if (grade >= 50) {
                posWaiting++;
                while (posReady == 0) pthread_cond_wait(&condGrade[2], &mutexFinalGrades);
                posReady--;
                pthread_cond_signal(&condGrade[0]);
            } else {
                negWaiting++;
                while (negReady == 0) pthread_cond_wait(&condGrade[3], &mutexFinalGrades);
                negReady--;
                pthread_cond_signal(&condGrade[1]);
            }
            pthread_mutex_unlock(&mutexFinalGrades);

            canWrite++;

            pthread_cond_signal(&condNotFull);
        }
        pthread_mutex_unlock(&mutexExames);
    }
    
    pthread_exit(NULL);
}

void* positive(void *arg) {

    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += 1;

    int id = *((int*) arg);

    printf("Thread(%d) ID: %lu a atualizar as notas positivas...\n", id + 1, pthread_self());

    while (processedInc < ARRAY_SIZE) {
        pthread_mutex_lock(&mutexFinalGrades);
        posReady++;
        if (posWaiting != 0) pthread_cond_signal(&condGrade[2]);
        posWaiting--;
        pthread_cond_timedwait(&condGrade[0], &mutexFinalGrades, &abstime);
        if (processedInc < ARRAY_SIZE) {
            pos++;
            processedInc++;
            printf("Thread(%d) recebeu uma nota positiva!\n", id + 1);
            fflush(stdout);
        }
        pthread_mutex_unlock(&mutexFinalGrades);
    }

    pthread_exit(NULL);
}

void* negative(void *arg) {

    struct timespec abstime;
    clock_gettime(CLOCK_REALTIME, &abstime);
    abstime.tv_sec += 1;

    int id = *((int*) arg);

    printf("Thread(%d) ID: %lu a atualizar as notas negativas...\n", id + 1, pthread_self());

    while (processedInc < ARRAY_SIZE) {
        pthread_mutex_lock(&mutexFinalGrades);
        negReady++;
        if (negWaiting != 0) pthread_cond_signal(&condGrade[3]);
        negWaiting--;
        pthread_cond_timedwait(&condGrade[1], &mutexFinalGrades, &abstime);
        if (processedInc < ARRAY_SIZE) {
            neg++;
            processedInc++;
            printf("Thread(%d) recebeu uma nota negativa!\n", id + 1);
            fflush(stdout);
        }
        pthread_mutex_unlock(&mutexFinalGrades);
    }
    pthread_exit(NULL);
}

int main(void){
    
    int i, index[NUMBER_OF_THREADS];
    pthread_t threads[NUMBER_OF_THREADS];

    if (pthread_mutex_init(&mutexExames, NULL) != 0 || pthread_mutex_init(&mutexFinalGrades, NULL) != 0) {
        perror("Erro ao criar mutex");
        exit(-1);
    }

    if (pthread_cond_init(&condNotEmpty, NULL) != 0 || pthread_cond_init(&condNotFull, NULL) != 0) {
        perror("Erro ao criar variavel condicional");
        exit(-1);
    }

    for (i = 0; i < 4; i++) {
        if (pthread_cond_init(&condGrade[i], NULL) != 0) {
            perror("Erro ao criar variavel condicional");
            exit(-1);
        }
    }

    index[0] = 0;
    if (pthread_create(&threads[0], NULL, generateExames, (void*) &index[0]) != 0) {
        perror("Erro ao criar thread");
        exit(-1);
    }

    for (i = 1; i < 3; i++) {
        index[i] = i;
        if (pthread_create(&threads[i], NULL, computeGrade, (void*) &index[i]) != 0) {
            perror("Erro ao criar thread");
            exit(-1);
        }
    }

    index[3] = 3;
    if (pthread_create(&threads[3], NULL, positive, (void*) &index[3]) != 0) {
        perror("Erro ao criar thread");
        exit(-1);
    }

    index[4] = 4;
    if (pthread_create(&threads[4], NULL, negative, (void*) &index[4]) != 0) {
        perror("Erro ao criar thread");
        exit(-1);
    }

    for (i = 0; i < NUMBER_OF_THREADS; i++) {

        if (pthread_join(threads[i], NULL) != 0) {
            perror("Erro ao dar join das threads");
            exit(-1);
        }
    }

    for (i = 0; i < 4; i++) {
        pthread_cond_destroy(&condGrade[i]);
    }
    pthread_cond_destroy(&condNotFull);
    pthread_cond_destroy(&condNotEmpty);
    pthread_mutex_destroy(&mutexFinalGrades);
    pthread_mutex_destroy(&mutexExames);

    printf("Todas as notas finais foram recebidas!\n");
    for (i = 0; i < ARRAY_SIZE; i++) {
        printf("Aluno nº: %d, G1: %d, G2: %d, G3: %d, Nota final: %d\n", arrayProva[i].number, arrayProva[i].g1Result, arrayProva[i].g2Result, arrayProva[i].g3Result, arrayProva[i].finalGrade);
    }

    double posPercentage = (double) pos / 300 * 100;
    double negPercentage = (double) neg / 300 * 100;
    printf("Nº de notas positivas: %d, Percentagem: %.0f%%\n", pos, posPercentage);
    printf("Nº de notas negativas: %d, Percentagem: %.0f%%\n", neg, negPercentage);

    return 0;
}