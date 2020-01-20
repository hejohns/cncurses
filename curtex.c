#include "macros.h"
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
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
	panic("Failed to set SIGWINCH handler", EXIT_FAILURE);
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
        panic("buffer is full", EXIT_FAILURE);
    }
    if(strlen(command) >= BUFFER_COLS_MAX){
        panic("command too long", EXIT_FAILURE);
    }
    strcpy(buffer.queue[buffer.size()], command);
    buffer.rows++;
}

char* screen_buffer_pop(char* dest){
    if(buffer.size()==0){
        panic("buffer is empty; nothing to pop", EXIT_FAILURE);
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
        panic("index out of range", EXIT_FAILURE);
    }
    return buffer.queue[index];
}

void screen_buffer_clear(){
    buffer.rows=0;
}

void screen_buffer_erase(size_t index){
    if(index >= buffer.size()){
        panic("index out of range", EXIT_FAILURE);
    }
    for(size_t i=index+1; i<buffer.size(); i++){
        strcpy(buffer.queue[i-1], buffer.queue[i]);
    }
    buffer.rows--;
}

int opcode(char* row){
    return 100*(row[0]-48)+10*(row[1]-48)+1*(row[2]-48);
}

void cgetyx(){
    buffer.push("001");
}

void cmove(int y, int x){
    if(y < 0 || y > ROWS_){
        panic("y out of bounds", EXIT_FAILURE);
    }
    if(x < 0 || x > COLS_){
        panic("x out of bounds", EXIT_FAILURE);
    }
    char str1[BUFFER_COLS_MAX];
    sprintf(str1, "002,%d,%d", y, x);
    buffer.push(str1);
}

void cmove_r(int dy, int dx){
    if(Y + dy < 0 || Y + dy > ROWS_){
        panic("dy out of bounds", EXIT_FAILURE);
    }
    if(X + dx < 0 || X + dx > COLS_){
        panic("dx out of bounds", EXIT_FAILURE);
    }
    char str1[BUFFER_COLS_MAX];
    sprintf(str1, "003,%d,%d", dy, dx);
    buffer.push(str1);
}

void cmove_p(double py, double px){
    if(py < 0 || py > 100){
        panic("dy out of bounds", EXIT_FAILURE);
    }
    if(px < 0 || px > 100){
        panic("dx out of bounds", EXIT_FAILURE);
    }
    char str1[BUFFER_COLS_MAX];
    sprintf(str1, "004,%f,%f", py, px);
    buffer.push(str1);
}

void cprintw(const char* fmt, ...){
    int numArgs = 0;
    for(int i=0; i<strlen(fmt); i++){
        if(fmt[0] == '%'){
            numArgs++;
        }
    }
    if(numArgs == 0){
        panic("cprintw needs more than just fmt argument. Use %%s.", EXIT_FAILURE);
    }
    va_list args;
    va_start(args, fmt);
    char str1[BUFFER_COLS_MAX];
    for(int i=0; i<strlen(fmt); i++){
        if(fmt[i] == '%'){
            switch(fmt[i+1]){
                case 's':;
                    sprintf(str1, "011,%s", va_arg(args, char*));
                    buffer.push(str1);
                    break;
                case 'd':;
                    sprintf(str1, "012,%d", va_arg(args, int));
                    buffer.push(str1);
                    break;
                case 'f':;
                    sprintf(str1, "013,%f", va_arg(args, double));
                    buffer.push(str1);
                    break;
                default:
                    panic("Can only take %%s, %%d, %%f", EXIT_FAILURE);
                    break;
            }
            i++;
        }
        else if(isascii(fmt[i])){
            cprintw("%s", fmt[i]);
        }
        else{
            panic("Invalid charachter passed to cprintw", EXIT_FAILURE);
        }
    }
    va_end(args);
}

void cvline(char ch, int n){
    char str1[BUFFER_COLS_MAX];
    sprintf(str1, "021,%c,%d", ch, n);
    buffer.push(str1);
}

