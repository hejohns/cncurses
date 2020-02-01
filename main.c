//main.c
#include "macros.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
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
        fclose(stdin);
        //execlp("gdb", "gdb", (char*)NULL);
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
            fclose(stdin);
            //execlp("gdb", "gdb", (char*)NULL);
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
    getmaxyx(stdscr, cROWS, cCOLS);
    timeout(10);
    cstart_color();
    cinit_pair(1, COLOR_GREEN, COLOR_BLACK);
    cinit_pair(2, COLOR_BLUE, COLOR_BLACK);
    cinit_pair(3, COLOR_WHITE, COLOR_BLACK);
    WINDOW* win1 = cnewwin(1,.5, 0, 0);
    WINDOW* win2 = cnewwin(.5, .5, 0, .5);
    WINDOW* win3 = cnewwin(.5, .5, .5, .5);
    cinit(3, win1, win2, win3);
    cwattron(1, COLOR_PAIR(1));
    cwattron(2, COLOR_PAIR(2));
    cwattron(3, COLOR_PAIR(3));
    //getmaxyx(stdscr, cROWS, cCOLS);
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    clear();
    refresh();
    cwprintw(1, "%s", "WIN1");
    cwprintw(2, "%s", "WIN2");
    cwprintw(3, "%s", "WIN3");
    /*
    box(win1, 0, 0);
    box(win2, 0, 0);
    box(win3, 0, 0);
    wrefresh(win1);
    wrefresh(win2);
    wrefresh(win3);
    wclear(win1);
    wrefresh(win1);
    */
    cwinch = true;
    buffer.repaint();
    //end{initialization}
    //begin{body]
    int ch;
    flushinp();
    while((ch = getch()) != KEY_RIGHT){
        switch(ch){
            case KEY_LEFT:
                wclear(win1);
                wprintw(win1, "hi");
                wrefresh(win1);
                break;
            case KEY_UP:
                raise(SIGWINCH);
                //cwinch = true;
                break;
            case ERR:
                //cwprintw(2, "%s", "9");
                cwinch = true;
                buffer.repaint();
                break;
            default:;
                char str[2];
                str[0] = ch;
                str[1] = '\0';
                if(isascii(ch)){
                    cwprintw(3, "%s", str);
                }
                //refresh();
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
    cwattroff(3, COLOR_PAIR(3));
    endwin();
    return EXIT_SUCCESS;
    //end{termination}
}
