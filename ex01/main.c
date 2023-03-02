#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    int x = 1;
    pid_t p = fork(); /*pid_t: sys/types.h; fork(): unistd.h*/
    if (p == 0) {
        x = x+1;
        printf("1. x = %d\n", x);
    } else {
        x = x-1;
        printf("2. x = %d\n", x);
    }
    printf("3. %d; x = %d\n", p, x);
} 

/*

a)
	Q: Assume that the PID of the child process is 1234. What is the output of this code in the terminal? Justify your answer.
	A: 2. x = 0
	   3. 1234; x = 0
	   1. x = 2
	   3. 0; x = 2

       -> PID do filho (1234) é retornado para o processo pai
       -> É retornado o valor zero ao processo filho
       -> Todas as variáveis do processo pai são herdadas pelo processo filho (neste caso x)
       -> Como são dois processos separados, o último print ("3. %d; x = %d\n", p, x") é executado duas vezes
       -> Em ambos o valor de x é diferente, pois, após o fork, qualquer alteração feita no x de um dos processos não 
          se refletirá no outro

b) 
    Q: Are you always guaranteed to see the output of the printf() in line 9 before the one in line 11? Explain.    
    A: Não, pois a ordem pela qual os resultados são apresentados é “aleatória”, visto que os dois processos
       estão a executar concorrentemente. Ou seja, a ordem de apresentação dos resultados pode alterar após 
       algumas execuções.
       
*/