#ifndef ASM_H 
#define ASM_H

typedef struct {
    pid_t pid;
    char currentTime[26];
    int readCount;
    int writeCount;
} sharedValues;

int fd;
sem_t *semWriter, *semReader, *sem1, *sem2, *sem3;
#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx13ab"

#endif