#define str(...) #__VA_ARGS__

#define DELIM "^"
#define BUFFER_ROWS_MAX 80
#define BUFFER_COLS_MAX 80
#define WINDOWS_MAX 5

#define panic(mesg, exit_code) fprintf(stderr, "Fatal: "mesg", %s, %d", __FILE__, __LINE__); exit(exit_code)


#define HAS_COLOR 1
#ifdef HAS_COLOR

#define cstart_color() if(has_colors())\
    start_color()

#define cinit_pair(...) if(has_colors())\
    init_pair(__VA_ARGS__)

#define cwattron(x, y) if(has_colors())\
    wattron(windows[x], y)

#define cwattroff(x, y) if(has_colors())\
    wattroff(windows[x], y)

#endif
