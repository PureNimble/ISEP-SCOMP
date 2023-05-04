#ifndef ASM_H 
#define ASM_H

typedef struct {
    int ticket;
    int ticketsNum;
} sharedValues;

int fd;
sem_t *semCostumer, *semSeller;
#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx11"

#endif