#include "macros.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include "cncurses.h"
#include "misc.h"


sigset_t sigwinch_mask;
WINDOW* cwindows[WINDOWS_MAX] = {NULL};
// y, x, beg_y, begin_x
int cwinparams[WINDOWS_MAX][4] = {{0}};
int cwincolors[WINDOWS_MAX];
bool cwinch = false;

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

void cinit(int num, ...){
    if(num >= WINDOWS_MAX){
        panic("Number of cwindows must be less than WINDOWS MAX.", EXIT_FAILURE);
    }
    cwindows[0] = stdscr;
    va_list args;
    va_start(args, num);
    for(int i=1; i<=num; i++){
        cwindows[i] = va_arg(args, WINDOW*);
        getmaxyx(cwindows[i], cwinparams[i][0], cwinparams[i][1]);
        getbegyx(cwindows[i], cwinparams[i][2], cwinparams[i][3]);
        cwinparams[i][0] /= cROWS;
        cwinparams[i][1] /= cCOLS;
        cwinparams[i][2] /= cROWS;
        cwinparams[i][3] /= cCOLS;
    }
    va_end(args);
}

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
    cwinch = true;
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
    dest=strcpy(dest, buffer.at(0));
    for(size_t i=1; i<buffer.size(); i++){
        strcpy(buffer.at(i-1), buffer.at(i));
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
        strcpy(buffer.at(i-1), buffer.at(i));
    }
    buffer.rows--;
}

int opcode(char* row){
    return 100*(row[0]-48)+10*(row[1]-48)+1*(row[2]-48);
}

void cgetyx(int win){
    if(win < 0 || win >= WINDOWS_MAX){
        panic("win out of range", EXIT_FAILURE);
    }
    char str1[BUFFER_COLS_MAX];
    snprintf(str1, 6, "001"DELIM"%d", win);
    buffer.push(str1);
}

void cwmove(int win, int y, int x){
    if(win < 0 || win >= WINDOWS_MAX){
        panic("win out of range", EXIT_FAILURE);
    }
    if(y < 0 || y > cROWS){
        panic("y out of bounds", EXIT_FAILURE);
    }
    if(x < 0 || x > cCOLS){
        panic("x out of bounds", EXIT_FAILURE);
    }
    char str1[BUFFER_COLS_MAX];
    sprintf(str1, "002"DELIM"%d"DELIM"%d"DELIM"%d", win, y, x);
    buffer.push(str1);
}

void cwmove_r(int win, int dy, int dx){
    if(win < 0 || win >= WINDOWS_MAX){
        panic("win out of range", EXIT_FAILURE);
    }
    if(Y[win] + dy < 0 || Y[win] + dy > cROWS){
        panic("dy out of bounds", EXIT_FAILURE);
    }
    if(X[win] + dx < 0 || X[win] + dx > cCOLS){
        panic("dx out of bounds", EXIT_FAILURE);
    }
    char str1[BUFFER_COLS_MAX];
    sprintf(str1, "003"DELIM"%d"DELIM"%d"DELIM"%d", win, dy, dx);
    buffer.push(str1);
}

void cwmove_p(int win, double py, double px){
    if(win < 0 || win >= WINDOWS_MAX){
        panic("win out of range", EXIT_FAILURE);
    }
    if(py < 0 || py > 100){
        panic("dy out of bounds", EXIT_FAILURE);
    }
    if(px < 0 || px > 100){
        panic("dx out of bounds", EXIT_FAILURE);
    }
    char str1[BUFFER_COLS_MAX];
    sprintf(str1, "004"DELIM"%d"DELIM"%f"DELIM"%f", win, py, px);
    buffer.push(str1);
}

void cwrefresh(int win){
    if(win < 0 || win >= WINDOWS_MAX){
        panic("win out of range", EXIT_FAILURE);
    }
    wrefresh(cwindows[win]);
    char str1[BUFFER_COLS_MAX];
    snprintf(str1, 6, "005"DELIM"%d", win);
    buffer.push(str1);
}

void clearBuf(){
    (buffer.clear)();
}

