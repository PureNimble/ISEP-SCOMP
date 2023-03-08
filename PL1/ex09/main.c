#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
    
    int i, j, final, counter = -99;
    pid_t pid;

    for (i = 0; i < 10; i++){
        counter+=100;
        pid = fork();
        if(pid < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }
        if(pid == 0){
            final = counter + 100;
            for(j = counter; j < final; j++){
                printf("%d\n", j);
            }
            exit(0);
        }
    }
    while(wait(NULL) > 0);
    printf ("Fim (processamento pai)\n");
    return 0;
}


/*

a)   
     Q: Is the output sorted? Can you guarantee it will be always sorted? Why?
     A: Não, nem é possível garantir uma apresentação ordenada, pois não existe forma de garantir que um processo executa antes de outro,
        ou seja, a apresentação dos resultados é feita de forma aleatória.

*/