//cstring_tests.c
#include "macros.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "cstring.h"

int main(){
    /* cstringSprintf */
    char* str;
    str = cstringInit(&str, 20);
    char test[] = "this is a test";
    char test2[] = "this is going to be a very long test to see how realloc is handled!";
    char test3[] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Maecenas luctus venenatis diam ac commodo. Duis magna erat, faucibus id sagittis eu, mattis vel tortor. Ut a dapibus turpis, ut consequat massa. In efficitur dignissim eros, et malesuada lorem rutrum id. Duis a felis eros. Suspendisse accumsan egestas mauris vitae laoreet. Vestibulum ac pretium urna. Quisque id faucibus mi. Donec in tellus metus. Curabitur vitae velit leo. Fusce non lectus blandit, finibus nisi et, iaculis mauris. Pellentesque odio felis, condimentum eu risus a, eleifend pharetra turpis. Vivamus non lacus vulputate nisi venenatis semper. Suspendisse et urna ultrices, imperdiet est ac, mattis magna. Aliquam tincidunt dolor id dui lobortis, ac suscipit nunc volutpat. Aenean ligula mauris, ultrices eu risus tempus, porta venenatis quam. Fusce volutpat massa sit amet augue efficitur, quis ullamcorper felis cursus. Sed varius vestibulum purus eget cursus. Duis sagittis tempor dolor. Etiam id consequat lectus cras amet.";
    int printed = cstringSprintf(&str, "%s", test);
    if(printed != strlen(test)) panic2("", EXIT_FAILURE);
    if(strcmp(str, test)) panic2("", EXIT_FAILURE);
    int printed2 = cstringSprintf(&str, "%s%s", test, test2);
    if(printed2 != strlen(test)+strlen(test2)) panic2("", EXIT_FAILURE);
    char cmp[200];
    strcpy(cmp, test);
    strcat(cmp, test2);
    if(strcmp(str, cmp)) panic2("", EXIT_FAILURE);
    //need to free or else valgrind thinks we've lost pointer to malloced memory
    cstringFree(&str);
    /* cstringStrcat */
    str = cstringInit(&str, 20);
    cstringSprintf(&str, "%s", test);
    cstringStrcat(&str, test2);
    if(strcmp(str, cmp)) panic2("", EXIT_FAILURE);
    cstringStrcat(&str, test3);
    //need to free or else valgrind thinks we've lost pointer to malloced memory
    cstringFree(&str);
    return 0;
}
