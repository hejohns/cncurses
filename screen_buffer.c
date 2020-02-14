//screen_buffer.c
#include "macros.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <curses.h>
#include "screen_buffer.h"


extern int cROWS, cCOLS;
static const char EOR = -10;

void screen_buffer_push(screen_buffer* win, char* cmd){
    /* OK
     * surprisingly, using static variables to store "call2(win, at, -1)" 
     * has almost no effect on runtime, even when pushing thousands of
     * times sequentially.
     */
    if(call(win, size) == 0){
        strcpy(win->queue, cmd);
        *(win->queue+strlen(cmd)+1) = EOR;
    }
    else{
        char* nextAvail = call2(win, at, call(win, size)-1);
        while(true){
            if(*nextAvail == EOR){
                nextAvail++;
                break;
            }
            nextAvail++;
        }
        if((nextAvail-win->queue)+strlen(cmd)+2 > win->queue_size/1.5){
            char* tmp = realloc(win->queue, (win->queue_size + strlen(cmd)+2)*2);
            if(tmp == NULL){
                fprintf(stderr, "warning: realloc failed. Attemping to continue, "\
                    "%s, %s, %d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
            }
            else{
                nextAvail = (nextAvail-win->queue)+tmp;
                win->queue = tmp;
                win->queue_size = (win->queue_size + strlen(cmd)+2)*2;
            }
        }
        strcpy(nextAvail, cmd);
        *(nextAvail+strlen(cmd)+1) = EOR;
    }
    win->rows++;
}

char* screen_buffer_pop(screen_buffer* win){
    /* OK
     */
    if(call(win, size) == 0) panic2("buffer is empty--nothing to pop", EXIT_FAILURE);
    char* ret = strdup(call2(win, at, call(win, size)-1));
    win->rows--;
    return ret;
}

size_t screen_buffer_size(screen_buffer* win){
    /* OK
     */
    return win->rows;
}

char* screen_buffer_at(screen_buffer* win, int index){
    /* OK
     * Since .at is oft called sequentially (when repainting), 
     * store lastPos to reduce recalculation.
     *
     * lastWin used  as sanity check to verify .at
     * is being used properly.
     */
    static screen_buffer* lastWin;
    static char* lastPos;
    if(index >= (int)call(win, size)) panic2("index out of range", EXIT_FAILURE);
    char* ret = NULL;
    if(index == -1){
        if(lastWin!=win) panic2("incorrect usage of index=-1", EXIT_FAILURE);
        for(char* i=lastPos; ; i++){
            if(*i == EOR){
                ret = i+1;
                break;
            }
        }
    }
    else if(index == 0){
        ret = win->queue;
    }
    else{
        int counter = 0;
        for(char* i=win->queue; ; i++){
            if(*i == EOR){
                counter++;
            }
            if(counter==index && index>=0){
                ret = i+1;
                break;
            }
        }
    }
    while(true){
        if(32<=*ret && *ret<=126){
            break;
        }
        else{
            ret++;
        }
    }
    lastWin = win;
    lastPos = ret;
    return ret;
}

void screen_buffer_clear(screen_buffer* win){
    /* OK
     * return queue size back to SCREEN_BUFFER_QUEUE_INITIAL
     * may decide against this later
     *
     * Supposedly, realloc is faster than free+malloc
     * https://stackoverflow.com/questions/1401234/differences-between-using-realloc-vs-free-malloc-functions
     */
    win->rows = (size_t)0;
    char* tmp = realloc(win->queue, SCREEN_BUFFER_QUEUE_INITIAL);
    if(tmp == NULL){
        fprintf(stderr, "warning: realloc failed. Using more memory than needed, "\
            "%s, %s, %d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    }
    else{
        win->queue = tmp;
        win->queue_size = (size_t)SCREEN_BUFFER_QUEUE_INITIAL;
    }
}

void screen_buffer_erase(screen_buffer* win, size_t index){
    /* OK
     */
    if(index >= call(win, size)) panic2("index out of range", EXIT_FAILURE);
    char* start = call2(win, at, index);
    for(size_t i=0; ; i++){
        if(*(start+i) == EOR){
            *(start+i) = '\0';
            break;
        }
        else{
            *(start+i) = '\0';
        }
    }
    win->rows--;
}

static int opcode(char* row){
    /* OK
     * helper func
     */
    return 100*(row[0]-'0') + 10*(row[1]-'0') + (row[2]-'0');
}

void screen_buffer_repaint(screen_buffer* win){
    /* in constant flux
     */
    wclear(win->ptr);
    for(size_t i=0; i<call(win, size); i++){
        char* rowi;
        if(i == 0){
            rowi = call2(win, at, 0);
        }
        else{
            rowi = call2(win, at, -1);
        }
        switch(opcode(rowi)){
            case 0:
                //wprintw delimeter
                wprintw(win->ptr, DELIM);
                break;
            case 2:;
                // wmove
                // args: y,x
                {
                char* preserve = strdup(rowi);
                char* token = strtok(rowi, DELIM);
                token = strtok(NULL, DELIM);
                int y = atoi(token);
                token = strtok(NULL, DELIM);
                int x = atoi(token);
                wmove(win->ptr, y, x);
                strcpy(rowi, preserve);
                free(preserve);
                }
                break;
            case 3:;
                // wmove_r 
                // args: dy,dx
                {
                char* preserve = strdup(rowi);
                char* token = strtok(rowi, DELIM);
                token = strtok(NULL, DELIM);
                int dy = atoi(token);
                token = strtok(NULL, DELIM);
                int dx = atoi(token);
                int y, x;
                getyx(win->ptr, y, x);
                if(y + dy < 0 || y + dy > cROWS){
                    panic2("dy out of bounds", EXIT_FAILURE);
                }
                if(x + dx < 0 || x + dx > cCOLS){
                    panic2("dx out of bounds", EXIT_FAILURE);
                }
                wmove(win->ptr, y+dy, x+dx);
                strcpy(rowi, preserve);
                free(preserve);
                }
                break;
            case 4:;
                // wmove_p
                // args: percenty,percentx
                {
                char* preserve = strdup(rowi);
                char* token = strtok(rowi, DELIM);
                token = strtok(NULL, DELIM);
                double py = atof(token);
                token = strtok(NULL, DELIM);
                double px = atof(token);
                if(py < 0 || py > 100){
                    panic2("dy out of bounds", EXIT_FAILURE);
                }
                if(px < 0 || px > 100){
                    panic2("dx out of bounds", EXIT_FAILURE);
                }
                wmove(win->ptr, (int)cROWS*py, (int)cCOLS*px);
                strcpy(rowi, preserve);
                free(preserve);
                }
                break;
            case 10:
                break;
            case 11:;
                // wprintw single string
                // args: str
                {
                //strtok destroys original. Must preserve copy.
                char* preserve = strdup(rowi);
                //first token is opcode- throw away
                char* token = strtok(rowi, DELIM);
                token = strtok(NULL, DELIM);
                char* str = strdup(token);
                wprintw(win->ptr, "%s", str);
                //restore copy
                strcpy(rowi, preserve);
                free(preserve);
                }
                break;
            case 12:;
                // wprintw single decimal
                // args: dec
                {
                //strtok destroys original. Must preserve copy.
                char* preserve = strdup(rowi);
                //first token is opcode- throw away
                char* token = strtok(rowi, DELIM);
                token = strtok(NULL, DELIM);
                int dec = atoi(token);
                wprintw(win->ptr, "%d", dec);
                //restore copy
                strcpy(rowi, preserve);
                free(preserve);
                }
                break;
            case 13:;
                // wprintw single float
                // args: flt
                {
                //strtok destroys original. Must preserve copy.
                char* preserve = strdup(rowi);
                //first token is opcode- throw away
                char* token = strtok(rowi, DELIM);
                token = strtok(NULL, DELIM);
                double flt = atof(token);
                wprintw(win->ptr, "%f", flt);
                //restore copy
                strcpy(rowi, preserve);
                free(preserve);
                }
                break;
            case 20:
                // box
                // args:
                box(win->ptr, 0, 0);
                break;
            case 21:;
                // wvline
                // args: ch, n
                {
                char* preserve = strdup(rowi);
                char* token = strtok(rowi, DELIM);
                token = strtok(NULL, DELIM);
                char ch = token[0];
                token = strtok(NULL, DELIM);
                int n = atoi(token);
                wvline(win->ptr, ch, n);
                strcpy(rowi, preserve);
                free(preserve);
                }
                break;
            case 22:;
                // whline
                // args: ch, n
                {
                char* preserve = strdup(rowi);
                char* token = strtok(rowi, DELIM);
                token = strtok(NULL, DELIM);
                char ch = token[0];
                token = strtok(NULL, DELIM);
                int n = atoi(token);
                whline(win->ptr, ch, n);
                strcpy(rowi, preserve);
                free(preserve);
                }
                break;
            default:
                panic2("opcode invalid", EXIT_FAILURE);
                break;
        }
    }
    wrefresh(win->ptr);
}

void screen_buffer_free(screen_buffer* win){
    /* self-explanatory
     */
    free(win->queue);
}
