#ifndef ASM_H 
#define ASM_H

#define ARRAY_SIZE 10
#define END 5

typedef struct {
    int array[ARRAY_SIZE];
    int run;
} sharedValues;

int fd, data[ARRAY_SIZE];
sem_t *semA, *semB, *semWrite;
#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx14"

#endif