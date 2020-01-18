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
#endif /* CURTEX_H */
