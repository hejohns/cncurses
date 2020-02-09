cncurses: main.c cncurses.h cncurses.c screen_buffer.h screen_buffer.c misc.h misc.c macros.h
	gcc -std=gnu11 -lncurses -Wpedantic -Wall -Wextra -g main.c cncurses.c screen_buffer.c misc.c -o cncurses
	ctags -R
tests: screen_buffer_tests.c screen_buffer.h screen_buffer.c cncurses.h cncurses.c
	gcc -std=gnu11 -lncurses -Wpedantic -Wall -Wextra -g screen_buffer_tests.c screen_buffer.h screen_buffer.c cncurses.h cncurses.c -o screen_buffer_tests
clean:
	rm cncurses
