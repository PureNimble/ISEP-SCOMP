#ifndef ASM_H
#define ASM_H

#define ARRAY_SIZE 10
typedef struct {
    int array[ARRAY_SIZE];
    int canRead;
} sharedArray;
#define DATA_SIZE sizeof(sharedArray)
#define FILE_NAME "/shmEx03"

#endif