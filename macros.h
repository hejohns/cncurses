#define str(...) #__VA_ARGS__

#define panic(mesg, exit_code) fprintf(stderr, "Fatal: "mesg", %s, %d", __FILE__, __LINE__); exit(exit_code)


#define HAS_COLOR 1
#ifdef HAS_COLOR

#define cstart_color() if(has_colors())\
    start_color()

#define cinit_pair(...) if(has_colors())\
    init_pair(__VA_ARGS__)

#define cattron(x) if(has_colors())\
    attron(x)

#define catrroff(x) if(has_colors())\
    attroff(x)

#endif
