#ifndef LIB_LAME_H
#define LIB_LAME_H
#include <lame.h>
#include <stdio.h>
#include "lib_Encoder.h"

#define INBUFSIZE 4096
#define MP3BUFSIZE (int)(2 * INBUFSIZE) + 7200

class lib_Lame : public lib_Encoder
{
public:
    lib_Lame();
    ~lib_Lame();
public:
    int  open(const char* snd,int channels,int rate,int bite);
    int  write(unsigned char* buffer,int len);
    void close();
private:
    lame_global_flags*  lame;
	FILE*               file;
	unsigned char*      buffer;
};

#endif
//! ccsdu2004
