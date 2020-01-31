//cncurses.h
#ifndef CNCURSES_H
#define CNCURSES_H

#include <curses.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


//globals
sigset_t sigwinch_mask;
int cROWS, cCOLS;
int Y[WINDOWS_MAX];
int X[WINDOWS_MAX];
extern WINDOW* cwindows[WINDOWS_MAX];

/* REQUIRES: number of arguments of type WINDOW* < WINDOWS_MAX
 * MODIFIES: none
 * EFFECTS: Assigns WINDOW* to indicies of cwindows[WINDOWS_MAX]
 */
void cinit(int num, ...);


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
    //"public"
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
screen_buffer buffer;


void cinit(int num, ...);
// ncurses wrapper functions
void cgetyx(int win);
void cwmove(int win, int y, int x);
void cwmove_r(int win, int dy, int dx);
void cwmove_p(int win, double py, double px);
void cwrefresh(int win);
void clearBuf();
void cwprintw(int win, const char* fmt, ...);
void cwvline(int win, char ch, int n);
void cwhline(int win, char ch, int n);
#endif /* CNCURSES_H */
