#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(void){
    int i;
    typedef struct{
        char cmd[32]; // Command name.
        int time_cap; // Time limit to complete (in seconds).
    } command_t;

    command_t cmds[5];
    strcpy(cmds[0].cmd, "cat filename1 filename2>file2&3");
    cmds[0].time_cap = 5;
    strcpy(cmds[1].cmd, "ls -R");
    cmds[1].time_cap = 13;
    strcpy(cmds[2].cmd, "mv file.txt /home");
    cmds[2].time_cap = 12;
    strcpy(cmds[3].cmd, "cp scenery.jpg /home");
    cmds[3].time_cap = 8;
    strcpy(cmds[4].cmd, "rm -r");
    cmds[4].time_cap = 11;
    
    for(i = 0; i < 5; i++){
        printf("Comando %s em execução...\n", cmds[i].cmd);
        sleep(10);
        printf("Execution ended!\n");
    }
    
    return 0;
}