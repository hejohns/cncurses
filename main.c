//main.c
#include "macros.h"
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
#include "cncurses.h"

int main(int argc, char** argv){
    //begin{initialization}}
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
    pid_t forkreturn2;
    if(forkreturn1 < 0){
        panic("fork1 failed", EXIT_FAILURE);
    }
    else if(forkreturn1 == 0){
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
        return 0;
    }
    else{
        forkreturn2 = fork();
        if(forkreturn2 < 0){
            panic("fork2 failed", EXIT_FAILURE);
        }
        else if(forkreturn2 == 0){
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
            return 0;
        }
        else{
            goto parent;
        }
    }
parent:;
#pragma GCC diagnostic ignored "-Wunused-variable"
    close(fd1[1]);
    close(fd2[0]);
    close(fd3[1]);
    close(fd4[0]);
    FILE* stdout_gdb1r = fdopen(fd1[0], "r");
    FILE* stdin_gdb1w = fdopen(fd2[1], "w");
    fprintf(stdin_gdb1w, "test2\n");
    FILE* stdout_gdb2r = fdopen(fd3[0], "r");
    FILE* stdin_gdb2w = fdopen(fd4[1], "w");
    waitpid(forkreturn1, NULL, WNOHANG);
    waitpid(forkreturn2, NULL, WNOHANG);
    sigwinch_initialize();
    initscr();
    curs_set(0);
    getmaxyx(stdscr, cROWS, cCOLS);
    timeout(50);
    cstart_color();
    //order matters; 1 means win1, 2 means win2, ...
    cinit_pair(1, COLOR_GREEN, COLOR_BLACK);
    cinit_pair(2, COLOR_BLUE, COLOR_BLACK);
    cinit_pair(3, COLOR_WHITE, COLOR_BLACK);
    cinit_pair(4, COLOR_YELLOW, COLOR_BLACK);
    //cnewwin is a macro; be careful with parenthesis
    WINDOW* win1 = cnewwin((1-(2.0/cROWS)), .5, 0, 0);
    WINDOW* win2 = cnewwin(.5, .5, 0, .5);
    WINDOW* win3 = cnewwin(.5, .5, .5, .5);
    WINDOW* win4 = cnewwin((1.0/cROWS), .5, (1-(2.0/cROWS)), 0);
    //first arg to cinit is number of windows
    cinit(4, win1, win2, win3, win4);
    cwattron(1, COLOR_PAIR(1));
    cwattron(2, COLOR_PAIR(2));
    cwattron(3, COLOR_PAIR(3));
    cwattron(4, COLOR_PAIR(4));
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    clear();
    refresh();
    cwprintw(4, "%s", "hi");
    cwinch = true;
    buffer[0].repaint(0);
    //end{initialization}
    //begin{body]
{
    int ch;
    flushinp();
    while((ch = getch()) != EXIT_KEY){
        switch(ch){
            case KEY_LEFT:
                cgetyx(3);
                cwmove_r(3, Y[3]+1, X[3]-1);
                break;
            case KEY_UP:
                raise(SIGWINCH);
                //cwinch = true;
                break;
            case KEY_RIGHT:;
                char str10[2];
                str10[0] = fgetc(stdout_gdb1r);
                if(str10[0] == EOF){
                    cwprintw(2, "%s", "L");
                    break;
                }
                str10[1] = '\0';
                cwprintw(2, "%s", str10);
                break;
            case KEY_BACKSPACE:;
                char nil[BUFFER_COLS_MAX];
                buffer[1].pop(1, nil);
                fprintf(stdin_gdb1w, "%c", '\b');
                fflush(stdin_gdb1w);
                break;
            case ERR:
                cwinch = true;
                buffer[0].repaint(0);
                cwinch = true;
                buffer[1].repaint(1);
                cwinch = true;
                buffer[2].repaint(2);
                cwinch = true;
                buffer[3].repaint(3);
                cwinch = true;
                buffer[4].repaint(4);
                break;
            default:;
                char str[2];
                str[0] = ch;
                str[1] = '\0';
                if(isascii(ch)){
                    cwprintw(1, "%s", str);
                    fprintf(stdin_gdb1w, "%c", ch);
                    if(ch == '\n'){
                        fflush(stdin_gdb1w);
                        cgetyx(1);
                        //cwmove(1, Y[1], X[1]);
                    }
                    else{
                    }
                }
                //refresh();
                //ungetch(ch);
                //buffer.repaint();
                //ch = wgetch(win1);
                break;
        }
        char win2_buf[2];
        win2_buf[0] = fgetc(stdout_gdb1r);
        if(!(win2_buf[0] == EOF)){
            win2_buf[1] = '\0';
            cwprintw(2, "%s", win2_buf);
        }
        char win3_buf[2];
        win3_buf[0] = fgetc(stdout_gdb2r);
        if(!(win3_buf[0] == EOF)){
            win3_buf[1] = '\0';
            cwprintw(3, "%s", win3_buf);
        }
    
    }
}
    //end{body}}
    //begin{termination}
    cwattroff(1, COLOR_PAIR(1));
    cwattroff(2, COLOR_PAIR(2));
    cwattroff(3, COLOR_PAIR(3));
    cwattroff(4, COLOR_PAIR(4));
    endwin();
    if(kill(forkreturn1, SIGTERM) || kill(forkreturn2, SIGTERM)){
        kill(forkreturn1, SIGKILL);
        kill(forkreturn2, SIGKILL);
        panic("Could not SIGTERM children. SIGKILLed them", EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
    //end{termination}
}
