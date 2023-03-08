#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
    
    int i, final, counter = -99;
    pid_t pidsList[10];

    for (i = 0; i < 10; i++){
        counter+=100;
        pidsList[i] = fork();
        if(pidsList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }
        if(pidsList[i] == 0){
            final = counter + 100;
            for(i = counter; i < final; i++){
                printf("%d\n", i);
            }
            exit(0);
        }
    }
   for(i = 0; i < 10; i++){
        waitpid(pidsList[i], NULL, 0);
    }
    printf ("Fim (processamento pai)\n");
    return 0;
}