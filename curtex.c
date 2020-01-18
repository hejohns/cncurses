#include <string.h>
#include "curtex.h"
#include "misc.h"

screen_buffer buffer={
    .push=&screen_buffer_push,
    .pop=&screen_buffer_pop,
    .size=&screen_buffer_size,
    .at=&screen_buffer_at,
    .clear=&screen_buffer_clear,
    .erase=&screen_buffer_erase,
    .rows=(size_t)0
};

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

void screen_buffer_push(char* command){
    if(buffer.size() >= BUFFER_ROWS_MAX){
        crash("buffer is full", EXIT_FAILURE);
    }
    if(strlen(command) >= BUFFER_COLS_MAX){
        crash("command too long", EXIT_FAILURE);
    }
    strcpy(buffer.queue[buffer.size()], command);
    buffer.rows++;
}

char* screen_buffer_pop(char* dest){
    if(buffer.size()==0){
        crash("buffer is empty; nothing to pop", EXIT_FAILURE);
    }
    dest=strcpy(dest, buffer.queue[0]);
    for(size_t i=1; i<buffer.size(); i++){
        strcpy(buffer.queue[i-1], buffer.queue[i]);
    }
    buffer.rows--;
    return dest;
}

size_t screen_buffer_size(){
    return buffer.rows;
}

char* screen_buffer_at(size_t index){
    if(index >= buffer.size()){
        crash("index out of range", EXIT_FAILURE);
    }
    return buffer.queue[index];
}

void screen_buffer_clear(){
    buffer.rows=0;
}

void screen_buffer_erase(size_t index){
    if(index >= buffer.size()){
        crash("index out of range", EXIT_FAILURE);
    }
    for(size_t i=index+1; i<buffer.size(); i++){
        strcpy(buffer.queue[i-1], buffer.queue[i]);
    }
    buffer.rows--;
}
