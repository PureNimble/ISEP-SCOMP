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

    int fd;
    fd = shm_open(FILE_NAME, O_RDONLY, S_IRUSR|S_IWUSR);
    if(fd < 0) {
		perror("Erro ao abrir memoria partilhada");
        exit (-1);
	}
    
    sharedStudent *shared_data = (sharedStudent*) mmap(NULL, DATA_SIZE, PROT_READ, MAP_SHARED, fd, 0);

    while (!shared_data -> canRead);
    printf("Número do aluno: %d\n", shared_data -> studentNum);
    printf("Nome do aluno: %s\n", shared_data -> studentName);
    printf("Morada do aluno: %s\n", shared_data -> studentAdress);

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