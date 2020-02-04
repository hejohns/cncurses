#include "macros.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "cncurses.h"
#include "screen_buffer.h"


bool cwinch = false;

void cinit(int num, ...){
    if(num <0){
        panic("Number of windows must be greater than 0", EXIT_FAILURE);
    }
    va_list args;
    va_start(args, num);
    for(int i=0; i<num; i++){
        screen_buffer* arg = va_arg(args, screen_buffer*);
        getmaxyx(arg->ptr, arg->dim[0], arg->dim[1]);
        getbegyx(arg->ptr, arg->dim[2], arg->dim[3]);
        arg->push = &screen_buffer_push;
        arg->pop = &screen_buffer_pop;
        arg->size = &screen_buffer_size;
        arg->at = &screen_buffer_at;
        arg->cclear = &screen_buffer_clear;
        arg->erase = &screen_buffer_erase;
        arg->repaint = &screen_buffer_repaint;
        arg->dim[0] /= cROWS;
        arg->dim[1] /= cCOLS;
        arg->dim[2] /= cROWS;
        arg->dim[3] /= cCOLS;
        arg->rows = (size_t)0;
    }
    va_end(args);
}

void sigwinch_handler(){
    cwinch = true;
}

void sigwinch_initialize(){
    sigset_t sigwinch_mask;
    sigemptyset(&sigwinch_mask);
    const struct sigaction sigwinch_action = {
	.sa_handler = &sigwinch_handler,
	.sa_mask = sigwinch_mask,
	.sa_flags = 0
    };
    if(sigaction(SIGWINCH, &sigwinch_action, NULL)){
	panic("Failed to set SIGWINCH handler", EXIT_FAILURE);
    }
}

//ncurses wrappers

void cresizeterm(int num, ...){
    if(cwinch){
        endwin();
        refresh();
        getmaxyx(stdscr, cROWS, cCOLS);
        resizeterm(cROWS, cCOLS);
        va_list args;
        va_start(args, num);
        for(int i=0; i<num; i++){
            screen_buffer* arg = va_arg(args, screen_buffer*);
            wclear(arg->ptr);
            cwattroff((*arg), COLOR_PAIR(i+1));
            delwin(arg->ptr);
            arg->ptr = newwin((int)(arg->dim[0]*cROWS), (int)(arg->dim[1]*cCOLS), (int)(arg->dim[2]*cROWS), (int)(arg->dim[3]*cCOLS));
            cwattron((*arg), COLOR_PAIR(i+1));
            wrefresh(arg->ptr);
        }
        va_end(args);
        cwinch = false;
    }
    else{
        //do nothing
    }
}

void cwprintw(screen_buffer* win, char* str){
}
