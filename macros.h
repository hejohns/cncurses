//macros.h
#ifndef MACROS_H
#define MACROS_H

#define HAS_COLOR 1
#define DELIM "^"
#define SCREEN_BUFFER__ROWS_MAX 8000
#define SCREEN_BUFFER_COLS_MAX 100
#define SCREEN_BUFFER_QUEUE_INITIAL SCREEN_BUFFER_ROWS_MAX*SCREEN_BUFFER_COLS_MAX/2
#define EXIT_KEY KEY_SHOME
#define RESET_KEY KEY_SEND

//used to call function pointers in structs (passes name arg into function automatically)
#define call(name, function) (name)->function(name)
#define call2(name, function, ...) (name)->function(name, __VA_ARGS__)


#define panic(mesg, exit_code) fprintf(stderr, "fatal: "mesg", %s, %d", __FILE__, __LINE__); exit(exit_code)
#define panic2(mesg, exit_code) fprintf(stderr, "fatal: "mesg", %s, %s, %d", __FILE__, __PRETTY_FUNCTION__, __LINE__); exit(exit_code)

#define cnewwin(y, x, sy, sx) newwin(y*cROWS, x*cCOLS, sy*cROWS, sx*cCOLS)


#ifdef HAS_COLOR

#define cstart_color() if(has_colors())\
    start_color()

#define cinit_pair(...) if(has_colors())\
    init_pair(__VA_ARGS__)

#define cwattron(win, pair) if(has_colors())\
    wattron(win.ptr,  pair);\

#define cwattroff(win, pair) if(has_colors())\
    wattroff(win.ptr, pair)

#elif

#define cstart_color() ;

#endif


#endif /* MACROS_H */
