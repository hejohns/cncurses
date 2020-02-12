#include "macros.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <signal.h>
#include <curses.h>
#include "cncurses.h"
#include "screen_buffer.h"
#include "cstring.h"


bool cwinch = false;

void cinit(int num, ...){
    if(num <0) panic2("Number of windows must be greater than 0", EXIT_FAILURE);
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
        arg->cerase = &screen_buffer_erase;
        arg->repaint = &screen_buffer_repaint;
        arg->free = &screen_buffer_free;
        arg->dim[0] /= cROWS;
        arg->dim[1] /= cCOLS;
        arg->dim[2] /= cROWS;
        arg->dim[3] /= cCOLS;
        arg->queue = calloc(SCREEN_BUFFER_QUEUE_INITIAL, sizeof(char));
        if(arg->queue == NULL) panic2("failed to allocate memory for screen_buffer.queue", EXIT_FAILURE);
        arg->rows = (size_t)0;
        arg->queue_size = SCREEN_BUFFER_QUEUE_INITIAL;
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

/* ncurses wrappers */
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

static int containsDelim(char* str){
    int ret = 0;
    char dummy[2] = DELIM;
    for(unsigned int i=0; i<strlen(str); i++){
        if(str[i] == dummy[0]){
            ret++;
        }
    }
    return ret;
}

void cwprintw(screen_buffer* win, char* str){
    call2(win, push, str);
}
