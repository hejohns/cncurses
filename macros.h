//macros.h
#ifndef MACROS_H
#define MACROS_H

#define HAS_COLOR 1
#define DELIM "^"
#define BUFFER_ROWS_MAX 8000
#define BUFFER_COLS_MAX 100
#define EXIT_KEY KEY_SHOME
#define RESET_KEY KEY_SEND

#define str(...) #__VA_ARGS__

//used to call function pointers in structs (passes struct pointer automatically)
#define call(name, function) (name).function(&(name))
#define call2(name, function, ...) (name).function(&(name), __VA_ARGS__)


#define panic(mesg, exit_code) fprintf(stderr, "Fatal: "mesg", %s, %d", __FILE__, __LINE__); exit(exit_code)

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
