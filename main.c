//main.c
#include <curses.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "misc.h"
#include "curtex.h"

int main(int argc, char** argv){
    //begin{initialization}}
    sigwinch_initialize();
    initscr();
    getmaxyx(stdscr, ROWS_, COLS_);
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    clear();
    //end{initialization}
    //begin{body]
    mvprintw(ROWS_/2, (COLS_-6)/2, "%d-%d", ROWS_, COLS_);
    refresh();
    getch();
    while(true){}
    //end{body}}
    //begin{termination}
    endwin();
    return EXIT_SUCCESS;
    //end{termination}
}
