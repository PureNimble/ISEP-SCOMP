#ifndef ASM_H 
#define ASM_H

typedef struct {
    int studentNum;
    int canRead;
    char studentName[10];
    char studentAdress[16];
} sharedStudent;
#define DATA_SIZE sizeof(sharedStudent)
#define FILE_NAME "/shmEx01"

#endif