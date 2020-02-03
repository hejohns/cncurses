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
double cwinparams[WINDOWS_MAX][4] = {{0}};
int cwincolors[WINDOWS_MAX];
bool cwinch = false;

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
    for(int i=0; i<WINDOWS_MAX; i++){
        buffer[i]=(screen_buffer){
            .push=&screen_buffer_push,
            .pop=&screen_buffer_pop,
            .size=&screen_buffer_size,
            .at=&screen_buffer_at,
            .clear=&screen_buffer_clear,
            .erase=&screen_buffer_erase,
            .repaint=&screen_buffer_repaint,
            .rows=(size_t)0
        };
    }
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

void screen_buffer_push(int win, char* command){
    if(buffer[win].size(win) >= BUFFER_ROWS_MAX){
        panic("buffer is full", EXIT_FAILURE);
    }
    if(strlen(command) >= BUFFER_COLS_MAX){
        panic("command too long", EXIT_FAILURE);
    }
    strcpy(buffer[win].queue[buffer[win].size(win)], command);
    buffer[win].rows++;
}

char* screen_buffer_pop(int win, char* dest){
    if(buffer[win].size(win)==0){
        panic("buffer is empty-- nothing to pop", EXIT_FAILURE);
    }
    dest=strcpy(dest, buffer[win].at(win, buffer[win].size(win)-1));
    buffer[win].rows--;
    return dest;
}

size_t screen_buffer_size(int win){
    return buffer[win].rows;
}

char* screen_buffer_at(int win, size_t index){
    if(index >= buffer[win].size(win)){
        panic("index out of range", EXIT_FAILURE);
    }
    return buffer[win].queue[index];
}

void screen_buffer_clear(int win){
    buffer[win].rows=0;
}

void screen_buffer_erase(int win, size_t index){
    if(index >= buffer[win].size(win)){
        panic("index out of range", EXIT_FAILURE);
    }
    for(size_t i=index+1; i<buffer[win].size(win); i++){
        strcpy(buffer[win].at(win, i-1), buffer[win].at(win, i));
    }
    buffer[win].rows--;
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
    buffer[0].push(0, str1);
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
    buffer[0].push(0, str1);
}

void cwmove_r(int win, int dy, int dx){
    if(win < 0 || win >= WINDOWS_MAX){
        panic("win out of range", EXIT_FAILURE);
    }
    char str1[BUFFER_COLS_MAX];
    sprintf(str1, "003"DELIM"%d"DELIM"%d"DELIM"%d", win, dy, dx);
    buffer[0].push(0, str1);
}

void cwmove_p(int win, double py, double px){
    if(win < 0 || win >= WINDOWS_MAX){
        panic("win out of range", EXIT_FAILURE);
    }
    char str1[BUFFER_COLS_MAX];
    snprintf(str1, BUFFER_COLS_MAX-1, "004"DELIM"%d"DELIM"%.4g"DELIM"%.4g", win, py, px);
    buffer[0].push(0, str1);
}

void cwrefresh(int win){
    if(win < 0 || win >= WINDOWS_MAX){
        panic("win out of range", EXIT_FAILURE);
    }
    wrefresh(cwindows[win]);
    char str1[BUFFER_COLS_MAX];
    snprintf(str1, 6, "005"DELIM"%d", win);
    buffer[0].push(0, str1);
}

void cclear(int win){
    (buffer[win].clear)(win);
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
        panic("cwprintw needs more than just fmt argument. Use %%s.", EXIT_FAILURE);
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
                                buffer[win].push(win, "000");
                            }
                            else{
                                snprintf(str1, 8, "011"DELIM"%d"DELIM"%c", win, arg_str[i]);
                                buffer[win].push(win, str1);
                            }
                        }
                    }
                    else{
                        sprintf(str1, "011"DELIM"%d"DELIM"%s", win, arg_str); 
                        buffer[win].push(win, str1);
                    }
                    break;
                case 'd':;
                    sprintf(str1, "012"DELIM"%d"DELIM"%d", win, va_arg(args, int));
                    buffer[win].push(win, str1);
                    break;
                case 'f':;
                    sprintf(str1, "013"DELIM"%d"DELIM"%f", win, va_arg(args, double));
                    buffer[win].push(win, str1);
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
    buffer[win].push(win, str1);
}

void cwhline(int win, char ch, int n){
    char str1[BUFFER_COLS_MAX];
    sprintf(str1, "022"DELIM"%d"DELIM"%c"DELIM"%d", win, ch, n);
    buffer[win].push(win, str1);
}

