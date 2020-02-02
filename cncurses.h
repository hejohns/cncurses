//cncurses.h
#ifndef CNCURSES_H
#define CNCURSES_H

#include <curses.h>
#include <signal.h>


//globals
int cROWS, cCOLS;
int Y[WINDOWS_MAX];
int X[WINDOWS_MAX];
extern WINDOW* cwindows[WINDOWS_MAX];
extern int cwincolors[WINDOWS_MAX];
extern bool cwinch;

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


/* REQUIRES: valid win, command
 * MODIFIES: none
 * EFFECTS: pushes command into back of queue for window win
 *
 * NOTES: command format- 999,...
 * first three digits are opcode, followed by DELIM seperated args
 */
void screen_buffer_push(int win, char* command);


/* REQUIRES: valid win, dest pointer, buffer.size()>0
 * MODIFIES: overwrites dest with last line of queue for window win
 * EFFECTS: pops back off queue and returns. row-- 
 *
 * NOTES: useful for deleting (think backspace)
 */
char* screen_buffer_pop(int win, char* dest);


/* REQUIRES: valid win
 * MODIFIES: none
 * EFFECTS: returns size of queue for window win
 *
 * NOTES:
 */
size_t screen_buffer_size(int win);


/* REQUIRES: valid win, idnex
 * MODIFIES: none
 * EFFECTS: returns pointer to row index of queue for window win
 *
 * NOTES: does basic sanity checks. panics upon error.
 */
char* screen_buffer_at(int win, size_t index);


/* REQUIRES: valid win
 * MODIFIES: none
 * EFFECTS: sets rows=0. Does not overwrite existing values in queue
 *
 * NOTES: a "quick format"
 */
void screen_buffer_clear(int win);


/* REQUIRES: valid win, index
 * MODIFIES: none
 * EFFECTS: size--
 *
 * NOTES: erases row index of queue for window win
 */
void screen_buffer_erase(int win, size_t index);


/* REQUIRES: valid win
 * MODIFIES: none
 * EFFECTS: repaints terminal as stored in queue for window win
 *
 * NOTES: repainting too frequently seems to cause screen flicker.
 * Need to look into the root cause.
 */
void screen_buffer_repaint(int win);


// declare and initialize screen buffer
typedef struct{
    //"public"
    void (*push)(int, char*);
    char* (*pop)(int, char*);
    size_t (*size)(int);
    char* (*at)(int, size_t);
    void (*clear)(int);
    void (*erase)(int, size_t);
    void (*repaint)(int);
    //private
    char queue[BUFFER_ROWS_MAX][BUFFER_COLS_MAX];
    size_t rows;
} screen_buffer;
screen_buffer buffer[WINDOWS_MAX];


void cinit(int num, ...);
// ncurses wrapper functions
void cgetyx(int win);
void cwmove(int win, int y, int x);
void cwmove_r(int win, int dy, int dx);
void cwmove_p(int win, double py, double px);
void cwrefresh(int win);
void cclear(int win);
void cwprintw(int win, const char* fmt, ...);
void cwvline(int win, char ch, int n);
void cwhline(int win, char ch, int n);
#endif /* CNCURSES_H */
