cncurses: main.c cncurses.h cncurses.c misc.h misc.c macros.h
	gcc -std=gnu11 -lncurses -Wpedantic -Wall -Werror -g main.c cncurses.c misc.c -o cncurses
	ctags -R

clean:
	rm cncurses
