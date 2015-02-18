/*
 * string_utilities.cpp
 *
 *  Created on: Jan 29, 2015
 *      Author: ratpack
 */

#include "830utilities.h"

void reverse(char s[]) {
    int i, j;
    char c;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* itoa:  convert n to characters in s in binary*/
void to_binary_string(unsigned char n, char s[]) {

    for (int i = 7; i >= 0; i--) {       /* generate digits */
        s[i] = n % 2 + '0';   /* get next digit */
        n /= 2;
    }

    s[8] = '\0';
}


