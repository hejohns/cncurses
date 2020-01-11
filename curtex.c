#include <curses.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int ROWS_, COLS_;

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

sigset_t sigwinch_mask;

int main(int argc, char** argv){
    sigemptyset(&sigwinch_mask);
    const struct sigaction sigwinch_action = {
	.sa_handler = &sigwinch_handler,
	.sa_mask = sigwinch_mask,
	.sa_flags = 0
    };
    if(sigaction(SIGWINCH, &sigwinch_action, NULL)){
	printf("Error setting SIGWINCH handler");
	return EXIT_FAILURE;
    }
    initscr();
    getmaxyx(stdscr, ROWS_, COLS_);
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    clear();
    mvprintw(ROWS_/2, (COLS_-6)/2, "%d-%d", ROWS_, COLS_);
    refresh();
    getch();
    while(true){}
    endwin();
    return EXIT_SUCCESS;
}
