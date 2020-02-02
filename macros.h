//macros.h
#ifndef MACROS_H
#define MACROS_H

#define str(...) #__VA_ARGS__

#define DELIM "^"
#define BUFFER_ROWS_MAX 8000
#define BUFFER_COLS_MAX 20
#define WINDOWS_MAX 5

#define EXIT_KEY KEY_SHOME

#define panic(mesg, exit_code) fprintf(stderr, "Fatal: "mesg", %s, %d", __FILE__, __LINE__); exit(exit_code)


#define cnewwin(a, b, c, d) newwin(a*cROWS, b*cCOLS, c*cROWS, d*cCOLS)

#define HAS_COLOR 1
#ifdef HAS_COLOR

#define cstart_color() if(has_colors())\
    start_color()

#define cinit_pair(...) if(has_colors())\
    init_pair(__VA_ARGS__)

#define cwattron(x, y) if(has_colors())\
    wattron(cwindows[x], y);\
    cwincolors[x] = y

#define cwattroff(x, y) if(has_colors())\
    wattroff(cwindows[x], y)

#endif

#endif /* MACROS_H */