void screen_buffer_repaint(int win){
    if(cwinch){
        cwinch = false;
        if(win == 0){
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
                cwindows[i] = newwin((int)(cwinparams[i][0]*cROWS), (int)(cwinparams[i][1]*cCOLS), (int)(cwinparams[i][2]*cROWS), (int)(cwinparams[i][3]*cCOLS));
                wattron(cwindows[i], cwincolors[i]);
            }
        }
        else{
            wclear(cwindows[win]);
            wrefresh(cwindows[win]);
        }
        if(buffer[win].size(win)==0){
            return;
        }
        if(buffer[win].size(win)>=BUFFER_ROWS_MAX){
            panic("buffer corrupted", EXIT_FAILURE);
        }
        for(size_t i=0; i<buffer[win].size(win); i++){
            int cfunc = opcode(buffer[win].at(win, i));
            switch(cfunc){
            case 0:
                //print delimeter
                printw(DELIM);
                break;
            case 1:
                // getyx
                // args: WINDOW* 
                {
                    char* preserve = strdup(buffer[win].at(win, i));
                    char* token = strtok(buffer[win].at(win, i), DELIM);
                    token = strtok(NULL, DELIM);
                    int win2 = atoi(token);
                    getyx(cwindows[win2], Y[win2], X[win2]);
                    strcpy(buffer[win].at(win, i), preserve);
                }
                break;
            case 2:;
                // wmove
                // args: win, y,x
                {
                char* preserve = strdup(buffer[win].at(win, i));
                char* token = strtok(buffer[win].at(win, i), DELIM);
                token = strtok(NULL, DELIM);
                int win2 = atoi(token);
                token = strtok(NULL, DELIM);
                int y = atoi(token);
                token = strtok(NULL, DELIM);
                int x = atoi(token);
                wmove(cwindows[win2], y, x);
                strcpy(buffer[win].at(win, i), preserve);
                }
                break;
            case 3:;
                // wmove_r 
                // args: win, dy,dx
                {
                char* preserve = strdup(buffer[win].at(win, i));
                char* token = strtok(buffer[win].at(win, i), DELIM);
                token = strtok(NULL, DELIM);
                int win2 = atoi(token);
                token = strtok(NULL, DELIM);
                int dy = atoi(token);
                token = strtok(NULL, DELIM);
                int dx = atoi(token);
                if(Y[win2] + dy < 0 || Y[win2] + dy > cROWS){
                    panic("dy out of bounds", EXIT_FAILURE);
                }
                if(X[win2] + dx < 0 || X[win2] + dx > cCOLS){
                    panic("dx out of bounds", EXIT_FAILURE);
                }
                wmove(cwindows[win2], Y[win2]+dy, X[win2]+dx);
                strcpy(buffer[win].at(win, i), preserve);
                }
                break;
            case 4:;
                // wmove_p
                // args: win, percenty,percentx
                {
                char* preserve = strdup(buffer[win].at(win, i));
                char* token = strtok(buffer[win].at(win, i), DELIM);
                token = strtok(NULL, DELIM);
                int win2 = atoi(token);
                token = strtok(NULL, DELIM);
                double py = atof(token);
                token = strtok(NULL, DELIM);
                double px = atof(token);
                if(py < 0 || py > 100){
                    panic("dy out of bounds", EXIT_FAILURE);
                }
                if(px < 0 || px > 100){
                    panic("dx out of bounds", EXIT_FAILURE);
                }
                wmove(cwindows[win2], (int)cROWS*py, (int)cCOLS*px);
                strcpy(buffer[win].at(win, i), preserve);
                }
                break;
            case 5:;
                //wrefresh
                //args: win
                {
                char* preserve = strdup(buffer[win].at(win, i));
                char* token = strtok(buffer[win].at(win, i), DELIM);
                token = strtok(NULL, DELIM);
                int win2 = atoi(token);
                wrefresh(cwindows[win2]);
                strcpy(buffer[win].at(win, i), preserve);
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
                char* preserve = strdup(buffer[win].at(win, i));
                //first token is opcode- throw away
                char* token = strtok(buffer[win].at(win, i), DELIM);
                token = strtok(NULL, DELIM);
                int win2 = atoi(token);
                token = strtok(NULL, DELIM);
                char* str = strdup(token);
                wprintw(cwindows[win2], "%s", str);
                //restore copy
                strcpy(buffer[win].at(win, i), preserve);
                }
                break;
            case 12:;
                // wprintw single decimal
                // args: win, dec
                {
                //strtok destroys original. Must preserve copy.
                char* preserve = strdup(buffer[win].at(win, i));
                //first token is opcode- throw away
                char* token = strtok(buffer[win].at(win, i), DELIM);
                token = strtok(NULL, DELIM);
                int win2 = atoi(token);
                token = strtok(NULL, DELIM);
                int dec = atoi(token);
                wprintw(cwindows[win2], "%d", dec);
                //restore copy
                strcpy(buffer[win].at(win, i), preserve);
                }
                break;
            case 13:;
                // wprintw single float
                // args: win, flt
                {
                //strtok destroys original. Must preserve copy.
                char* preserve = strdup(buffer[win].at(win, i));
                //first token is opcode- throw away
                char* token = strtok(buffer[win].at(win, i), DELIM);
                token = strtok(NULL, DELIM);
                int win2 = atoi(token);
                token = strtok(NULL, DELIM);
                double flt = atof(token);
                wprintw(cwindows[win2], "%f", flt);
                //restore copy
                strcpy(buffer[win].at(win, i), preserve);
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
                char* preserve = strdup(buffer[win].at(win, i));
                char* token = strtok(buffer[win].at(win, i), DELIM);
                token = strtok(NULL, DELIM);
                int win2 = atoi(token);
                token = strtok(NULL, DELIM);
                char ch = token[0];
                token = strtok(NULL, DELIM);
                int n = atoi(token);
                wvline(cwindows[win2], ch, n);
                strcpy(buffer[win].at(win, i), preserve);
                }
                break;
            case 22:;
                // whline
                // args: ch, n
                {
                char* preserve = strdup(buffer[win].at(win, i));
                char* token = strtok(buffer[win].at(win, i), DELIM);
                token = strtok(NULL, DELIM);
                int win2 = atoi(token);
                token = strtok(NULL, DELIM);
                char ch = token[0];
                token = strtok(NULL, DELIM);
                int n = atoi(token);
                whline(cwindows[win2], ch, n);
                strcpy(buffer[win].at(win, i), preserve);
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
            default:
                panic("opcode invalid", EXIT_FAILURE);
                break;
            }
        }
        if(win == 0){
            for(int i=WINDOWS_MAX-1; i>=0; i--){
                if(cwindows[i] == NULL){
                    continue;
                }
                wrefresh(cwindows[i]);
            }
        }
        else{
            wrefresh(cwindows[win]);
        }
    }
    else{
        //do nothing- screen tends to flicker when repainted too often.
        //Don't know why yet. Increasing timeout seems to help
    }
}
