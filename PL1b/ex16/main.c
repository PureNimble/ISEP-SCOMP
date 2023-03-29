#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>

#define NUMBER_OF_CHILDREN 50

volatile sig_atomic_t counter_SIGUSR1 = 0;
volatile sig_atomic_t counter_handler = 0;

void handleFather(int signo, siginfo_t *sinfo, void *context){
    if(signo == SIGUSR1){
        counter_SIGUSR1++;
    }
    counter_handler++;
}

void handleChild(int signo, siginfo_t *sinfo, void *context){
    simulate2();
    exit(0);
}

int simulate1(){
    int n, result;
    time_t t;
    /* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand ((unsigned) time(&t));

    /* initialize n */
    n = rand() % 10 + 1;
    result = rand() % 100;

    if (result > 35){
        result = 0;
    }
    else result = 1;

    sleep(n);
    return result;
}

int simulate2(){
    int n, result;
    time_t t;
    /* intializes RNG (srand():stdlib.h; time(): time.h) */
    srand ((unsigned) time(&t));

    /* initialize n */
    n = rand() % 10 + 1;
    result = rand() % 1;

    sleep(n);
    return result;
}

void childSignalSender(pid_t pidList[], int signal){
    int i;
    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        kill(pidList[i], signal);
    }
}

int main(void){
    int i;
    pid_t pidList[NUMBER_OF_CHILDREN], parentPid = getpid();

    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);

    for(i = 0; i < NUMBER_OF_CHILDREN; i++){
        pidList[i] = fork();
        if(pidList[i] < 0){
            perror("Erro ao criar o processo");
            exit(-1);
        }
        if(pidList[i] == 0){
            act.sa_sigaction = handleChild;
            sigaction(SIGUSR1, &act, NULL);

            if(simulate1() == 1){
                kill(parentPid, SIGUSR1);
            }
            else kill(parentPid, SIGUSR2);
            pause();
            exit(-1);
        }
    }
    act.sa_sigaction = handleFather;
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act, NULL);
    while(counter_handler < 25);
    if(counter_SIGUSR1 == 0){
        printf("Inefficient algorithm!\n");
        childSignalSender(pidList, SIGKILL);
    }else{
        childSignalSender(pidList, SIGUSR1);
        while(wait(NULL) > 0);
        printf("Sucesso!\n");
    }
    return 0;
}