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
/* terrible practice but... */
#define STR(x) #x
#define STUFF_ARROW_KEY(cstr, arr) \
    do{\
        cstringSprintf(cstr, "screen -S %s -p 0 -X stuff '%s'", argv[1], STR(arr));\
        system(*cstr);\
        cstringSprintf(cstr, "screen -S %s-%d -p 0 -X stuff '%s'", argv[1], uniq, STR(arr));\
        system(*cstr);\
    } while(false)

static size_t cstring_size(char* ptr){
    size_t* tmp = (size_t*)(ptr-sizeof(size_t));
    return *tmp;
}

char* cstringInit(char** ptr, size_t size){
    *ptr = calloc(sizeof(size_t)+size+1, 1);
    size_t* tmp = (size_t*)*ptr;
    *tmp = size;
    *ptr += sizeof(size_t);
    return *ptr;
}

void cstringFree(char** ptr){
    free(*ptr-sizeof(size_t));
    *ptr = NULL;
}

int cstringVsprintf(char** ptr, const char* format, va_list ap){
/* https://stackoverflow.com/questions/37947200/c-variadic-wrapper
 */
    va_list args2;
    va_copy(args2, ap);
    int len = vsnprintf(*ptr, cstring_size(*ptr), format, ap);
    if(len > (int)cstring_size(*ptr)){
        char* tmp = *ptr-sizeof(size_t);
        char* tmp2 = realloc(tmp, sizeof(size_t)+len+1);
        if(tmp == NULL){
            fprintf(stderr, "could not allocate memory for string. Attmempting to continue\n");
        }
        else{
            tmp = tmp2;
            *(size_t*)tmp = len;
            *ptr = tmp2+sizeof(size_t);
            vsnprintf(*ptr, len+1, format, args2);
        }
    }
    va_end(args2);
    return len;
}

int cstringSprintf(char** ptr, const char* format, ...){
/* https://stackoverflow.com/questions/37947200/c-variadic-wrapper
 */
    va_list args, args2;
    va_start(args, format);
    va_copy(args2, args);
    int len = vsnprintf(*ptr, cstring_size(*ptr), format, args);
    va_end(args);
    if(len > (int)cstring_size(*ptr)){
        char* tmp = *ptr-sizeof(size_t);
        char* tmp2 = realloc(tmp, sizeof(size_t)+len+1);
        if(tmp == NULL){
            fprintf(stderr, "could not allocate memory for string. Attmempting to continue\n");
        }
        else{
            tmp = tmp2;
            *(size_t*)tmp = len;
            *ptr = tmp2+sizeof(size_t);
            vsnprintf(*ptr, len+1, format, args2);
        }
    }
    va_end(args2);
    return len;
}

char* cstringStrcat(char** dest, char* src){
    char* destCpy = strdup(*dest);
    cstringSprintf(dest, "%s%s", destCpy, src);
    free(destCpy);
    return *dest;
}

int main(int argc, char** argv){
    if(argc != 3){
        fprintf(stderr, "incorrect usage\n");
        exit(EXIT_FAILURE);
    }
    unsigned int uniq = time(NULL);
    char* cstr;
    cstringInit(&cstr, 80);
    cstringSprintf(&cstr, "screen -S %s-%d -dm bash -i", argv[1], uniq);
    system(cstr);
    cstringSprintf(&cstr, "screen -S %s-%d -p 0 -X stuff '%s %s 2>&1 | tee ./bt.tmp > ./infob.tmp\n'", argv[1], uniq, "gdb", argv[2]);
    system(cstr);
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
    int ch = 0;
    while((ch = getch()) != EXIT_KEY){
        if(ch == KEY_RESIZE){
            //ignore
        }
        else if(ch == KEY_BACKSPACE){
            STUFF_ARROW_KEY(&cstr, );
        }
        else if(ch == KEY_LEFT){
            STUFF_ARROW_KEY(&cstr, OD);
        }
        else if(ch == KEY_RIGHT){
            STUFF_ARROW_KEY(&cstr, OC);
        }
        else if(ch == KEY_UP){
            STUFF_ARROW_KEY(&cstr, OA);
        }
        else if(ch == KEY_DOWN){
            STUFF_ARROW_KEY(&cstr, OB);
        }
        else if(ch == '\n'){
            STUFF_ARROW_KEY(&cstr, \n);
            /*
            close(btfd);
            remove("./bt.tmp");
            if(btfd = open("./bt.tmp", O_CREAT | O_TRUNC | O_WRONLY | O_SYNC, 0644), btfd == -1){
                fprintf(stderr, "open() failed\n");
                endwin();
                exit(EXIT_FAILURE);
            }
            */
            ftruncate(btfd, 0);
            cstringSprintf(&cstr, "screen -S %s-%d -p 0 -X stuff '%s'", argv[1], uniq, "bt\ninfo b\n");
            system(cstr);
        }
        else{
            cstringSprintf(&cstr, "screen -S %s -p 0 -X stuff '%c'", argv[1], (char)ch);
            system(cstr);
            cstringSprintf(&cstr, "screen -S %s-%d -p 0 -X stuff '%c'", argv[1], uniq, (char)ch);
            system(cstr);
        }
    }
    endwin();
    cstringSprintf(&cstr, "screen -S %s-%d -X quit", argv[1], uniq);
    system(cstr);
    cstringSprintf(&cstr, "screen -S %s -X quit", argv[1]);
    system(cstr);
    cstringFree(&cstr);
    close(btfd);
    close(infobfd);
    remove("./bt.tmp");
    remove("./infob.tmp");
    return 0;
}
