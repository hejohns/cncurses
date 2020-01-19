#define str(...) #__VA_ARGS__

#define panic(mesg, exit_code) fprintf(stderr, "Fatal: "mesg", %s, %d", __FILE__, __LINE__); exit(exit_code)

#define cgetyx() buffer.push("001")
#define cmove_a(...) buffer.push("010,"str(__VA_ARGS__))
#define cprintw_1(...) buffer.push("011,"str(__VA_ARGS__))
#define cprintw_2(...) buffer.push("012,"str(__VA_ARGS__))
#define cprintw_3(...) buffer.push("013,"str(__VA_ARGS__))
#define cmove_r(...) buffer.push("019,"str(__VA_ARGS__))
#define cmove_p(...) buffer.push("020,"str(__VA_ARGS__))
#define cmvprintw_1(...) buffer.push("021,"str(__VA_ARGS__))
#define cmvprintw_2(...) buffer.push("022,"str(__VA_ARGS__))
#define cmvprintw_3(...) buffer.push("023,"str(__VA_ARGS__))

#define HAS_COLOR 1
#ifdef HAS_COLOR

#define START_COLOR() if(has_colors())\
    start_color()

#define INIT_PAIR(...) if(has_colors())\
    init_pair(__VA_ARGS__)

#define ATTRON(x) if(has_colors())\
    attron(x)

#define ATTROFF(x) if(has_colors())\
    attroff(x)

#endif
