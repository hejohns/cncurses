//screen_buffer.h
#ifndef SCREEN_BUFFER_H
#define SCREEN_BUFFER_H

#include "macros.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "cncurses.h"


struct screen_buffer;
typedef struct screen_buffer{
    /* "public" */
    void (*push)(struct screen_buffer*, char*);
    char* (*pop)(struct screen_buffer*);
    size_t (*size)(struct screen_buffer*);
    char* (*at)(struct screen_buffer*, int);
    // named cclear because clear is expanded as a ncurses macro
    void (*cclear)(struct screen_buffer*);
    // named cerase because erase is expanded as a ncurses macro
    void (*cerase)(struct screen_buffer*, size_t);
    void (*repaint)(struct screen_buffer*);
    void (*free)(struct screen_buffer*);
    WINDOW* ptr;
    // y, x, beg_y, beg_x
    double dim[4];
    /* "private" */
    char* queue;
    size_t queue_size;
    size_t rows;
} screen_buffer;

void screen_buffer_push(screen_buffer* win, char* cmd);
char* screen_buffer_pop(screen_buffer* win);
size_t screen_buffer_size(screen_buffer* win);
char* screen_buffer_at(screen_buffer* win, int index);
void screen_buffer_clear(screen_buffer* win);
void screen_buffer_erase(screen_buffer* win, size_t index);
void screen_buffer_repaint(screen_buffer* win);
void screen_buffer_free(screen_buffer* win);


#endif /* SCREEN_BUFFER_H */
