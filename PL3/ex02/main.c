#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>

typedef struct {
    int firstValue;
    int secondValue;
    int turn;
    bool flag[2];
} sharedValues;

#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx02"
#define NUMBER_OF_OPERATIONS 1000000

int main(void){

    int fd, status, i;
    pid_t pid;
    
    fd = shm_open(FILE_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}

    if (ftruncate (fd, DATA_SIZE) < 0) {
        perror("Erro ao alocar espaço na memória");
        exit(-1);
    }
    
    sharedValues *shared_data = (sharedValues*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    shared_data -> firstValue = 10000;
    shared_data -> secondValue = 500;
    shared_data -> turn = 0;
    shared_data -> flag[0] = false;
    shared_data -> flag[1] = false;

    pid = fork();
    if(pid < 0){
        perror("Erro ao criar o processo");
        exit(-1);
    }else if(pid == 0){
        for(i = 0; i < NUMBER_OF_OPERATIONS; i++){
            shared_data -> flag[1] = true;
            shared_data -> turn = 0;
            while (shared_data -> flag[0] && shared_data -> turn == 0);
            shared_data -> firstValue++;
            shared_data -> secondValue--;
            shared_data -> flag[1] = false;
        }

        if(munmap(shared_data, DATA_SIZE) < 0){
            perror("Erro ao remover mapping");
            exit(-1);
        }

        if(close(fd) < 0){
            perror("Erro ao fechar file descriptor");
            exit(-1);
        }

        exit(0);
    }
    for(i = 0; i < NUMBER_OF_OPERATIONS; i++){
        shared_data -> flag[0] = true;
        shared_data -> turn = 1;
        while(shared_data -> flag[1] && shared_data -> turn == 1);
        shared_data -> firstValue--;
        shared_data -> secondValue++;
        shared_data -> flag[0] = false;
    }

    waitpid(pid, &status, 0);
    printf("1º valor: %d\n2º valor: %d\n", shared_data -> firstValue, shared_data -> secondValue);

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

/*

Q: Will these results always be correct? Propose a solution that ensures data consistency.

A:
    Não, os resultados nem sempre estarão corretos, pois ambos os processos Pai e filho 
(em execução concorrente) estão a tentar aceder e modificar os dados presentes na mesma 
zona de memória partilhada sem nenhum tipo de mecanismo de coordenação, o que pode levar 
a comportamentos e resultados inesperados.

    Uma solução possível que garante a consistencia dos dados é a sincronização por active wait (spinning)
utilizado neste exercício.

*/