//cncurses.h
#ifndef CNCURSES_H
#define CNCURSES_H



typedef struct screen_buffer screen_buffer;

/* globals */
//declared in header so main also has reference implicitly (important for cnewterm macro)
int cROWS, cCOLS;
extern bool cwinch;

/* REQUIRES: num arguments of type screen_buffer*
 * MODIFIES: each screen_buffer* passed is initialized and screen_buffer->ptr set to valid  WINDOW*
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
 * EFFECTS: sets sigwinch_handler() as SIGWINCH handle. On failure, exits w/ error code.
 *
 * NOTES: Run at beginning of main
 */
void sigwinch_initialize();


/* ncurses wrapper functions */
void cresizeterm(int num, ...);

void cwprintw(screen_buffer* win, const char* fmt, ...);
/* * * * * * * * * * * * * * */

#endif /* CNCURSES_H */
