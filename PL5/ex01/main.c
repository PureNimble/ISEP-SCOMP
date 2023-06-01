#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int main(void) {
    pid_t pid = fork();

    if (pid == 0) {
        fork();
        pthread_t thread_id;
        pthread_create (&thread_id , NULL, thread_func, NULL);
        pthread_join (thread_id, NULL);
    }
    fork();

    return 0;
}

/*
    Excluding the main process and thread:

    (a)
        Q: How many processes are created? Justify your answer.
        A: São criados 5 processos.
           Pai (cria 2 processos):
               O processo pai cria apenas 2 novos processos filho nas linhas 7 e 15
               uma vez que este não corresponde à condição (pid == 0), não executando 
               por isso este bloco de código.

           Filho (cria 2 processos):
                 O processo filho começa a sua execução a partir da linha 8. Uma vez que 
                 este realiza a condição (pid == 0), vai executar este bloco de código criando
                 um novo processo na linha 10. No final cria um novo processo na linha 15.

           "Neto" (cria 1 processo):
                 O processo "Neto" começa a sua execução a partir da linha 11. Cria um 
                 processo filho na linha 15.


        
    (b)
        Q: How many threads are created? Justify your answer.
        A: São criadas 2 threads no bloco de código if(pid == 0).
           - Filho cria 1 thread.
           - "Neto" cria 1 thread.
*/