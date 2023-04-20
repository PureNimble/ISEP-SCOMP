#ifndef ASM_H
#define ASM_H

#define ARRAY_SIZE 10
#define HALF_ARRAY 5

typedef struct {
    int array[ARRAY_SIZE];
    int canRead;
    int canWrite;
} sharedArray;

#define DATA_SIZE sizeof(sharedArray)
#define FILE_NAME "/shmEx04"

#endif