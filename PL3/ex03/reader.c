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

    int fd, i, *ptr;
    float average = 0;

    fd = shm_open(FILE_NAME, O_RDONLY, S_IRUSR|S_IWUSR);
    if(fd < 0) {
		perror("Erro ao abrir memoria partilhada");
        exit (-1);
	}

    if (ftruncate (fd, DATA_SIZE) < 0) {
        perror("Erro ao alocar espaço na memória");
        exit(-1);
    }
    
    sharedArray *shared_data = (sharedArray*) mmap(NULL, DATA_SIZE, PROT_READ, MAP_SHARED, fd, 0);

    while(!shared_data -> canRead);
    ptr = shared_data -> array;
    for(i = 0; i < ARRAY_SIZE; i++){
        average += *ptr;
        ptr++;
    }
    average /= ARRAY_SIZE;

    printf("Média: %.2f\n", average);

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