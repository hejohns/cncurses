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


void screen_buffer_push(screen_buffer* win, char* cmd);
char* screen_buffer_pop(screen_buffer* win, char*);
size_t screen_buffer_size(screen_buffer* win);
char* screen_buffer_at(screen_buffer* win, size_t index);
void screen_buffer_clear(screen_buffer* win);
void screen_buffer_erase(screen_buffer* win, size_t index);
void screen_buffer_repaint(screen_buffer* win);


#endif /* SCREEN_BUFFER_H */
