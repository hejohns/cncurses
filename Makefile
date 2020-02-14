# keep -Wpedantic until module is considered stable

CXX := gcc
CXXFLAGS := -std=gnu11 -Wall -Wextra -ggdb

cncurses.exe: main.o cncurses.o screen_buffer.o cstring.o misc.o
	$(CXX) $(CXXFLAGS) -lncurses -Wpedantic $^ -o $@ 
	ctags -R
screen_buffer_tests.exe: screen_buffer_tests.o screen_buffer.o cncurses.o cstring.o
	$(CXX) $(CXXFLAGS) -Wpedantic -lncurses $^ -o $@
cstring_tests.exe: cstring_tests.o cstring.o
	$(CXX) $(CXXFLAGS) -Wpedantic $^ -o $@
main.o: main.c
	$(CXX) $(CXXFLAGS) -lncurses -Wpedantic $^ -c
	ctags -R
cncurses.o: cncurses.c
	$(CXX) $(CXXFLAGS) -Wpedantic $^ -c
	ctags -R
screen_buffer.o: screen_buffer.c
	$(CXX) $(CXXFLAGS) -Wpedantic $^ -c
	ctags -R
screen_buffer_tests.o: screen_buffer_tests.c
	$(CXX) $(CXXFLAGS) -Wpedantic -lncurses $^ -c
cstring.o: cstring.c
	$(CXX) $(CXXFLAGS) -Wpedantic $^ -c
	ctags -R
cstring_tests.o: cstring_tests.c
	$(CXX) $(CXXFLAGS) -Wpedantic $^ -c
misc.o: misc.c
	$(CXX) $(CXXFLAGS) -Wpedantic $^ -c
	ctags -R
clean:
	rm *.exe *.o
