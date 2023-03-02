#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    fork(); fork(); fork();
    printf("SCOMP!\n");
} 

/*
a)  
    Q: How many processes are created by this code?
    A: O processo pai cria 7 processos filhos.

b)
    Q: Draw a process tree that describes the processes created.
    A: **Ver imagem anexada dentro da pasta do ex03 (alinea_b_ex03)**

c )
    Q: How many times is “SCOMP” printed?
    A: "SCOMP" é imprimido 8 vezes, pois existem 8 processos diferentes.
*/