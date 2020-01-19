curtex: main.c curtex.h curtex.c misc.h misc.c macros.h
	gcc -lncurses -Wpedantic -Wall -Werror -g main.c curtex.c misc.c -o curtex
	ctags -R

clean:
	rm curtex
