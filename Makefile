curtex: main.c curtex.c misc.c
	gcc -lncurses -Wpedantic -Wall -Werror -g main.c curtex.c misc.c -o curtex
	ctags -R

clean:
	rm curtex
