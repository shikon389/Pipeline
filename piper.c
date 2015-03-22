#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "shell.h"

//char **cmd[] = {{ "ls", 0}, {"grep", ".c", 0}, {"grep", "s", 0}};
int pipes[50][2];

void runOne(char **cmd){
    switch(fork()){
        /* child */
        case 0:
            execvp(cmd[0], cmd);
            break; 

        /* error */
        case -1:
            perror("fork");
            break; 

        /* parent */
        default: 
            break; 
    }
}

void runFirst(char **cmd, int fd[]){
    switch(fork()){
        /* child */
        case 0: 
            dup2(fd[1], 1);
            close(fd[0]);
            close(fd[1]);
            execvp(cmd[0], cmd);
            perror(cmd[0]);
            break; 

        /* error */
        case -1: 
            perror("fork");
            break; 

        /* parent */
        default: 
            break; 
    }
}

void runMiddle(char **cmd, int fd1[], int fd2[]){
    switch(fork()){
        /* child */
        case 0: 
            dup2(fd1[0], 0);
            dup2(fd2[1], 1);
            close(fd1[0]);
            close(fd1[1]);
            close(fd2[0]);
            close(fd2[1]);
            execvp(cmd[0], cmd);
            perror(cmd[0]);
            break; 

        /* error */
        case -1: 
            perror("fork");
            break; 

        /* parent */
        default: 
            break; 
    }
     
}

void runLast(char **cmd, int fd[]){
    switch(fork()){
        /* child */
        case 0: 
            dup2(fd[0], 0);
            close(fd[1]);
            close(fd[0]);
            execvp(cmd[0], cmd);
            perror(cmd[0]);
            break; 

        /* error */
        case -1: 
            perror("fork");
            break; 

        /* parent */
        default: 
            break; 
    }
}

int piper(char **cmds[], int numCommands){
    /* no commands */
    if(numCommands == 0){
        return 0;
    }

    /* one command */
    if(numCommands == 1){
        runOne(cmds[0]);
    }

    int pid, status;

    /* need to pipe two commands */
    if(numCommands == 2){
        /* create pipe */
        pipe(pipes[0]); 

        /* run commands */
        runFirst(cmds[0], pipes[0]);
        runLast(cmds[1], pipes[0]);

        /* close the pipes */
        close(pipes[0][0]);
        close(pipes[0][1]);

        /* pick up zombie processes */
        while((pid = wait(&status)) != -1){ 
            fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
        }
        exit(0);
    }

    /* 
     * more than 2 commands to pipe, must use
     * runMiddle() and pipe multiple fds together 
     */
    if(numCommands > 2){
        /* number of fds we need to pipe */
        /* 3 commands = 2 pipes 
         * 4 commands = 3 pipes
         * 5 commands = 4 pipes
         * x commands = (x-1) pipes
         */
        int counter = 0; 
        for(; counter<numCommands-1; counter++){
            pipe(pipes[counter]);
        }


        /*
        pipe(pipes[0]);
        pipe(pipes[1]);
        */


        /* keeps track of the current pipe */
        int p_counter = 0; 

        /* execute the first command */
        runFirst(cmds[0], pipes[0]);

        /* execute the middle commands */
        for(counter = 1; counter < numCommands-1; counter++){
            runMiddle(cmds[counter], pipes[p_counter], pipes[p_counter+1]);
            p_counter = p_counter + 1; 
            close(pipes[p_counter-1][0]);
            close(pipes[p_counter-1][1]);
        }

        /* have to run the last command */
        runLast(cmds[counter], pipes[p_counter]);
        close(pipes[p_counter][0]);
        close(pipes[p_counter][1]);

        /* pick up zombie processes */
        while((pid = wait(&status)) != -1){ 
            fprintf(stderr, "process %d exits with %d\n", pid, WEXITSTATUS(status));
        }
    }

    return 1;
}
/*
int main(int argc, char **argv){
    char ***cmds = (char ***)calloc(4, sizeof(char **));
    cmds[0] = (char **)calloc(3, sizeof(char *));
    cmds[1] = (char **)calloc(3, sizeof(char *));
    cmds[2] = (char **)calloc(3, sizeof(char *));
    cmds[3] = (char **)calloc(3, sizeof(char *));

    // set the first command 
    cmds[0][0] = "cat";
    cmds[0][1] = "t";
    cmds[0][2] = 0;

    // set the second command 
    cmds[1][0] = "grep";
    cmds[1][1] = "at";
    cmds[1][2] = 0;

    // set the third command 
    cmds[2][0] = "grep";
    cmds[2][1] = "cat";
    cmds[2][2] = 0;

    // set the fourth command 
    cmds[3][0] = "grep";
    cmds[3][1] = "cata";
    cmds[3][2] = 0;

    piper(cmds, 4);
}*/
