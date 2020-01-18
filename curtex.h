//curtex.h
#ifndef CURTEX_H
#define CURTEX_H

#include <curses.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_ROWS_MAX 80
#define BUFFER_COLS_MAX 80

//globals
/* extern screen_buffer buffer */
sigset_t sigwinch_mask;
int ROWS_, COLS_;
int X, Y;

/* REQUIRES: valid buffer
 * MODIFIES: empties buffer
 * EFFECTS: paints screen from buffer
 *
 * NOTES: Repaints screen if SIGWINCH interrupt received.
 * Modifies global buffer, but does not take args since struct sigaction expects void (*func)(void)
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


/* REQUIRES: dest pointer. buffer.size()>0
 * MODIFIES: overwrites dest with first line of buffer
 * EFFECTS: pops front off queue and returns. rows shifted down 1; 
 *
 * NOTES: not intended to be used directly
 */
char* screen_buffer_pop(char* dest);


/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: returns size of buffer.
 *
 * NOTES:
 */
size_t screen_buffer_size();


/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: returns pointer to row index
 *
 * NOTES:
 */
char* screen_buffer_at(size_t index);


/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: sets rows=0. Does not overwrite existing values in queue
 *
 * NOTES: a "quick format"
 */
void screen_buffer_clear();


/* REQUIRES: valid index
 * MODIFIES: none
 * EFFECTS: size--
 *
 * NOTES:
 */
void screen_buffer_erase(size_t index);


/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: repaints terminal as stored in global buffer.
 *
 * NOTES: no
 */
void screen_buffer_repaint();


// declare and initialize screen buffer
typedef struct{
    //public
    void (*push)(char*);
    char* (*pop)(char*);
    size_t (*size)();
    char* (*at)(size_t);
    void (*clear)();
    void (*erase)(size_t);
    void (*repaint)();
    //private
    char queue[BUFFER_ROWS_MAX][BUFFER_COLS_MAX];
    size_t rows;
} screen_buffer;
extern screen_buffer buffer;

#endif /* CURTEX_H */
