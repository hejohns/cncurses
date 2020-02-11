//cstring.h
#ifndef CSTRING_H
#define CSTRING_H

#include <string.h>

char* cstringInit(char** ptr, size_t size);
void cstringFree(char** ptr);
int cstringSprintf(char* str, const char* format, ...);

#endif /* CSTRING_H */
