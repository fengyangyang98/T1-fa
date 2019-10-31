#include "util.h"

int str_comp(const void * a,const void * b)
{
    return *(char*)a-*(char*)b;
}

bool check_string(char * str)
{
    size_t len = strlen(str);

    qsort(str, len, sizeof(char), str_comp);

    size_t index = 0;
    for(index = 0; index < len - 1; index++)
    {
        if(str[index] == str[index + 1]) return false;
    }
    return true;
}

bool check_string_nfa(char * str)
{
    size_t len = strlen(str);

    qsort(str, len, sizeof(char), str_comp);

    size_t index = 0;
    for(index = 0; index < len - 1; index++)
    {
        if(str[index] == str[index + 1] && str[index] != '^') return false;
    }
    return true;
}

bool char_in_string(char * str, char c)
{
    char* ptr = strchr(str, c);
	int pos = ptr-str;
	if (ptr)
		return true;

    return false;
}

bool char_is_empty(char * str)
{
    if(strlen(str) == 0)
        return true;
    return false;
}