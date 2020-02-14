//cstring.h
#ifndef CSTRING_H
#define CSTRING_H


/* BE CAREFUL
 * cstring* expects address of char* associated with cstring
 * to modify char* if necessary
 *
 * NEVER pass char* directly into a cstring* unless it is 
 * not being modified 
 */

/* REQUIRES:
 * MODIFIES:
 * EFFECTS:
 */
char* cstringInit(char** ptr, size_t size);

/* REQUIRES:
 * MODIFIES:
 * EFFECTS:
 */
void cstringFree(char** ptr);

/* REQUIRES:
 * MODIFIES:
 * EFFECTS:
 */
int cstringVsprintf(char** ptr, const char* format, va_list ap);

/* REQUIRES:
 * MODIFIES:
 * EFFECTS:
 */
int cstringSprintf(char** str, const char* format, ...);

/* REQUIRES:
 * MODIFIES:
 * EFFECTS:
 */
char* cstringStrcat(char** dest, char* src);

#endif /* CSTRING_H */
