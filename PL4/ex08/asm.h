#ifndef ASM_H 
#define ASM_H

typedef struct {
    int clientNum;
    char clientName[12];
    char clientAddress[16];
} client;

#define LIMIT 100

typedef struct {
    client data[LIMIT];
    int readCount;
    int writeCount;
    int run;
    int index;
} sharedValues;

int fd;
sem_t *sem1, *sem2, *sem3, *semRead, *semWrite;
#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx08"

#endif