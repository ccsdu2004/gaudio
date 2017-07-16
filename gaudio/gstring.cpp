#include <string.h>
#include "gstring.h"

char* getlastof(const char* str,char k)
{
    char* retval = strchr(const_cast<char*>(str),k);
    char* p = retval;

    while(p != NULL)
    {
        p = strchr(p + 1,k);
        if(p != NULL)
            retval = p;
    }

    if(retval != NULL)
        retval++;
    return(retval);
}

#ifndef WIN32

char* strlwr(char* s)
{
    char* str;
    str = s;
    while(*str != 0)//'/0')
    {
        if(*str > 'A' && *str < 'Z')
        {
            *str += 'a'-'A';
        }
        str++;
    }
    return s;
}

#endif
