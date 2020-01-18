//misc.h
#ifndef MISC_H
#define MISC_H

/* REQUIRES: strlen(message)<40
 * MODIFIES: none
 * EFFECTS: exits program with code exit_status
 *
 * NOTES: prints to stderr w/ assert-like format.
 * message must be as short as possible. strcat'("Fatal: ",message,__FILE__,__LINE__) is stored in a 80 byte array.
 */
void crash(const char* message, int exit_status);
#endif /* MISC_H */