void chline(char ch, int n){
    char str1[BUFFER_COLS_MAX];
    sprintf(str1, "022,%c,%d", ch, n);
    buffer.push(str1);
}

void screen_buffer_repaint(){
    if(buffer.size()==0){
        return;
    }
    if(buffer.size()>=BUFFER_ROWS_MAX){
        panic("buffer corrupted", EXIT_FAILURE);
    }
    for(size_t i=0; i<buffer.size(); i++){
        int cfunc = opcode(buffer.queue[i]);
        switch(cfunc){
        case 0:
            break;
        case 1:
            // getxy
            // args: none
            {
                getyx(stdscr, Y, X);
            }
            break;
        case 2:;
            // move
            // args: y,x
            {
            char* preserve = strdup(buffer.queue[i]);
            char* token = strtok(buffer.queue[i], ",");
            token = strtok(NULL, ",");
            int y = atoi(token);
            token = strtok(NULL, ",");
            int x = atoi(token);
            move(y, x);
            strcpy(buffer.queue[i], preserve);
            }
            break;
        case 3:;
            // move_r 
            // args: dy,dx
            {
            char* preserve = strdup(buffer.queue[i]);
            char* token = strtok(buffer.queue[i], ",");
            token = strtok(NULL, ",");
            int dy = atoi(token);
            token = strtok(NULL, ",");
            int dx = atoi(token);
            move(Y+dy, X+dx);
            strcpy(buffer.queue[i], preserve);
            }
            break;
        case 4:;
            // move_p
            // args: percenty,percentx
            {
            char* preserve = strdup(buffer.queue[i]);
            char* token = strtok(buffer.queue[i], ",");
            token = strtok(NULL, ",");
            double py = atof(token);
            token = strtok(NULL, ",");
            double px = atof(token);
            move((int)ROWS_*py, (int)COLS_*px);
            strcpy(buffer.queue[i], preserve);
            }
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
            break;
        case 11:;
            // printw single string
            // args: str
            {
            //strtok destroys original. Must preserve copy.
            char* preserve = strdup(buffer.queue[i]);
            //first token is opcode- throw away
            char* token = strtok(buffer.queue[i], ",");
            token = strtok(NULL, ",");
            char* str = strdup(token);
            printw("%s", str);
            //restore copy
            strcpy(buffer.queue[i], preserve);
            }
            break;
        case 12:;
            // printw single decimal
            // args: dec
            {
            //strtok destroys original. Must preserve copy.
            char* preserve = strdup(buffer.queue[i]);
            //first token is opcode- throw away
            char* token = strtok(buffer.queue[i], ",");
            token = strtok(NULL, ",");
            int dec = atoi(token);
            printw("%d", dec);
            //restore copy
            strcpy(buffer.queue[i], preserve);
            }
            break;
        case 13:;
            // printw single float
            // args: flt
            {
            //strtok destroys original. Must preserve copy.
            char* preserve = strdup(buffer.queue[i]);
            //first token is opcode- throw away
            char* token = strtok(buffer.queue[i], ",");
            token = strtok(NULL, ",");
            double flt = atof(token);
            printw("%f", flt);
            //restore copy
            strcpy(buffer.queue[i], preserve);
            }
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
        case 21:;
            // vline
            // args: ch, n
            {
            char* preserve = strdup(buffer.queue[i]);
            char* token = strtok(buffer.queue[i], ",");
            token = strtok(NULL, ",");
            char ch = token[0];
            token = strtok(NULL, ",");
            int n = atoi(token);
            vline(ch, n);
            strcpy(buffer.queue[i], preserve);
            }
            break;
        case 22:;
            // hline
            // args: ch, n
            {
            char* preserve = strdup(buffer.queue[i]);
            char* token = strtok(buffer.queue[i], ",");
            token = strtok(NULL, ",");
            char ch = token[0];
            token = strtok(NULL, ",");
            int n = atoi(token);
            hline(ch, n);
            strcpy(buffer.queue[i], preserve);
            }
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
        default:
            panic("opcode invalid", EXIT_FAILURE);
            break;
        }
    }
}
