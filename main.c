//main.c
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
    getmaxyx(stdscr, ROWS_, COLS_);
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    clear();
    //end{initialization}
    //begin{body]
    buffer.push("020,050,050");
    buffer.push("001");
    buffer.push("011,%s,Hello");
    buffer.push("019,101,100");
    buffer.push("011,%s,cruel");
    buffer.push("019,102,100");
    buffer.push("011,%s,world");
    buffer.repaint();
    refresh();
    getch();
    while(true){}
    //end{body}}
    //begin{termination}
    endwin();
    return EXIT_SUCCESS;
    //end{termination}
}
