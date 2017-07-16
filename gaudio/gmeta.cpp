#include <string.h>
#include "gmeta.h"

void gmeta_init(gmeta_t* meta)
{
    memset(meta->id3v1,0,sizeof(char)*128);
    meta->id3v1length = 0;
    meta->id3v2 = 0;
    meta->id3v2length = 0;
}

void gmeta_deinit(gmeta_t* meta)
{
}

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

int id3_header_size_get(streamBuffer* stream)
{
    ID3Header id3;
    unsigned long tell = stream_tell(stream);
    stream_read(stream,&id3,sizeof(ID3Header),1);
    if(strncmp(id3.header,"ID3",3))
    {
       stream_seek(stream,tell,SEEK_SET);
       return 0;
    }
    unsigned long length = id3.size[0]*0x200000+id3.size[1]*0x4000+id3.size[2]*0x80+id3.size[3];
    return length;
}
