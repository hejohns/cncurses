//main.c
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include "macros.h"
#include "cncurses.h"

int main(int argc, char** argv){
    //check args for proper usage
    if(argc != 2){
        printf("Usage:\n%s <name of executable to debug>\n\tExecutable must be compiled with gdb \'-g\' flag\n", argv[0]);
        return EXIT_SUCCESS;
    }
    //begin{initialization}}
    sigwinch_initialize();
    int fd1[2], fd2[2], fd3[2], fd4[4];
    if(pipe(fd1) || pipe(fd2) || pipe(fd3) || pipe(fd4)){
        panic("failed to create pipes", EXIT_FAILURE);
    }
    for(int i=0; i<2; i++){
        int flags1 = fcntl(fd1[i], F_GETFL, 0);
        flags1 = flags1 | O_NONBLOCK;
        fcntl(fd1[i], F_SETFL, flags1);
        int flags2 = fcntl(fd2[i], F_GETFL, 0);
        flags2 = flags2 | O_NONBLOCK;
        fcntl(fd2[i], F_SETFL, flags2);
        int flags3 = fcntl(fd3[i], F_GETFL, 0);
        flags3 = flags3 | O_NONBLOCK;
        fcntl(fd3[i], F_SETFL, flags3);
        int flags4 = fcntl(fd4[i], F_GETFL, 0);
        flags4 = flags4 | O_NONBLOCK;
        fcntl(fd4[i], F_SETFL, flags4);
    }
    pid_t forkreturn1 = fork();
    pid_t forkreturn2 = fork();
    if(forkreturn1 < 0){
        panic("fork1 failed", EXIT_FAILURE);
    }
    else if(forkreturn1 == 0){
<<<<<<< HEAD
        //bt full
        //close unused fds
        close(fd1[0]);
        close(fd2[1]);
        close(fd3[0]);
        close(fd3[1]);
        close(fd4[0]);
        close(fd4[1]);
        fclose(stdout);
        dup2(fd1[1], 1);
        fclose(stderr);
        fopen("/dev/null", "w");
        fclose(stdin);
        dup2(fd2[0], 0);
        printf("test\n");
        execlp("gdb", "gdb", argv[1], (char*)NULL);
=======
        execlp("gdb", "gdb", (char*)NULL);
>>>>>>> didnt realize i was on master
        return 0;
    }
    else{
        if(forkreturn2 < 0){
            panic("fork2 failed", EXIT_FAILURE);
        }
        else if(forkreturn2 == 0){
<<<<<<< HEAD
            //info b
            //close unused fds
            close(fd1[0]);
            close(fd1[1]);
            close(fd2[0]);
            close(fd2[1]);
            close(fd3[0]);
            close(fd4[1]);
            fclose(stdout);
            dup2(fd3[1], 1);
            fclose(stderr);
            fopen("/dev/null", "w");
            fclose(stdin);
            dup2(fd4[0], 0);
            execlp("gdb", "gdb", argv[1], (char*)NULL);
=======
            execlp("gdb", "gdb", (char*)NULL);
>>>>>>> didnt realize i was on master
            return 0;
        }
        else{
            waitpid(forkreturn1, NULL, WNOHANG);
            waitpid(forkreturn2, NULL, WNOHANG);
            goto parent;
        }
    }
parent:;
#pragma GCC diagnostic ignored "-Wunused-variable"
    //close unused fds
    close(fd1[1]);
    close(fd2[0]);
    close(fd3[1]);
    close(fd4[0]);
    FILE* stdout_gdb1r = fdopen(fd1[0], "r");
    FILE* stdin_gdb1w = fdopen(fd2[1], "w");
    FILE* stdout_gdb2r = fdopen(fd3[0], "r");
    FILE* stdin_gdb2w = fdopen(fd4[1], "w");
    initscr();
    timeout(50);
    curs_set(0);
    getmaxyx(stdscr, cROWS, cCOLS);
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    cstart_color();
    //order matters; 1 means win1, 2 means win2, ...
    cinit_pair(1, COLOR_WHITE, COLOR_BLACK);
    cinit_pair(2, COLOR_BLUE, COLOR_BLACK);
    cinit_pair(3, COLOR_YELLOW, COLOR_BLACK);
    cinit_pair(4, COLOR_GREEN, COLOR_BLACK);
    //cnewwin is a macro; be careful with parenthesis
    win1.ptr = cnewwin((1-(3.0/cROWS)), .5, 0, 0);
    win2.ptr = cnewwin(.8, .5, 0, .5);
    win3.ptr = cnewwin(.2, .5, .8, .5);
    win4.ptr = cnewwin((3.0/cROWS), .5, (1-(3.0/cROWS)), 0);
    //first arg to cinit is number of windows
    cinit(4, &win1, &win2, &win3, &win4);
    cwattron(win1, COLOR_PAIR(1));
    cwattron(win2, COLOR_PAIR(2));
    cwattron(win3, COLOR_PAIR(3));
    cwattron(win4, COLOR_PAIR(4));
    clear();
    refresh();
    //end{initialization}
    //begin{body]
<<<<<<< HEAD
    box(win1.ptr, 0, 0);
    box(win2.ptr, 0, 0);
    box(win3.ptr, 0, 0);
    box(win4.ptr, 0, 0);
{
    int ch;
    flushinp();
    while((ch = getch()) != EXIT_KEY){
        switch(ch){
            case KEY_LEFT:;
                break;
            case KEY_UP:;
                break;
            case KEY_RIGHT:;
                break;
            case KEY_BACKSPACE:;
                break;
            case '\n':;
                break;
            case ERR:;
                break;
            default:;
                //alphanumerics
                break;
        }
    wrefresh(win1.ptr);
    wrefresh(win2.ptr);
    wrefresh(win3.ptr);
    wrefresh(win4.ptr);
    }
}
=======
    //cvline('|', 20);
    //cmove_p(.50,.50);
    //cgetyx();
    //c000(1);
    //cprintw("%s", "Hello");
    //cmove_r(1, 0);
    //cprintw("%s", "cruel");
    //cmove_r(2, 0);
    //cprintw("%s", "world");
    //cmove_r(3, 0);
    //cprintw("%d-%d,", 10, 20);
    //cprintw("%f-%f", .100, .200);
    //crefresh();
    refresh();
    echo();
    //cclear();
    while(true){}
>>>>>>> didnt realize i was on master
    //end{body}}
    //begin{termination}
    cwattroff(win1, COLOR_PAIR(1));
    cwattroff(win2, COLOR_PAIR(2));
    cwattroff(win3, COLOR_PAIR(3));
    cwattroff(win4, COLOR_PAIR(4));
    endwin();
    if(kill(forkreturn1, SIGTERM) || kill(forkreturn2, SIGTERM)){
        kill(forkreturn1, SIGKILL);
        kill(forkreturn2, SIGKILL);
        system("reset");
        panic("Could not SIGTERM children. SIGKILLed them", EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
    //end{termination}
}
