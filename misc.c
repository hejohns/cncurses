//misc.c
#include "macros.h"
#include "misc.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int containsDelim(char* str){
    int ret = 0;
    char dummy[2] = DELIM;
    for(unsigned int i=0; i<strlen(str); i++){
        if(str[i] == dummy[0]){
            ret++;
        }
    }
    return ret;
}