void cwprintw(int win, const char* fmt, ...){
    if(win < 0 || win >= WINDOWS_MAX){
        panic("win out of range", EXIT_FAILURE);
    }
    int numArgs = 0;
    for(int i=0; i<strlen(fmt); i++){
        if(fmt[i] == '%'){
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
                    char* arg_str = va_arg(args, char*);
                    char dummy[2] = DELIM;
                    if(containsDelim(arg_str)){
                        for(int i=0; i<strlen(arg_str); i++){
                            if(arg_str[i] == dummy[0]){
                                buffer.push("000");
                            }
                            else{
                                snprintf(str1, 8, "011"DELIM"%d"DELIM"%c", win, arg_str[i]);
                                buffer.push(str1);
                            }
                        }
                    }
                    else{
                        sprintf(str1, "011"DELIM"%d"DELIM"%s", win, arg_str); 
                        buffer.push(str1);
                    }
                    break;
                case 'd':;
                    sprintf(str1, "012"DELIM"%d"DELIM"%d", win, va_arg(args, int));
                    buffer.push(str1);
                    break;
                case 'f':;
                    sprintf(str1, "013"DELIM"%d"DELIM"%f", win, va_arg(args, double));
                    buffer.push(str1);
                    break;
                default:
                    panic("Can only take %%s, %%d, %%f", EXIT_FAILURE);
                    break;
            }
            i++;
        }
        else if(isascii(fmt[i])){
            str1[0] = fmt[i];
            str1[1] = '\0';
            cwprintw(win, "%s", str1);
        }
        else{
            panic("Invalid charachter passed to cprintw", EXIT_FAILURE);
        }
    }
    va_end(args);
}

void cwvline(int win, char ch, int n){
    char str1[BUFFER_COLS_MAX];
    sprintf(str1, "021"DELIM"%d"DELIM"%c"DELIM"%d", win, ch, n);
    buffer.push(str1);
}

void cwhline(int win, char ch, int n){
    char str1[BUFFER_COLS_MAX];
    sprintf(str1, "022"DELIM"%d"DELIM"%c"DELIM"%d", win, ch, n);
    buffer.push(str1);
}

