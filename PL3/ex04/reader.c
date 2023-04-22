#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "asm.h"

int main(void){

    int fd, i, j;
    float average = 0;
    fd = shm_open(FILE_NAME, O_RDWR, S_IRUSR|S_IWUSR);
    if(fd == -1) {
		perror("Erro ao abrir memoria partilhada");
        exit (-1);
	}

    if (ftruncate (fd, DATA_SIZE) < 0) {
        perror("Erro ao alocar espaço na memória");
        exit(-1);
    }
    
    sharedArray *shared_data = (sharedArray*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    for(i = 0; i < HALF_ARRAY; i++){
        while(!shared_data -> canRead);
        shared_data -> canRead = 0;
        for(j = 0; j < ARRAY_SIZE; j++){
            average += shared_data -> array[j];
        }
        average /= ARRAY_SIZE;
        printf("Média do %dº array: %.2f\n", i + 1, average);
        average = 0;
        shared_data -> canWrite = 1;
    }

    if(munmap(shared_data, DATA_SIZE) < 0){
        perror("Erro ao remover mapping");
        exit(-1);
    }

    if(close(fd) < 0){
        perror("Erro ao fechar file descriptor");
        exit(-1);
    }

    if (shm_unlink(FILE_NAME) < 0) {
        perror("Erro ao remover o Ficheiro!");
        exit(-1);
    }

    return 0;
}