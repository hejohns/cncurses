//curtex.h
#ifndef CURTEX_H
#define CURTEX_H

#include <curses.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//globals
sigset_t sigwinch_mask;
int ROWS_, COLS_;


/* REQUIRES: valid buffer
 * MODIFIES: empties buffer
 * EFFECTS: paints screen from buffer
 *
 * NOTES: Repaints screen if SIGWINCH interrupt received.
 */
void sigwinch_handler();

/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: sets sigwinch_handler as SIGWINCH handle. On failure, exits w/ error code.
 *
 * NOTES: Run at beginning of main.
 */
void sigwinch_initialize();

/* REQUIRES: valid command
 * MODIFIES: none
 * EFFECTS: pushes command into buffer queue
 *
 * NOTES: command format- 999,...
 * first three digits are command code, followed by comma seperated args
 */
void screen_buffer_push(char* command);

/* REQUIRES: dest pointer
 * MODIFIES: overwrites dest with first line of buffer
 * EFFECTS: pops front off queue and returns
 *
 * NOTES: not intended to be used directly
 */
char* screen_buffer_pop(char* dest);

/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: returns size of buffer. size--
 *
 * NOTES:
 */
size_t screen_buffer_size();

/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: returns front of queue
 *
 * NOTES:
 */
char* screen_buffer_peek(char* dest);

/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: none
 *
 * NOTES:
 */
void screen_buffer_clear();

/* REQUIRES: valid index
 * MODIFIES: none
 * EFFECTS: size--
 *
 * NOTES:
 */
void screen_buffer_erase(size_t index);

typedef struct{
    //public
    void (*push)(char*);
    char* (*pop)(char*);
    size_t (*size)();
    char* (*peek)(char*);
    void (*clear)();
    void (*erase)(size_t);
    //private
    char queue[80][80];
    size_t rows;
} screen_buffer;

screen_buffer buffer={
    .push=&screen_buffer_push,
    .pop=&screen_buffer_pop,
    .size=&screen_buffer_size,
    .peek=&screen_buffer_peek,
    .clear=&screen_buffer_clear,
    .erase=&screen_buffer_erase,
    .rows=(size_t)0
};
#endif /* CURTEX_H */
