//main.c
#include "macros.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include "cncurses.h"


screen_buffer win1, win2, win3, win4;


int main(int argc, char** argv){
    //check args for proper usage
    if(argc != 2){
        printf("Usage:\n%s <name of executable to debug>\n\tExecutable must be compiled with gdb \'-g\' flag\n", argv[0]);
        return EXIT_SUCCESS;
    }
    //begin{initialization}}
    sigwinch_initialize();
    int fd1[2], fd2[2], fd3[2], fd4[4];
    if(pipe(fd1) || pipe(fd2) || pipe(fd3) || pipe(fd4)){
        panic("failed to create pipes", EXIT_FAILURE);
    }
    for(int i=0; i<2; i++){
        int flags1 = fcntl(fd1[i], F_GETFL, 0);
        flags1 = flags1 | O_NONBLOCK;
        fcntl(fd1[i], F_SETFL, flags1);
        int flags2 = fcntl(fd2[i], F_GETFL, 0);
        flags2 = flags2 | O_NONBLOCK;
        fcntl(fd2[i], F_SETFL, flags2);
        int flags3 = fcntl(fd3[i], F_GETFL, 0);
        flags3 = flags3 | O_NONBLOCK;
        fcntl(fd3[i], F_SETFL, flags3);
        int flags4 = fcntl(fd4[i], F_GETFL, 0);
        flags4 = flags4 | O_NONBLOCK;
        fcntl(fd4[i], F_SETFL, flags4);
    }
    pid_t forkreturn1 = fork();
    pid_t forkreturn2 = fork();
    if(forkreturn1 < 0){
        panic("fork1 failed", EXIT_FAILURE);
    }
    else if(forkreturn1 == 0){
        //bt
        //close unused fds
        close(fd1[0]);
        close(fd2[1]);
        close(fd3[0]);
        close(fd3[1]);
        close(fd4[0]);
        close(fd4[1]);
        fclose(stdout);
        dup2(fd1[1], 1);
        fclose(stderr);
        fopen("/dev/null", "w");
        fclose(stdin);
        dup2(fd2[0], 0);
        printf("test\n");
        execlp("gdb", "gdb", argv[1], (char*)NULL);
        return 0;
    }
    else{
        if(forkreturn2 < 0){
            panic("fork2 failed", EXIT_FAILURE);
        }
        else if(forkreturn2 == 0){
            //info b
            //close unused fds
            close(fd1[0]);
            close(fd1[1]);
            close(fd2[0]);
            close(fd2[1]);
            close(fd3[0]);
            close(fd4[1]);
            fclose(stdout);
            dup2(fd3[1], 1);
            fclose(stderr);
            fopen("/dev/null", "w");
            fclose(stdin);
            dup2(fd4[0], 0);
            execlp("gdb", "gdb", argv[1], (char*)NULL);
            return 0;
        }
        else{
            waitpid(forkreturn1, NULL, WNOHANG);
            waitpid(forkreturn2, NULL, WNOHANG);
            goto parent;
        }
    }
parent:;
#pragma GCC diagnostic ignored "-Wunused-variable"
    //close unused fds
    close(fd1[1]);
    close(fd2[0]);
    close(fd3[1]);
    close(fd4[0]);
    FILE* stdout_gdb1r = fdopen(fd1[0], "r");
    FILE* stdin_gdb1w = fdopen(fd2[1], "w");
    FILE* stdout_gdb2r = fdopen(fd3[0], "r");
    FILE* stdin_gdb2w = fdopen(fd4[1], "w");
    initscr();
    timeout(50);
    curs_set(0);
    getmaxyx(stdscr, cROWS, cCOLS);
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    cstart_color();
    //order matters; 1 means win1, 2 means win2, ...
    cinit_pair(1, COLOR_WHITE, COLOR_BLACK);
    cinit_pair(2, COLOR_BLUE, COLOR_BLACK);
    cinit_pair(3, COLOR_YELLOW, COLOR_BLACK);
    cinit_pair(4, COLOR_GREEN, COLOR_BLACK);
    //cnewwin is a macro; be careful with parenthesis
    win1.ptr = cnewwin((1-(3.0/cROWS)), .5, 0, 0);
    win2.ptr = cnewwin(.8, .5, 0, .5);
    win3.ptr = cnewwin(.2, .5, .8, .5);
    win4.ptr = cnewwin((3.0/cROWS), .5, (1-(3.0/cROWS)), 0);
    //first arg to cinit is number of windows
    cinit(4, &win1, &win2, &win3, &win4);
    cwattron(win1, COLOR_PAIR(1));
    cwattron(win2, COLOR_PAIR(2));
    cwattron(win3, COLOR_PAIR(3));
    cwattron(win4, COLOR_PAIR(4));
    clear();
    refresh();
    fprintf(stdin_gdb1w, "h\n");
    clearok(win1.ptr, true);
    clearok(win2.ptr, true);
    clearok(win3.ptr, true);
    clearok(win4.ptr, true);
    immedok(win1.ptr, true);
    immedok(win2.ptr, true);
    immedok(win3.ptr, true);
    immedok(win4.ptr, true);
    scrollok(win1.ptr, true);
    scrollok(win2.ptr, true);
    scrollok(win3.ptr, true);
    scrollok(win4.ptr, true);
    fflush(stdin_gdb1w);
    //end{initialization}
    //begin{body]
{
    //as with any tmp reg, do not assume regs will be untouched if code is not directly sequential
    size_t reg0Size = BUFFER_COLS_MAX/2;
    char* reg0 = malloc(reg0Size);
    size_t reg1Size = BUFFER_COLS_MAX/2;
    char* reg1 = malloc(reg1Size);
    int ch;
    flushinp();
    while((ch = getch()) != EXIT_KEY){
        //for testing. Will replace screen_buffer queue with less crude implementation
        if(reg0Size > BUFFER_COLS_MAX || reg1Size > BUFFER_COLS_MAX){
            panic("REG CHECK", EXIT_FAILURE);
        }
        switch(ch){
            case RESET_KEY:;
                endwin();
                system("reset");
                break;
            case KEY_LEFT:;
                char tmp[80];
                char tmp2[90];
                for(int i=0; i<153; i++){
                    fgets(tmp, 80, stdout_gdb2r);
                    sprintf(tmp2, "011"DELIM"%s", tmp);
                    call2(&win1, push, tmp2);
                    call2(&win2, push, tmp2);
                    call2(&win3, push, tmp2);
                    call2(&win4, push, tmp2);
                }
                break;
            case KEY_UP:;
                //get breakpoints
                fprintf(stdin_gdb2w, "info b\n");
                fflush(stdin_gdb2w);
                call(&win3, cclear);
                if(getline(&reg1, &reg1Size, stdout_gdb2r) != EOF)
                {
                    strcpy(reg0, reg1);
                    for(; getline(&reg1, &reg1Size, stdout_gdb2r) != EOF && reg1[0] != '('; ){
                        strcat(reg0, reg1);
                    }
                    snprintf(reg1, reg1Size, "011"DELIM"%s", reg0);
                    call2(&win3, push, reg1);
                }
                //cwprintw(win3, );
                break;
            case KEY_RIGHT:;
                break;
            case KEY_BACKSPACE:;
                break;
            case '\n':;
                break;
            case ERR:;
                break;
            default:;
                //alphanumerics
                break;
        }
    cresizeterm(4, &win1, &win2, &win3, &win4);
    call(&win1, repaint);
    call(&win2, repaint);
    call(&win3, repaint);
    call(&win4, repaint);
    }
    free(reg0);
    free(reg1);
}
    //end{body}}
    //begin{termination}
    cwattroff(win1, COLOR_PAIR(1));
    cwattroff(win2, COLOR_PAIR(2));
    cwattroff(win3, COLOR_PAIR(3));
    cwattroff(win4, COLOR_PAIR(4));
    endwin();
    if(kill(forkreturn1, SIGTERM) || kill(forkreturn2, SIGTERM)){
        kill(forkreturn1, SIGKILL);
        kill(forkreturn2, SIGKILL);
        system("reset");
        panic("Could not SIGTERM children. SIGKILLed them", EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
    //end{termination}
}
