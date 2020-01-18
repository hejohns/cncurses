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


/* REQUIRES: yes
 * MODIFIES: yes
 * EFFECTS: yes
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

/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: none
 *
 * NOTES:
 */
void screen_buffer_push(char* command);

/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: none
 *
 * NOTES:
 */
void screen_buffer_pop(char* command);

/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: none
 *
 * NOTES:
 */
size_t screen_buffer_size();

/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: none
 *
 * NOTES:
 */
char* screen_buffer_peek();

/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: none
 *
 * NOTES:
 */
void screen_buffer_clear();

/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: none
 *
 * NOTES:
 */
void screen_buffer_erase(size_t index);

typedef struct{
    //public
    void (*push)(char*);
    void (*pop)(char*);
    size_t (*size)();
    char* (*peek)();
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
