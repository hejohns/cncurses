//main.c
#include "macros.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>
#include "cncurses.h"
#include "screen_buffer.h"
#include "cstring.h"


/* GDB's readline does not play well. For now, autocomplete
 * is destroyed. We will figure something out to reenable...
 */

screen_buffer win1, win2, win3, win4;
static const int cstrInitSize = 100;


int main(int argc, char** argv){
    //check args for proper usage
    if(argc != 2){
        printf("Usage:\n%s <name of executable to debug>\n\tExecutable must be compiled with gdb \'-g\' flag\n", argv[0]);
        return EXIT_SUCCESS;
    }
    //begin{initialization}}
    sigwinch_initialize();
    //sigaction(SIGPIPE, &(struct sigaction){SIG_IGN}, NULL);
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
        //bt
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
        execlp("gdb", "gdb", "-nowindows", argv[1], (char*)NULL);
        return 0;
    }
    else{
        if(forkreturn2 < 0){
            panic("fork2 failed", EXIT_FAILURE);
        }
        else if(forkreturn2 == 0){
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
            execlp("gdb", "gdb", "-nowindows", argv[1], (char*)NULL);
            return 0;
        }
        else{
            waitpid(forkreturn1, NULL, WNOHANG);
            waitpid(forkreturn2, NULL, WNOHANG);
            goto parent;
        }
    }
parent:;
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
    timeout(100);
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
    win1.ptr = cnewwin((1-(3.0/cROWS)), (.5-(3.0/cROWS)), 0, 0);
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
    clearok(win1.ptr, true);
    clearok(win2.ptr, true);
    clearok(win3.ptr, true);
    clearok(win4.ptr, true);
    immedok(win1.ptr, false);
    immedok(win2.ptr, false);
    immedok(win3.ptr, false);
    immedok(win4.ptr, false);
    scrollok(win1.ptr, true);
    scrollok(win2.ptr, true);
    scrollok(win3.ptr, true);
    scrollok(win4.ptr, true);
    //end{initialization}
    //begin{body]
    fprintf(stdin_gdb1w, "set confirm off\n");
    fflush(stdin_gdb1w);
    fprintf(stdin_gdb2w, "set confirm off\n");
    fflush(stdin_gdb2w);
{
    int ch;
    flushinp();
    //resetting terminal seems to reduce flickering
    endwin();
    system("reset");
    char* win1_cstr1;
    char win1_cstr2[cstrInitSize];
    win1_cstr1 = cstringInit(&win1_cstr1, cstrInitSize);
    char* win2_cstr1;
    char win2_cstr2[cstrInitSize];
    win2_cstr1 = cstringInit(&win2_cstr1, cstrInitSize);
    char* win4_cstr;
    cstringInit(&win4_cstr, cstrInitSize);
    char* lastCmd;
    cstringInit(&lastCmd, cstrInitSize);
    while((ch = getch()) != EXIT_KEY){
        switch(ch){
            case RESET_KEY:;
                endwin();
                system("reset");
                break;
            case KEY_LEFT:;
                break;
            case KEY_UP:;
                break;
            case KEY_DOWN:;
                break;
            case KEY_RIGHT:;
                break;
            case KEY_BACKSPACE:;
                if(call(&win4, size) == 0){
                    //do nothing
                }
                else{
                    win4_cstr[strlen(win4_cstr-1)] = '\0';
                    char* circFile;
                    circFile = call(&win4, pop);
                    free(circFile);
                }
                break;
            case '\n':;
                 //KEY_ENTER does not seem to work as expected
                 if(win4_cstr[0] == '\0'){
                     fprintf(stdin_gdb1w, "%s", lastCmd);
                 }
                 else{
                     fprintf(stdin_gdb1w, "%s", win4_cstr);
                     cstringSprintf(&lastCmd, "%s", win4_cstr);
                 }
                 fflush(stdin_gdb1w);
                 call(&win4, cclear);
                 fprintf(stdin_gdb2w, "%s\necho\n\n\n", win4_cstr);
                 fflush(stdin_gdb2w);
                 win4_cstr[0] = '\0';
                 break;
            case ERR:;
                break;
            default:;
                //alphanumerics
                cwprintw(&win4, "%c", ch);
                char dummy[2];
                dummy[0] = ch;
                dummy[1] = '\0';
                cstringStrcat(&win4_cstr, dummy);
                break;
        }
        cresizeterm(4, &win1, &win2, &win3, &win4);
        {
            //print stdout of gdb1 to win1
            win1_cstr1[0] = '\0';
            win1_cstr2[0] = '\0';
            while(fgets(win1_cstr2, cstrInitSize, stdout_gdb1r) != NULL){
                cstringStrcat(&win1_cstr1, win1_cstr2);
            }
            if(strlen(win1_cstr1) > 0){
                cwprintw(&win1, win1_cstr1);
                call(&win1, repaint);
            }
        }
        {
            //print bt to win2
            call(&win2, cclear);
            fprintf(stdin_gdb2w, "bt");
            fflush(stdin_gdb2w);
            win2_cstr1[0] = '\0';
            win2_cstr2[0] = '\0';
            while(fgets(win2_cstr2, cstrInitSize, stdout_gdb2r) != NULL){
                if(win2_cstr2[0] == '('){
                    cstringStrcat(&win2_cstr1, "\n");
                }
                cstringStrcat(&win2_cstr1, win2_cstr2);
            }
            if(strlen(win2_cstr1) > 0){
                cwprintw(&win2, win2_cstr1);
                call(&win2, repaint);
            }
        }
        {
            //print info breakpoints to win3
            call(&win3, cclear);
            fprintf(stdin_gdb2w, "info breakpoints");
            fflush(stdin_gdb2w);
            win2_cstr1[0] = '\0';
            win2_cstr2[0] = '\0';
            while(fgets(win2_cstr2, cstrInitSize, stdout_gdb2r) != NULL){
                if(win2_cstr2[0] == '('){
                    cstringStrcat(&win2_cstr1, "\n");
                }
                cstringStrcat(&win2_cstr1, win2_cstr2);
            }
            if(strlen(win2_cstr1) > 0){
                cwprintw(&win3, win2_cstr1);
                call(&win3, repaint);
            }
        }
        call(&win4, repaint);
    }
    cstringFree(&win1_cstr1);
    cstringFree(&win2_cstr1);
    cstringFree(&win4_cstr);
    cstringFree(&lastCmd);
}
    //end{body}}
    //begin{termination}
    cwattroff(win1, COLOR_PAIR(1));
    cwattroff(win2, COLOR_PAIR(2));
    cwattroff(win3, COLOR_PAIR(3));
    cwattroff(win4, COLOR_PAIR(4));
    call(&win1, free);
    call(&win2, free);
    call(&win3, free);
    call(&win4, free);
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
