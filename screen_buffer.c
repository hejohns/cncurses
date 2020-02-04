//screen_buffer.c

#include "macros.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "screen_buffer.h"


void screen_buffer_push(screen_buffer* win, char* cmd){
    if(screen_buffer_size(win) >= BUFFER_ROWS_MAX){
        panic("buffer is full", EXIT_FAILURE);
    }
    if(strlen(cmd) >= BUFFER_COLS_MAX){
        panic("command too long", EXIT_FAILURE);
    }
    strcpy(win->queue[win->rows], cmd);
    win->rows++;
}

char* screen_buffer_pop(screen_buffer* win, char* dest){
    if(screen_buffer_size(win) == 0){
        panic("buffer is empty--nothing to pop", EXIT_FAILURE);
    }
    dest = strncpy(dest, screen_buffer_at(win, screen_buffer_size(win)-1), BUFFER_COLS_MAX);
    win->rows--;
    return dest;
}

size_t screen_buffer_size(screen_buffer* win){
    if(win->rows >= BUFFER_ROWS_MAX){
        panic("buffer corrupted", EXIT_FAILURE);
    }
    return win->rows;
}

char* screen_buffer_at(screen_buffer* win, size_t index){
    if(index >= screen_buffer_size(win)){
        panic("index out of range", EXIT_FAILURE);
    }
    return win->queue[index];
}

void screen_buffer_clear(screen_buffer* win){
    win->rows = (size_t)0;
}

void screen_buffer_erase(screen_buffer* win, size_t index){
    if(index >= screen_buffer_size(win)){
        panic("index out of range", EXIT_FAILURE);
    }
    for(size_t i=index+1; i<screen_buffer_size(win); i++){
        strncpy(screen_buffer_at(win, i-1), screen_buffer_at(win, i), BUFFER_COLS_MAX);
    }
}

int opcode(char* row){
    return 100*(row[0]-'0') + 10*(row[1]-'0') + (row[2]-'0');
}

void screen_buffer_repaint(screen_buffer* win){
    wclear(win->ptr);
    for(size_t i=0; i<call(*win, size); i++){
        switch(opcode(call2(*win, at, i))){
            case 0:;
                break;
            case 1:;
                break;
            default:
                panic("opcode invalid", EXIT_FAILURE);
                break;
        }
    }
    wrefresh(win->ptr);
}


