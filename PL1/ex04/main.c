#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    int a=0, b, c, d;
    b = (int) fork();
    c = (int) getpid(); /* getpid(), getppid(): unistd.h*/
    d = (int) getppid();
    a = a + 5;
    printf("\na=%d, b=%d, c=%d, d=%d\n",a,b,c,d);
} 

/*
a)  Q: Which of the variables a, b, c and d will have the same value in both processes?
    A: Variável a.

b)  Q: Draw a process tree that describes the processes created.
    A: **Ver imagem anexada dentro da pasta do ex04 (alinea_b_ex04)**

*/