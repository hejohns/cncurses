//cncurses.h
#ifndef CNCURSES_H
#define CNCURSES_H

#include <curses.h>
#include <signal.h>


// declare and initialize screen buffer
struct screen_buffer;
typedef struct screen_buffer{
    //"public"
    void (*push)(struct screen_buffer*, char*);
    char* (*pop)(struct screen_buffer*, char*);
    size_t (*size)(struct screen_buffer*);
    char* (*at)(struct screen_buffer*, size_t);
    void (*clear)(struct screen_buffer*);
    void (*erase)(struct screen_buffer*, size_t);
    void (*repaint)(struct screen_buffer*);
    WINDOW* ptr;
    // y, x, beg_y, beg_x
    double dim[4];
    //private
    char queue[BUFFER_ROWS_MAX][BUFFER_COLS_MAX];
    size_t rows;
} screen_buffer;

//globals
int cROWS, cCOLS;
extern bool cwinch;
screen_buffer win1, win2, win3, win4;

/* REQUIRES: number of arguments of type screen_buffer*
 * MODIFIES: each screen_buffer* passed is initialized and linked to WINDOW*
 * EFFECTS: none
 */
void cinit(int num, ...);


/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: sets bool cwinch to true
 */
void sigwinch_handler();


/* REQUIRES: none
 * MODIFIES: none
 * EFFECTS: sets void sigwinch_handler() as SIGWINCH handle. On failure, exits w/ error code.
 *
 * NOTES: Run at beginning of main
 */
void sigwinch_initialize();

void cinit(int num, ...);
//ncurses wrapper functions

void cresizeterm(int num, ...);

#endif /* CNCURSES_H */
