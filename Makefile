cncurses: main.c cncurses.h cncurses.c screen_buffer.h screen_buffer.c misc.h misc.c macros.h
	gcc -std=gnu11 -lncurses -Wpedantic -Wall -Werror -g main.c cncurses.c screen_buffer.c misc.c -o cncurses
	ctags -R

clean:
	rm cncurses
