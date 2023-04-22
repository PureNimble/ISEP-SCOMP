#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include "asm.h"

int main(void){

    int fd, i, j;
    time_t t;
    
   fd = shm_open(FILE_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd == -1) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}
    
    if (ftruncate (fd, DATA_SIZE) < 0) {
        perror("Erro ao alocar espaço na memória");
        exit(-1);
    }
    
    sharedArray *shared_data = (sharedArray*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    
    shared_data -> canWrite = 1;
    shared_data -> canRead = 0;

    srand((unsigned) time(&t));
    for(i = 0; i < HALF_ARRAY; i++){
        while(!shared_data -> canWrite);
        shared_data -> canWrite = 0;
        for(j = 0; j < ARRAY_SIZE; j++){
            shared_data -> array[j] = rand() % 20 + 1;
        }
        shared_data -> canRead = 1;
    }

    if(munmap(shared_data, DATA_SIZE) < 0){
        perror("Erro ao remover mapping");
        exit(-1);
    }

    if(close(fd) < 0){
        perror("Erro ao fechar file descriptor");
        exit(-1);
    }


    return 0;
}