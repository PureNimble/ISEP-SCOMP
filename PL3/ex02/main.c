#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

typedef struct {
    int firstValue;
    int secondValue;
    int allowFather;
    int allowSon;
} sharedValues;

#define DATA_SIZE sizeof(sharedValues)
#define FILE_NAME "/shmEx02"
#define NUMBER_OF_OPERATIONS 1000000

int main(void){

    int fd, clear, status, i;
    pid_t pid;
    fd = shm_open(FILE_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd == -1) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}
    ftruncate (fd, DATA_SIZE);
    sharedValues *shared_data = (sharedValues*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    shared_data -> firstValue = 1000;
    shared_data -> secondValue = 500;
    shared_data -> allowFather = 1;
    shared_data -> allowSon = 0;

    pid = fork();
    if(pid < 0){
        perror("Erro ao criar o processo");
        exit(-1);
    }else if(pid == 0){
        for(i = 0; i < NUMBER_OF_OPERATIONS; i++){
            while(!shared_data -> allowSon);
            shared_data -> allowSon = 0;
            shared_data -> firstValue++;
            shared_data -> secondValue--;
            shared_data -> allowFather = 1;
        }

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
        exit(0);
    }
    for(i = 0; i < NUMBER_OF_OPERATIONS; i++){
        while(!shared_data -> allowFather);
        shared_data -> allowFather = 0;
        shared_data -> firstValue--;
        shared_data -> secondValue++;
        shared_data -> allowSon = 1;
    }

    waitpid(pid, &status, 0);
    printf("1º valor: %d\n2º valor: %d\n", shared_data -> firstValue, shared_data -> secondValue);

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

/*

Q: Will these results always be correct? Propose a solution that ensures data consistency.

A:
    Não, os resultados nem sempre estarão corretos, pois ambos os processos Pai e filho 
(em execução concorrente) estão a tentar aceder e modificar os dados presentes na mesma 
zona de memória partilhada sem nenhum tipo demecanismo de coordenação, o que pode levar 
a comportamentos e resultados inesperados.

    Uma solução possível que garante a consistencia dos dados é a sincronização por active wait (spinning)
utilizado neste exercício.

*/