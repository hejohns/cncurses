//main.c
#include "macros.h"
#include <curses.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "misc.h"
#include "curtex.h"

screen_buffer buffer;

int main(int argc, char** argv){
    //begin{initialization}}
    sigwinch_initialize();
    initscr();
    START_COLOR();
    INIT_PAIR(1, COLOR_GREEN, COLOR_BLACK);
    ATTRON(COLOR_PAIR(1));
    getmaxyx(stdscr, ROWS_, COLS_);
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    clear();
    //end{initialization}
    //begin{body]
    cmove_p(050,050);
    cgetyx();
    cprintw_1(%s,hello);
    cmove_r(101,100);
    cprintw_1(%s,cruel);
    cmove_r(102,100);
    cprintw_1(%s,world);
    buffer.repaint();
    refresh();
    getch();
    while(true){}
    //end{body}}
    //begin{termination}
    ATTROFF(COLOR_PAIR(1));
    endwin();
    return EXIT_SUCCESS;
    //end{termination}
}
