#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include <stdarg.h>

#define EXIT_KEY '|'
void stuff(char* cstr, int num, ...){
    va_list args;
    va_start(args, num);
    for(int i=0; i<num; i++){
        cstr[strlen(cstr)-2] = va_arg(args, int);
        system(cstr);
    }
    va_end(args);
}

int main(int argc, char** argv){
    if(argc != 3){
        fprintf(stderr, "incorrect usage\n");
        exit(EXIT_FAILURE);
    }
    unsigned int uniq = time(NULL);
    size_t sz = 80;
    char* cstr = malloc(sz);
    int len = snprintf(cstr, sz, "screen -S %s-%d -dm bash -i", argv[1], uniq);
    if(len > sz){
        char* tmp = realloc(cstr, len+1);
        if(tmp == NULL){
            fprintf(stderr, "realloc failure\n");
            exit(EXIT_FAILURE);
        }
        else{
            sz = len+1;
            cstr = tmp;
            sprintf(cstr, "screen -S %s-%d -dm bash -i", argv[1], uniq);
        }
    }
    system(cstr);
    len = snprintf(cstr, sz, "screen -S %s-%d -p 0 -X stuff \"c\"", argv[1], uniq);
    if(len > sz){
        char* tmp = realloc(cstr, len+1);
        if(tmp == NULL){
            fprintf(stderr, "realloc failure\n");
            exit(EXIT_FAILURE);
        }
        else{
            sz = len+1;
            cstr = tmp;
            sprintf(cstr, "screen -S %s-%d -p 0 -X stuff \"c\"", argv[1], uniq);
        }
    }
    stuff(cstr, 4, 'g', 'd', 'b', ' ');
    for(int i=0; i<strlen(argv[2]); i++){
        stuff(cstr, 1, argv[2][i]);
    }
    stuff(cstr, 32, ' ', '2', '>', '&', '1', ' ', '|', 't', 'e', 'e', ' ', '.', '/', 'b', 't', '.', 't', 'm', 'p', '>', '.', '/', 'i', 'n', 'f', 'o', 'b', '.', 't', 'm', 'p','\n');
    initscr();
    timeout(-1);
    curs_set(0);
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    clear();
    refresh();
    int btfd;
    int infobfd;
    if(btfd = open("./bt.tmp", O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, 0644), btfd == -1){
        fprintf(stderr, "open() failed\n");
        endwin();
        exit(EXIT_FAILURE);
    }
    if(infobfd = open("./infob.tmp", O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, 0644), infobfd == -1){
        fprintf(stderr, "open() failed\n");
        endwin();
        exit(EXIT_FAILURE);
    }
    char* screenStuff = malloc(strlen("screen -S ") + strlen(argv[1]) + strlen(" -p 0 -X stuff \"c\"") + 1);
    int ch = 0;
    while((ch = getch()) != EXIT_KEY){
        if(ch == KEY_BACKSPACE){
            sprintf(screenStuff, "screen -S %s -p 0 -X stuff \"%c\"", argv[1], '');
            system(screenStuff);
            stuff(cstr, 1, '');
        }
        else if(ch == KEY_LEFT){
            sprintf(screenStuff, "screen -S %s -p 0 -X stuff \"%c\"", argv[1], '');
            system(screenStuff);
            stuff(screenStuff, 2, 'O', 'D');
            stuff(cstr, 3, '', 'O', 'D');
        }
        else if(ch == KEY_RIGHT){
            sprintf(screenStuff, "screen -S %s -p 0 -X stuff \"%c\"", argv[1], '');
            system(screenStuff);
            stuff(screenStuff, 2, 'O', 'C');
            stuff(cstr, 3, '', 'O', 'C');
        }
        else if(ch == KEY_UP){
            sprintf(screenStuff, "screen -S %s -p 0 -X stuff \"%c\"", argv[1], '');
            system(screenStuff);
            stuff(screenStuff, 2, 'O', 'A');
            stuff(cstr, 3, '', 'O', 'A');
        }
        else if(ch == KEY_DOWN){
            sprintf(screenStuff, "screen -S %s -p 0 -X stuff \"%c\"", argv[1], '');
            system(screenStuff);
            stuff(screenStuff, 2, 'O', 'B');
            stuff(cstr, 3, '', 'O', 'B');
        }
        else if(ch == '\n'){
            sprintf(screenStuff, "screen -S %s -p 0 -X stuff \"%c\"", argv[1], '\n');
            system(screenStuff);
            stuff(cstr, 1, '\n');
            truncate("./bt.tmp", 0);
            stuff(cstr, 3, 'b', 't', '\n');
            //truncate("./infob.tmp", 0);
            //stuff(cstr, 6, 'i', 'n', 'f', 'o', ' ', 'b', '\n');
        }
        else{
            sprintf(screenStuff, "screen -S %s -p 0 -X stuff \"%c\"", argv[1], (char)ch);
            system(screenStuff);
            stuff(cstr, 1, (char)ch);
        }
    }
    endwin();
    sprintf(cstr, "screen -S %s-%d -X quit", argv[1], uniq);
    system(cstr);
    sprintf(screenStuff, "screen -S %s -X quit", argv[1]);
    system(screenStuff);
    free(cstr);
    free(screenStuff);
    close(btfd);
    close(infobfd);
    remove("./bt.tmp");
    remove("./infob.tmp");
    return 0;
}
