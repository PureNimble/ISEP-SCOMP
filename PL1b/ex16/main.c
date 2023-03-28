#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#define NUMBER_OF_CHILDREN 50

volatile sig_atomic_t counter_handlerSIGUSR1 = 0;
volatile sig_atomic_t counter_handler = 0;


void handler(int signo, siginfo_t *sinfo, void *context){

    if(signo == SIGUSR1){
        counter_handlerSIGUSR1++;
    }
    counter_handler++;

}

int simulate1(){

    int n;
    int result;
    time_t t;
    printf("estou aqui, no simulate1");
    /* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand ((unsigned) time(&t));

    /* initialize n */
    n = rand() % 10 + 1;
    result = rand() % 500;

    if (result > 50){
        result = 0;
    }
    else result = 1;


    sleep(n);
    return result;

}


int simulate2(){

    int n;
    int result;
    time_t t;
    /* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand ((unsigned) time(&t));

    /* initialize n */
    n = rand() % 10 + 1;
    result = rand() % 500;

    if (result > 50){
        result = 0;
    }
    else result = 1;

    printf("sou eu, o simulate2");
    sleep(n);
    return result;

}


int main(void){
    int i, valor;
    pid_t pidList[NUMBER_OF_CHILDREN], parentPid = getpid();
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handler;
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }
        if(pidList[i] == 0){
            printf("estou aqui, nos filhos");
            valor = simulate1();
            if(valor == 1){
                kill(parentPid, SIGUSR1);
            }
            else kill(parentPid, SIGUSR2);
            pause();
            act.sa_handler = SIG_IGN;
            sigaction(SIGUSR1, &act, NULL);
            simulate2();
            exit(0);
        }
        while(counter_handler < 25);
        if(counter_handlerSIGUSR1 == 0){
            printf("Inefficient algorithm!");
            for(i = 0; i < NUMBER_OF_CHILDREN; i++){
                kill(pidList[i], SIGKILL);
            }
        }
        else{
            for(i = 0; i < NUMBER_OF_CHILDREN; i++){
                kill(pidList[i], SIGUSR1);
            }
        }
    }
    return 0;
}