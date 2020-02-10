cncurses.exe: main.o cncurses.o screen_buffer.o misc.o macros.h
	gcc -std=gnu11 -lncurses -Wpedantic -Wall -Wextra -g main.o cncurses.o screen_buffer.o misc.o -o cncurses.exe
	ctags -R
main.o: main.c macros.h
	gcc -std=gnu11 -lncurses -Wpedantic -Wall -Wextra -c main.c
	ctags -R
cncurses.o: cncurses.h cncurses.c macros.h
	gcc -std=gnu11 -Wall -Wextra -c cncurses.c
	ctags -R
screen_buffer.o: screen_buffer.h screen_buffer.c macros.h
	gcc -std=gnu11 -Wall -Wextra -c screen_buffer.c
	ctags -R
misc.o: misc.h misc.c macros.h
	gcc -std=gnu11 -Wpedantic -Wall -Wextra -c misc.c
	ctags -R
screen_buffer_tests.exe: screen_buffer_tests.c screen_buffer.h screen_buffer.c cncurses.h cncurses.c
	gcc -std=gnu11 -lncurses -Wall -Wextra -g screen_buffer_tests.c screen_buffer.h screen_buffer.c cncurses.h cncurses.c -o screen_buffer_tests.exe
clean:
	rm *.exe *.o
