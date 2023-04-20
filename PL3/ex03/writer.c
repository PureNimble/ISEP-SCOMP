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

    int fd, clear, *ptr, i;
    time_t t;
    fd = shm_open(FILE_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd == -1) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}
    ftruncate (fd, DATA_SIZE);
    sharedArray *shared_data = (sharedArray*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    shared_data -> canRead = 0;
    srand((unsigned) time(&t));
    ptr = shared_data -> array;
    for (i = 0; i < ARRAY_SIZE; i++){
        *ptr = rand() % 20 + 1;
        ptr++;
    }
    shared_data -> canRead = 1;

    clear = munmap(shared_data, DATA_SIZE);
    if(clear < 0){
        perror("Erro ao remover mapping");
        exit(-1);
    }

    clear = close(fd);
    if(clear < 0){
        perror("Erro ao fechar file descriptor");
        exit(-1);
    }

    return 0;
}