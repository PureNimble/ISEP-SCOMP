#include <stdio.h>
#include <sys/types.h> 
#include <stdlib.h>
#include <unistd.h>

int main() {
    int i;
    int status;
    for (i = 0; i < 4; i++){
        if (fork() == 0) {
            exit(0);
        }
    }
    printf ("This is the end.\n");
    return 0;
}