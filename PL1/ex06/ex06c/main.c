#include <stdio.h>
#include <sys/types.h> 
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int i, status;
    pid_t listPids[4];
    for (i = 0; i < 4; i++){
        if ((listPids[i] = fork()) == 0) {
            printf("Processo %d com PID %d.\n", i+1, getpid());
            exit(0);
        }
    }
    for(i = 0; i < 4; i++){
        if(listPids[i] % 2 == 0){
            printf("PID par(%d) em execução...\n", listPids[i]);
			waitpid(listPids[i], &status, 0);
            if(WIFEXITED(status)){
				printf("PID par (%d) executado com sucesso.\n", listPids[i]);
			}
        }
    }
    printf ("This is the end.\n");
    return 0;
}