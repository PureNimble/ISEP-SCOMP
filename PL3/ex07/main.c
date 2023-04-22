#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

#define STR_SIZE 50
#define NR_DISC 10
#define NUMBER_OF_CHILDREN 2

struct aluno{
    int numero;
    char nome[STR_SIZE];
    int disciplinas[NR_DISC];
    int canRead;
};

#define DATA_SIZE sizeof(struct aluno)
#define FILE_NAME "/shmEx07"

int main(void){

    int fd, status, i;
    pid_t firstChild, secondChild;
    
    fd = shm_open(FILE_NAME, O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd < 0) {
		perror("Erro ao criar memoria partilhada");
        exit(-1);
	}

    if (ftruncate (fd, DATA_SIZE) < 0) {
        perror("Erro ao alocar espaço na memória");
        exit(-1);
    }

    struct aluno *shared_data = (struct aluno*) mmap(NULL, DATA_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    firstChild = fork();
    if(firstChild < 0){
        perror("Erro ao criar o processo");
        exit(-1);
    }else if(firstChild == 0){
        int max = 0, min = 20;
        while(!shared_data -> canRead);

        for(i = 0; i < NR_DISC; i++){
            if(max < shared_data -> disciplinas[i]){
                max = shared_data -> disciplinas[i];
            }else if(min > shared_data -> disciplinas[i]){
                min = shared_data -> disciplinas[i];
            }
        }
        printf("Aluno %s nº %d \n", shared_data -> nome, shared_data -> numero);
        printf("Nota mais alta: %d\n", max);
        printf("Nota mais baixa: %d\n", min);

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

    secondChild = fork();
    if(secondChild < 0){
        perror("Erro ao criar o processo");
        exit(-1);
    }else if(secondChild == 0){
        float average = 0;
        while(!shared_data -> canRead);

        for(i = 0; i < NR_DISC; i++){
            average += shared_data -> disciplinas[i];
        }
        average /= NR_DISC;
        printf("Média: %.2f\n", average);

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
    printf("Introduza o número do aluno:\n");
	scanf("%d", &shared_data -> numero);

    printf("Introduza o nome do aluno:\n");
	scanf("%s", shared_data -> nome);

    for(i = 0; i < NR_DISC; i++){
        printf("Introduza a %dº nota do aluno:\n", i + 1);
	    scanf("%d", &shared_data -> disciplinas[i]);
    }

    shared_data -> canRead = 1;
    
    waitpid(firstChild, &status, 0);
    waitpid(secondChild, &status, 0);

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