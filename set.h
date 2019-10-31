#ifndef SET_H
#define SET_H

#include "globals.h"
#include <stdbool.h>

typedef struct set
{
    unsigned s[MAXSET];
    unsigned set_number;
} set;

void set_init(set * s)
{
    s->set_number = 0;
}

void set_push(set * s, unsigned ele)
{
    s->s[s->set_number++] = ele;
}

void set_erase(set * s, unsigned i)
{
    if(i == s->set_number - 1) s->set_number--;
    else
    {
        s->s[s->s[i]] = s->s[s->set_number - 1]; 
        s->set_number--;
    }
}

unsigned set_count(set * s, unsigned ele)
{
    unsigned count = 0;
    unsigned index;
    for(index = 0; index < s->set_number; index++)
    {
        if(s->s[index] == ele)
        {
            count++;
        }
    }
    return count;
}
#endif