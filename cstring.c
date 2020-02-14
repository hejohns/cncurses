//cstring.c
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


static size_t cstring_size(char* ptr){
    size_t* tmp = (size_t*)(ptr-sizeof(size_t));
    return *tmp;
}

char* cstringInit(char** ptr, size_t size){
    *ptr = calloc(sizeof(size_t)+size+1, 1);
    size_t* tmp = (size_t*)*ptr;
    *tmp = size;
    *ptr += sizeof(size_t);
    return *ptr;
}

void cstringFree(char** ptr){
    free(*ptr-sizeof(size_t));
    *ptr = NULL;
}

int cstringVsprintf(char** ptr, const char* format, va_list ap){
/* https://stackoverflow.com/questions/37947200/c-variadic-wrapper
 */
    va_list args2;
    va_copy(args2, ap);
    int len = vsnprintf(*ptr, cstring_size(*ptr), format, ap);
    if(len > (int)cstring_size(*ptr)){
        char* tmp = *ptr-sizeof(size_t);
        char* tmp2 = realloc(tmp, sizeof(size_t)+len+1);
        if(tmp == NULL){
            fprintf(stderr, "could not allocate memory for string. Attmempting to continue\n");
        }
        else{
            tmp = tmp2;
            *(size_t*)tmp = len;
            *ptr = tmp2+sizeof(size_t);
            vsnprintf(*ptr, len+1, format, args2);
        }
    }
    va_end(args2);
    return len;
}

int cstringSprintf(char** ptr, const char* format, ...){
/* https://stackoverflow.com/questions/37947200/c-variadic-wrapper
 */
    va_list args, args2;
    va_start(args, format);
    va_copy(args2, args);
    int len = vsnprintf(*ptr, cstring_size(*ptr), format, args);
    va_end(args);
    if(len > (int)cstring_size(*ptr)){
        char* tmp = *ptr-sizeof(size_t);
        char* tmp2 = realloc(tmp, sizeof(size_t)+len+1);
        if(tmp == NULL){
            fprintf(stderr, "could not allocate memory for string. Attmempting to continue\n");
        }
        else{
            tmp = tmp2;
            *(size_t*)tmp = len;
            *ptr = tmp2+sizeof(size_t);
            vsnprintf(*ptr, len+1, format, args2);
        }
    }
    va_end(args2);
    return len;
}

char* cstringStrcat(char** dest, char* src){
    char* destCpy = strdup(*dest);
    cstringSprintf(dest, "%s%s", destCpy, src);
    free(destCpy);
    return *dest;
}
