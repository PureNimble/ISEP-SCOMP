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
				printf("Son number 1 return value = %d\n", WEXITSTATUS(status1));
				printf("Son number 2 return value = %d\n", WEXITSTATUS(status2));
			}
        }
        else{
            sleep(2);
            exit(2);
        }
    }
    else{
        sleep(1);
        exit(1);
    }
    return 0;
}