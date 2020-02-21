#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define EXIT_KEY KEY_HOME

int main(int argc, char** argv){
    //check for existing bt.tmp and infob.tmp
    initscr();
    timeout(-1);
    curs_set(0);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    clear();
    refresh();
    char* screenStuff = malloc(strlen("screen -S ") + strlen(argv[1]) + strlen(" -p 0 -X stuff \"c\"") + 1);
    int ch;
    while((ch = getch()) != EXIT_KEY){
        switch(ch){
            KEY_BACKSPACE:
                sprintf(screenStuff, "screen -S %s -p 0 -X stuff \"%c\"", argv[1], '');
                system(screenStuff);
                break;
            default:
                sprintf(screenStuff, "screen -S %s -p 0 -X stuff \"%c\"", argv[1], (char)ch);
                system(screenStuff);
                break;
        }
    }
    free(screenStuff);
    return 0;
}
