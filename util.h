#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int str_comp(const void * a,const void * b);
bool check_string(char * str);
bool char_in_string(char * str, char c);
bool char_is_empty(char * str);
bool check_string_nfa(char * str);

#endif