#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef struct {
    int studentNum;
    int newData;
    char studentName[10];
    char studentAdress[16];
} sharedStudent;

int main(void){

    int fd, r, data_size = sizeof(sharedStudent);
    sharedStudent *shared_data;
    fd = shm_open("/shmtest", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
    if(fd == -1) {
		perror("Erro ao criar memoria partilhada");
        return -1;
	}
    ftruncate (fd, data_size);
    shared_data = (sharedStudent*)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(fd == -1) {
		perror("Erro no mmap");
        return -1;
	}
    shared_data -> newData = 1;
    printf("Introduza o número do aluno:\n");
	scanf("%d", &shared_data -> studentNum);

    printf("Introduza o nome do aluno:\n");
	scanf("%s", shared_data -> studentName);

    printf("Introduza o endereço do aluno:\n");
	scanf("%s", shared_data -> studentAdress);

    /*r = munmap(shared_data, data_size);
    if (r < 0) exit(1);
    r = close(fd);
    if (r < 0) exit(1);*/
    
    return 0;
}