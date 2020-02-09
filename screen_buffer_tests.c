//screen_buffer_tests.c
#include "macros.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <sys/types.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include "screen_buffer.h"
#include "cncurses.h"


screen_buffer win1;
screen_buffer win2, win3;

int main(int argc, char** argv){
    cinit(1, &win1);
    cinit(2, &win2, &win3);
    /* test screen_buffer_at/screen_buffer_push */
    char* test1 = "this is a string\n";
    char* test2 = "this is another string\t";
    char* test3 = "this is yet another string\n";
    call2(&win1, push, test1);
    call2(&win1, push, test2);
    call2(&win1, push, test3);
    if(strcmp(call2(&win1, at, 0), test1)) panic2("", EXIT_FAILURE);
    if(strcmp(call2(&win1, at, 1), test2)) panic2("", EXIT_FAILURE);
    if(strcmp(call2(&win1, at, 2), test3)) panic2("", EXIT_FAILURE);
    char* testRealloc = "a long string\t";
    for(int i=3; i<20000; i++){
        call2(&win1, push, testRealloc);
    }
    if(strcmp(call2(&win1, at, 3), testRealloc)) panic2("", EXIT_FAILURE);
    if(argc == 2){
        if(!strcmp(argv[1], "-1")){
            for(int i=4; i<20000; i++){
                if(strcmp(call2(&win1, at, -1), testRealloc)) panic2("", EXIT_FAILURE);
            }
        }
    }
    else{
        for(int i=4; i<20000; i++){
            if(strcmp(call2(&win1, at, i), testRealloc)) panic2("", EXIT_FAILURE);
        }
    }
    /* test screen_buffer_pop */
    char* popTest;
    size_t sizeBef = call(&win1, size);
    popTest = call(&win1, pop);
    if(strcmp(testRealloc, popTest)) panic2("", EXIT_FAILURE);
    if(call(&win1, size) != sizeBef-1) panic2("", EXIT_FAILURE);
    free(popTest);
    /* test screen_buffer_erase */
    call2(&win1, cerase, 0);
    if(strcmp(call2(&win1, at, 0), test2)) panic2("", EXIT_FAILURE);
    /* test screen_buffer_clear */
    call(&win1, cclear);
    if(call(&win1, size) != 0) panic2("", EXIT_FAILURE);
    if(win1.queue_size != SCREEN_BUFFER_QUEUE_INITIAL) panic2("", EXIT_FAILURE);
    // not freeing memory so valgrind will show how much memory was allocated at the end
    return 0;
}
