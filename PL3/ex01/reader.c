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

    int fd, clear;
    fd = shm_open(FILE_NAME, O_RDWR, S_IRUSR|S_IWUSR);
    if(fd == -1) {
		perror("Erro ao abrir memoria partilhada");
        exit (-1);
	}
    ftruncate (fd, DATA_SIZE);
    sharedStudent *shared_data = (sharedStudent*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    while (!shared_data -> canRead);
    printf("%d\n", shared_data -> studentNum);
    printf("%s\n", shared_data -> studentName);
    printf("%s\n", shared_data -> studentAdress);

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

    clear = shm_unlink(FILE_NAME);
    if (clear < 0) {
        perror("Erro ao remover o Ficheiro!");
        exit(-1);
    }

    return 0;
}