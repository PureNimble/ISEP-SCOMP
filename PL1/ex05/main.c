#include <stdio.h>
#include <sys/types.h> 
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void){
    int status1, status2;
    pid_t p1 = fork();
    waitpid(p1, &status1, 0);
    if(p1>0){
        pid_t p2 = fork();
        waitpid(p2, &status2, 0);
        if(p2>0){
            if(WIFEXITED(status1) && WIFEXITED(status2)){
				printf("Valor de retorno do filho 1 = %d\n", WEXITSTATUS(status1));
				printf("Valor de retorno do filho 2 = %d\n", WEXITSTATUS(status2));
			}
        }
        else if(p2==0){
            printf("Filho 2 a dormir (não incomodar)\n");
            sleep(2);
            exit(2);
        }
        else{
            perror("Erro ao criar o Processo (filho 2)");
            exit(-1);
        }
    }
    else if(p1==0){
        printf("Filho 1 a dormir (não incomodar)\n");
        sleep(1);
        exit(1);
    }
    else{
        perror("Erro ao criar o Processo (filho 1)");
        exit(-1);
    }
    return 0;
}