cncurses: main.c cncurses.h cncurses.c misc.h misc.c macros.h
	gcc -lncurses -Wpedantic -Wall -Werror -g main.c cncurses.c misc.c -o cncurses
	ctags -R

clean:
	rm cncurses
