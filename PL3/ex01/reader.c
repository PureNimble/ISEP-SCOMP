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

    int fd, data_size = sizeof(sharedStudent);
    sharedStudent *shared_data;
    fd = shm_open("/shmtest", O_RDWR, S_IRUSR|S_IWUSR);
    if(fd == -1) {
		perror("Erro ao abrir memoria partilhada");
        return -1;
	}
    ftruncate (fd, data_size);
    shared_data = (sharedStudent*)mmap(NULL, data_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(fd == -1) {
		perror("Erro no mmap");
        return -1;
	}
    while (!shared_data -> newData);
    
    printf("%d\n", shared_data -> studentNum);
    printf("%s\n", shared_data -> studentName);
    printf("%s\n", shared_data -> studentAdress);

    return 0;
}