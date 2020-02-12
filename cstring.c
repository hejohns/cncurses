//cstring.c
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>


#define SIZE_ToverCHAR ((sizeof(size_t)/sizeof(char)) + (sizeof(size_t)%sizeof(char)))

static size_t cstring_size(char* ptr){
    size_t* tmp = (size_t*)(ptr-SIZE_ToverCHAR);
    return *tmp;
}

char* cstringInit(char** ptr, size_t size){
    *ptr = calloc(SIZE_ToverCHAR+size+1, sizeof(char));
    size_t* tmp = (size_t*)*ptr;
    *tmp = size;
    return *ptr+SIZE_ToverCHAR;
}

void cstringFree(char** ptr){
    free(*ptr-SIZE_ToverCHAR);
    *ptr = NULL;
}

int cstringSprintf(char** ptr, const char* format, ...){
/* https://stackoverflow.com/questions/37947200/c-variadic-wrapper
 */
    va_list args;
    va_start(args, format);
    int len = vsnprintf(*ptr, cstring_size(*ptr), format, args);
    if(len > cstring_size(*ptr)){
        char* tmp = *ptr-SIZE_ToverCHAR;
        char* tmp2 = realloc(tmp, (SIZE_ToverCHAR+len+1)*sizeof(char));
        if(tmp == NULL){
            fprinf(stderr, "could not allocate memory for string. Attmempting to continue\n");
        }
        else{
            tmp = tmp2;
            size_t* tmp3 = tmp;
            *tmp3 = len+SIZE_ToverCHAR+len+1;
            *ptr = tmp2+SIZE_ToverCHAR;
        }
    }
    return len;
}