void screen_buffer_repaint(){
    if(cwinch){
        cwinch = false;
        for(int i=WINDOWS_MAX-1; i>=0; i--){
            if(cwindows[i] == NULL){
                continue;
            }
            wclear(cwindows[i]);
            wrefresh(cwindows[i]);
        }
        endwin();
        refresh();
        getmaxyx(stdscr, cROWS, cCOLS);
        resizeterm(cROWS, cCOLS);
        for(int i=WINDOWS_MAX-1; i>=1; i--){
            if(cwindows[i] == NULL){
                continue;
            }
            wattroff(cwindows[i], cwincolors[i]);
            delwin(cwindows[i]);
            cwindows[i] = newwin(cwinparams[i][0]*cROWS, cwinparams[i][1]*cCOLS, cwinparams[i][2]*cROWS, cwinparams[i][3]*cCOLS);
            wattron(cwindows[i], cwincolors[i]);
        }
        if(buffer.size()==0){
            return;
        }
        if(buffer.size()>=BUFFER_ROWS_MAX){
            panic("buffer corrupted", EXIT_FAILURE);
        }
        for(size_t i=0; i<buffer.size(); i++){
            int cfunc = opcode(buffer.at(i));
            switch(cfunc){
            case 0:
                //print delimeter
                printw(DELIM);
                break;
            case 1:
                // getyx
                // args: WINDOW* 
                {
                    char* preserve = strdup(buffer.at(i));
                    char* token = strtok(buffer.at(i), DELIM);
                    token = strtok(NULL, DELIM);
                    int win = atoi(token);
                    getyx(cwindows[win], Y[win], X[win]);
                    strcpy(buffer.at(i), preserve);
                }
                break;
            case 2:;
                // wmove
                // args: win, y,x
                {
                char* preserve = strdup(buffer.at(i));
                char* token = strtok(buffer.at(i), DELIM);
                token = strtok(NULL, DELIM);
                int win = atoi(token);
                token = strtok(NULL, DELIM);
                int y = atoi(token);
                token = strtok(NULL, DELIM);
                int x = atoi(token);
                wmove(cwindows[win], y, x);
                strcpy(buffer.at(i), preserve);
                }
                break;
            case 3:;
                // wmove_r 
                // args: win, dy,dx
                {
                char* preserve = strdup(buffer.at(i));
                char* token = strtok(buffer.at(i), DELIM);
                token = strtok(NULL, DELIM);
                int win = atoi(token);
                token = strtok(NULL, DELIM);
                int dy = atoi(token);
                token = strtok(NULL, DELIM);
                int dx = atoi(token);
                wmove(cwindows[win], Y[win]+dy, X[win]+dx);
                strcpy(buffer.at(i), preserve);
                }
                break;
            case 4:;
                // wmove_p
                // args: win, percenty,percentx
                {
                char* preserve = strdup(buffer.at(i));
                char* token = strtok(buffer.at(i), DELIM);
                token = strtok(NULL, DELIM);
                int win = atoi(token);
                token = strtok(NULL, DELIM);
                double py = atof(token);
                token = strtok(NULL, DELIM);
                double px = atof(token);
                wmove(cwindows[win], (int)cROWS*py, (int)cCOLS*px);
                strcpy(buffer.at(i), preserve);
                }
                break;
            case 5:;
                //wrefresh
                //args: win
                {
                char* preserve = strdup(buffer.at(i));
                char* token = strtok(buffer.at(i), DELIM);
                token = strtok(NULL, DELIM);
                int win = atoi(token);
                wrefresh(cwindows[win]);
                strcpy(buffer.at(i), preserve);
                }
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
                // wprintw single string
                // args: win, str
                {
                //strtok destroys original. Must preserve copy.
                char* preserve = strdup(buffer.at(i));
                //first token is opcode- throw away
                char* token = strtok(buffer.at(i), DELIM);
                token = strtok(NULL, DELIM);
                int win = atoi(token);
                token = strtok(NULL, DELIM);
                char* str = strdup(token);
                wprintw(cwindows[win], "%s", str);
                //restore copy
                strcpy(buffer.at(i), preserve);
                }
                break;
            case 12:;
                // wprintw single decimal
                // args: win, dec
                {
                //strtok destroys original. Must preserve copy.
                char* preserve = strdup(buffer.at(i));
                //first token is opcode- throw away
                char* token = strtok(buffer.at(i), DELIM);
                token = strtok(NULL, DELIM);
                int win = atoi(token);
                token = strtok(NULL, DELIM);
                int dec = atoi(token);
                wprintw(cwindows[win], "%d", dec);
                //restore copy
                strcpy(buffer.at(i), preserve);
                }
                break;
            case 13:;
                // wprintw single float
                // args: win, flt
                {
                //strtok destroys original. Must preserve copy.
                char* preserve = strdup(buffer.at(i));
                //first token is opcode- throw away
                char* token = strtok(buffer.at(i), DELIM);
                token = strtok(NULL, DELIM);
                int win = atoi(token);
                token = strtok(NULL, DELIM);
                double flt = atof(token);
                wprintw(cwindows[win], "%f", flt);
                //restore copy
                strcpy(buffer.at(i), preserve);
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
                // wvline
                // args: ch, n
                {
                char* preserve = strdup(buffer.at(i));
                char* token = strtok(buffer.at(i), DELIM);
                token = strtok(NULL, DELIM);
                int win = atoi(token);
                token = strtok(NULL, DELIM);
                char ch = token[0];
                token = strtok(NULL, DELIM);
                int n = atoi(token);
                wvline(cwindows[win], ch, n);
                strcpy(buffer.at(i), preserve);
                }
                break;
            case 22:;
                // whline
                // args: ch, n
                {
                char* preserve = strdup(buffer.at(i));
                char* token = strtok(buffer.at(i), DELIM);
                token = strtok(NULL, DELIM);
                int win = atoi(token);
                token = strtok(NULL, DELIM);
                char ch = token[0];
                token = strtok(NULL, DELIM);
                int n = atoi(token);
                whline(cwindows[win], ch, n);
                strcpy(buffer.at(i), preserve);
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
        for(int i=WINDOWS_MAX-1; i>=0; i--){
            if(cwindows[i] == NULL){
                continue;
            }
            wrefresh(cwindows[i]);
        }
    }
    else{
        //do nothing
    }
}
