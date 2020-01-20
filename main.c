//main.c
#include "macros.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "misc.h"
#include "curtex.h"

screen_buffer buffer;

int main(int argc, char** argv){
    //begin{initialization}}
    pid_t forkreturn1 = fork();
    pid_t forkreturn2;
    if(forkreturn1 < 0){
        panic("fork1 failed", EXIT_FAILURE);
    }
    else if(forkreturn1 == 0){
        return 0;
    }
    else{
        forkreturn2 = fork();
        if(forkreturn2 < 0){
            panic("fork2 failed", EXIT_FAILURE);
        }
        else if(forkreturn2 == 0){
            return 0;
        }
        else{
            goto parent;
        }
    }
parent:
    waitpid(forkreturn1, NULL, WNOHANG);
    waitpid(forkreturn2, NULL, WNOHANG);
    sigwinch_initialize();
    initscr();
    START_COLOR();
    cinit_pair(1, COLOR_GREEN, COLOR_BLACK);
    cattron(COLOR_PAIR(1));
    getmaxyx(stdscr, ROWS_, COLS_);
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    clear();
    //end{initialization}
    //begin{body]
    move(30, 30);
    refresh();
    vline('|', 20);
    refresh();
    getch();
    while(true){}
    cmove_p(050,050);
    cgetyx();
    cprintw_1(%s,hello);
    cmove_r(101,100);
    cprintw_1(%s,cruel);
    cmove_r(102,100);
    cprintw_1(%s,world);
    buffer.repaint();
    refresh();
    echo();
    char buf1[10];
    char buf2[20] = "021,050,050,%s,";
    getstr(buf1);
    (buffer.clear)();
    strcat(buf2, buf1);
    buffer.push(buf2);
    while(true){}
    //end{body}}
    //begin{termination}
    catroff(COLOR_PAIR(1));
    endwin();
    return EXIT_SUCCESS;
    //end{termination}
}
