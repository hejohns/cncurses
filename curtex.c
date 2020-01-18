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
    .repaint=&screen_buffer_repaint,
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
    //
    buffer.repaint();
    //
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

int opcode(char* row){
    return 100*(row[0]-48)+10*(row[1]-48)+1*(row[2]-48);
}

void screen_buffer_repaint(){
    if(buffer.size()==0){
        return;
    }
    if(buffer.size()>=BUFFER_ROWS_MAX){
        crash("buffer corrupted", EXIT_FAILURE);
    }
    for(size_t i=0; i<buffer.size(); i++){
        int cfunc = opcode(buffer.queue[i]);
        switch(cfunc){
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        case 10:
            mvprintw(ROWS_/2, COLS_/2, "%d-%d", ROWS_, COLS_);
            break;
        case 11:
            break;
        case 12:
            break;
        case 13:
            break;
        case 14:
            break;
        case 15:
            break;
        case 16:
            break;
        case 17:
            break;
        case 18:
            break;
        case 19:
            break;
        case 20:
            break;
        case 21:
            break;
        case 22:
            break;
        case 23:
            break;
        case 24:
            break;
        case 25:
            break;
        case 26:
            break;
        case 27:
            break;
        case 28:
            break;
        case 29:
            break;
        case 30:
            break;
        case 31:
            break;
        case 32:
            break;
        case 33:
            break;
        case 34:
            break;
        case 35:
            break;
        case 36:
            break;
        case 37:
            break;
        case 38:
            break;
        case 39:
            break;
        case 40:
            break;
        case 41:
            break;
        case 42:
            break;
        case 43:
            break;
        case 44:
            break;
        case 45:
            break;
        case 46:
            break;
        case 47:
            break;
        case 48:
            break;
        case 49:
            break;
        case 50:
            break;
        case 51:
            break;
        case 52:
            break;
        case 53:
            break;
        case 54:
            break;
        case 55:
            break;
        case 56:
            break;
        case 57:
            break;
        case 58:
            break;
        case 59:
            break;
        case 60:
            break;
        case 61:
            break;
        case 62:
            break;
        case 63:
            break;
        case 64:
            break;
        case 65:
            break;
        case 66:
            break;
        case 67:
            break;
        case 68:
            break;
        case 69:
            break;
        case 70:
            break;
        case 71:
            break;
        case 72:
            break;
        case 73:
            break;
        case 74:
            break;
        case 75:
            break;
        case 76:
            break;
        case 77:
            break;
        case 78:
            break;
        case 79:
            break;
        case 80:
            break;
        case 81:
            break;
        case 82:
            break;
        case 83:
            break;
        case 84:
            break;
        case 85:
            break;
        case 86:
            break;
        case 87:
            break;
        case 88:
            break;
        case 89:
            break;
        case 90:
            break;
        case 91:
            break;
        case 92:
            break;
        case 93:
            break;
        case 94:
            break;
        case 95:
            break;
        case 96:
            break;
        case 97:
            break;
        case 98:
            break;
        case 99:
            break;
        case 100:
            break;
        }
    }
}
