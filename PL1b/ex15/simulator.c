#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(char *argv[]){
    int i;
    for(i = 0; i < 5; i++){
        printf("Comando %s em execução...\n", argv[i]);
        sleep(10);
        printf("Execution ended!\n");
    }
    
    return 0;
}