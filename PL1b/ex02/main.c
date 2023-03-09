#include <stdio.h>
#include <unistd.h>

int main(){
    for(;;){
        printf("I love SCOMP! \n");
        sleep(1);
    }
}
/*  
a) 
    Q: On one terminal, compile and run the program.
    A: 
       Para compilar e executar o programa utilizam-se os seguintes comandos no diretório do source:
       Compilar: gcc -Wall main.c -o main
       Executar: ./main

b)
    Q: Open another terminal, and type the command ps to discover the PID of the process in which
       the program displaying the “I love SCOMP!” message is executing.
    A:
       Comando para listar todos os processos: ps -A
       Output: 
                PID TTY          TIME CMD
                1  ?        00:00:00 init
                8  tty1     00:00:00 init
                9  tty1     00:00:00 bash
                25 tty1     00:00:00 main   <-- (processo referente ao programa)
                26 tty2     00:00:00 init
                27 tty2     00:00:00 bash
                38 tty2     00:00:00 ps

c)
    Q: Type the command kill -s STOP target˙PID to send the STOP signal and suspend that process.
       Note: target˙PID is the PID of the process that will receive the signal.
    A: 
       Comando: kill -s STOP 25
       Mensagem que apareceu no terminal onde estava a executar o programa: [1]+  Stopped                 ./main

d)
    Q: Type the command ps -l to check that the process became “stopped” (status should be “T”).
    A:
       Comando para listar todos os processos: ps -A -l
       Output:
                F S   UID   PID  PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
                0 S     0     1     0  0  80   0 -  2238 ?      ?        00:00:00 init
                0 S     0     8     1  0  80   0 -  2238 -      tty1     00:00:00 init
                0 S  1000     9     8  0  80   0 -  3562 -      tty1     00:00:00 bash
                0 T  1000    25     9  0  80   0 -  2703 -      tty1     00:00:00 main   <-- (processo referente ao programa) (stopped)
                0 S     0    26     1  0  80   0 -  2238 -      tty2     00:00:00 init
                0 S  1000    27    26  0  80   0 -  3562 -      tty2     00:00:00 bash
                0 R  1000    39    27  0  80   0 -  3880 -      tty2     00:00:00 ps

e)
    Q: Send the signal CONT such the process resumes its normal execution. Check that the process left  
       the “stopped” state.
    A:
       Comando: kill -s CONT 25
       Comando para listar todos os processos: ps -A -l
       Output:
                F S   UID   PID  PPID  C PRI  NI ADDR SZ WCHAN  TTY          TIME CMD
                0 S     0     1     0  0  80   0 -  2238 ?      ?        00:00:00 init
                0 S     0     8     1  0  80   0 -  2238 -      tty1     00:00:00 init
                0 S  1000     9     8  0  80   0 -  3562 -      tty1     00:00:00 bash
                0 S  1000    25     9  0  80   0 -  2703 -      tty1     00:00:00 main   <-- (processo referente ao programa) (resumed)
                0 S     0    26     1  0  80   0 -  2238 -      tty2     00:00:00 init
                0 S  1000    27    26  0  80   0 -  3562 -      tty2     00:00:00 bash
                0 R  1000    40    27  0  80   0 -  3880 -      tty2     00:00:00 ps
*/