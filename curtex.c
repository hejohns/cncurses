#include "curtex.h"
#include "misc.h"

void sigwinch_initialize(){
    sigemptyset(&sigwinch_mask);
    const struct sigaction sigwinch_action = {
	.sa_handler = &sigwinch_handler,
	.sa_mask = sigwinch_mask,
	.sa_flags = 0
    };
    if(sigaction(SIGWINCH, &sigwinch_action, NULL)){
	crash("Failed to set SIGWINCH handler", EXIT_FAILURE);
    }
}

void sigwinch_handler(){
    endwin();
    refresh();
    clear();
    getmaxyx(stdscr, ROWS_, COLS_);
    resizeterm(ROWS_, COLS_);
    mvprintw(ROWS_/2, (COLS_-6)/2, "%d-%d", ROWS_, COLS_);
    refresh();
    getch();
}
