/*
This file is part of CityUID App.
A Flipper Zero application to analyse student ID cards from the University of Duisburg-Essen (Intercard)

Based on UDECard App by Alexander Hahn (https://github.com/hahnworks/UDECard)
Copyright (C) 2025 Jason Lam (github.com/jasonlam510)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "util.h"

#include <furi.h>

void reverse_string(char* str) {
    int len = strlen(str);

    for(int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

void remove_leading_zeros_from_string(char* str) {
    int len = strlen(str);
    int i = 0;

    while(i < len && str[i] == '0') {
        i++;
    }

    if(i == len) {
        str[0] = '0';
        str[1] = '\0';
    } else {
        memmove(str, str + i, len - i + 1);
    }
}

void remove_trailing_spaces_from_string(char* str) {
    int len = strlen(str);
    
    // Find the last non-space character
    while(len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\0')) {
        len--;
    }
    
    // Null terminate at the last non-space character
    str[len] = '\0';
}
