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
        fclose(stdout);
        fopen("/dev/null", "w");
        fclose(stderr);
        fopen("/dev/null", "w");
        execlp("gdb", "gdb", (char*)NULL);
        return 0;
    }
    else{
        forkreturn2 = fork();
        if(forkreturn2 < 0){
            panic("fork2 failed", EXIT_FAILURE);
        }
        else if(forkreturn2 == 0){
            fclose(stdout);
            fopen("/dev/null", "w");
            fclose(stderr);
            fopen("/dev/null", "w");
            execlp("gdb", "gdb", (char*)NULL);
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
    timeout(-1);
    cstart_color();
    cinit_pair(1, COLOR_GREEN, COLOR_BLACK);
    cinit_pair(2, COLOR_BLUE, COLOR_BLACK);
    WINDOW* win1 = newwin(10,20, 5, 10);
    WINDOW* win2 = newwin(10, 20, 5, 40);
    cinit(2, win1, win2);
    //cinit(1, win1);
    cwattron(1, COLOR_PAIR(1));
    cwattron(2, COLOR_PAIR(2));
    getmaxyx(stdscr, cROWS, cCOLS);
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    clear();
    refresh();
    cwprintw(1, "%s", "Helloooooooo\n|WOOOOORRRLF");
    cwprintw(2, "%s", "YELLLLLOOO\n|WOOOOORRRLF");
    cwinch = true;
    buffer.repaint();
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
    echo();
    //cclear();
    int ch;
    while((ch = getch()) != KEY_RIGHT){
        switch(ch){
            case KEY_LEFT:
                wclear(win1);
                wprintw(win1, "hi");
                wrefresh(win1);
                break;
            case KEY_UP:
                cwattroff(1, COLOR_PAIR(1));
                delwin(win1);
                win1=newwin(10,20,5,10);
                cwattron(1, COLOR_PAIR(1));
                cwinch = true;
                break;
            default:
                refresh();
                //ungetch(ch);
                //buffer.repaint();
                //ch = wgetch(win1);
                break;
        }
        //buffer.repaint();
    }
    //end{body}}
    //begin{termination}
    cwattroff(1, COLOR_PAIR(1));
    cwattroff(2, COLOR_PAIR(2));
    endwin();
    return EXIT_SUCCESS;
    //end{termination}
}
