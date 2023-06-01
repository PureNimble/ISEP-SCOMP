#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void* primeNum(void* arg) {

    int value = *((int*) arg);
    int i, j, prime;

    printf("Thread [%lu] em execução...\n", pthread_self());

    if (value > 1){
        for (i = 2; i <= value; i++) {
            prime = 1;

            for (j = 2; j * j <= i && prime; j++) {
                prime = (i % j != 0);
            }

            if (prime) {
                printf("Número %d é primo\n", i);
            }
        }
    } else printf("Nenhum número primo encontrado\n");

    printf("Fim de execução!\n");

    pthread_exit(NULL);
}

int main(void){
    
    int num;
    pthread_t thread;

    printf("Insira um número: ");
    scanf("%d", &num);

    if (pthread_create(&thread, NULL, primeNum, (void*) &num) != 0) {
        perror("Erro ao criar thread");
        exit(-1);
    }

    if (pthread_join(thread, NULL) != 0) {
        perror("Erro ao dar join das threads");
        exit(-1);
    }

    return 0;
}