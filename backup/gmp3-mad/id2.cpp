#include <string.h>
#include "id2.h"

#define ID2_TITLE   0
#define ID2_ARTIST  1
#define ID2_ALBUM   2
#define ID2_YEAR    3
#define ID2_COMMENT 4
#define ID2_GENRE   5

int id2_meta_check(const char* buffer,int flag,char* out)
{
    if(strncmp(buffer,"TAG",3))
        return 0;
    if(flag == ID2_TITLE)
    {
        strncpy(out,buffer+3,30);    
        return 1;        
    }  
    if(flag == ID2_ARTIST)
    {
        strncpy(out,buffer+33,30);    
        return 1;        
    }  
    if(flag == ID2_ALBUM)
    {
        strncpy(out,buffer+63,30);    
        return 1;        
    }  
    if(flag == ID2_YEAR)
    {
        strncpy(out,buffer+93,4);    
        return 1;        
    }   
    if(flag == ID2_COMMENT)
    {
        strncpy(out,buffer+97,4);    
        return 1;        
    } 
    if(flag == ID2_GENRE)
    {
        strncpy(out,buffer+127,1);    
        return 1;        
    }                    
    return 0;
}
