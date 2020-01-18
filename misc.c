//misc.c
#include "misc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void crash(const char* message, int exit_status){
    if(strlen(message)>40){
        fprintf(stderr, "Fatal: message too long, %s, %d\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
    char format[80];
    strcat(format, "Fatal: ");
    strcat(format, message);
    strcat(format, ", %s, %d\n");
    fprintf(stderr, message, __FILE__, __LINE__);
    exit(exit_status);
}
