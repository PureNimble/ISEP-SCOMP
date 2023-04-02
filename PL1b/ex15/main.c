#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

typedef struct{
    char cmd[32]; // Command name.
    int time_cap; // Time limit to complete (in seconds).
} command_t;

command_t *cur_cmd = NULL;

void handler(int signo, siginfo_t *sinfo, void *context){
    char msg [85];
    sprintf(msg, "The command %s did not complete in its allowed time!\n", cur_cmd->cmd);
    write(STDOUT_FILENO, msg, strlen(msg));
}

int main(void){
    pid_t pid;
    pid = fork();
    if(pid == 0){
        execv("./simulator", (char *[]){"cat filename1 filename2>file2&3", "ls -R", "mv file.txt /home", "cp scenery.jpg /home", "rm -r", NULL });
        exit(-1);
    }
    int i, remainingTime;
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handler;
    sigaction(SIGALRM, &act, NULL);
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
        cur_cmd = &cmds[i];
        alarm(cur_cmd->time_cap);
        sleep(10);
        remainingTime = alarm(0);
        if(remainingTime > 0){
            sleep(remainingTime);
        }
    }
    
    return 0;
}