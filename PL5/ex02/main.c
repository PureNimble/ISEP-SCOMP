#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define NUMBER_OF_THREADS 5

typedef struct {
    int num;
    char name[24];
    int grade;
} student;

void* threadFunc(void *arg){

    printf("Thread ID: %lu\n", pthread_self());
    student *dataPoint = ((student*) arg);
    printf("Número: %d\nNome: %s\nNota: %d\n", dataPoint -> num, dataPoint -> name, dataPoint -> grade);
    printf("------------------------\n");
	pthread_exit(NULL);
}

int main(void){
    
    int i;
    pthread_t threads[NUMBER_OF_THREADS];
    student data[NUMBER_OF_THREADS];
    time_t t;

    for (i = 0; i < NUMBER_OF_THREADS; i++) {
        
        srand ((unsigned) time(&t) * i);
		data[i].num = (rand() % 100) + 1;
		sprintf(data[i].name, "Nome %d", i + 1);
		data[i].grade = (rand() % 10) + 10;
	}

    for (i = 0; i < NUMBER_OF_THREADS; i++) {

        if (pthread_create(&threads[i], NULL, threadFunc, (void*) &data[i]) != 0) {
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

    return 0;
}