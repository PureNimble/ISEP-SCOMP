#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    
#include <sys/types.h>
#include <sys/wait.h>


int main()
{
  pid_t p;
  if (fork() == 0) {
    printf("PID = %d\n", getpid()); exit(0);
  }
  if ((p=fork()) == 0) {
    printf("PID = %d\n", getpid()); exit(0);
  }

  printf("Parent PID = %d\n", getpid());

  printf("Waiting... (for PID=%d)\n",p);
  waitpid(p, NULL, 0);

  printf("Enter Loop...\n");
  while (1); /* Infinite loop */
} 

/*

a)
    Q: In the shell, list the running processes with ps (look for the PIDs printed). Are all parent and child processes
       listed? Why?
    A: Não, na lista de processos apenas aparece o processo pai e o primeiro processo que se transformou num processo zombie, ou seja,
       terminou antes do seu processo pai e este (ainda a executar) não reparou.
       Quanto ao processo que não aparece (processo 2), o seu desaparecimento da lista de processos em execução deve-se ao facto de este 
       já ter terminado a sua execução e, por isso mesmo, a sua informação ficou ao encargo do processo pai.


       Parent PID = 84
       PID = 85
       Waiting... (for PID=86)
       PID = 86
       Enter Loop...

       -------------------------------------------------------------//---------//------------------------------------------------------------

       PID TTY          TIME CMD
       72 pts/0    00:00:00 bash
       83 pts/0    00:00:00 make
       84 pts/0    00:00:03 main
       85 pts/0    00:00:00 main <defunct>
       87 pts/0    00:00:00 ps


b) 
    Q: Is there anything particular about the child process? Explain. 
    A: Sim, tal como disse na resposta anterior, o processo 1 transformou-se num processo zombie, o que significa que terminou primeiro do que
       o processo pai e este, que ainda estava em execução, não reparou.
       

c) 
    Q: Kill the parent process: kill -KILL <PID>
    A: make: *** [Makefile:11: run] Killed

       ps
          PID TTY          TIME CMD
           72 pts/0    00:00:00 bash
          117 pts/0    00:00:00 ps
       [1]+  Exit 2                  make run

*/