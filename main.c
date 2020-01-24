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
#include "cncurses.h"

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
    cstart_color();
    cinit_pair(1, COLOR_GREEN, COLOR_BLACK);
    cattron(COLOR_PAIR(1));
    getmaxyx(stdscr, ROWS_, COLS_);
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    clear();
    refresh();
    WINDOW* myWin = newwin(20,20, 10, 10);
    //box(myWin, 0, 0);
    //wmove(myWin, 1, 1);
    wprintw(myWin, "Helloooooooo\nWOOOOORRRLF");
    wrefresh(myWin);
    //end{initialization}
    //begin{body]
    //cmove(0,0);
    //cvline('|', 20);
    //cmove_r(0, 1);
    //cvline('|', 20);
    //cmove_r(0, 2);
    //cvline('|', 20);
    //cmove_p(.50,.50);
    ////cgetyx();
    //cprintw("%s\n%s\n%s", "Hello", "Cruel", "World");
    //cprintw("%d-%d,", 10, 20);
    //cprintw("%f-%f", .100, .200);
    //crefresh();
    refresh();
    echo();
    //cclear();
    while(true){}
    //end{body}}
    //begin{termination}
    cattroff(COLOR_PAIR(1));
    endwin();
    return EXIT_SUCCESS;
    //end{termination}
}
