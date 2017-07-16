#ifndef LIB_TWOLAME_H
#define LIB_TWOLAME_H
#include <twolame.h>
#include <stdio.h>
#include "lib_Encoder.h"

class lib_Twolame : public lib_Encoder
{
public:
    lib_Twolame();
    ~lib_Twolame();
public:
    int  open(const char* snd,int channels,int rate,int bite);
    int  write(unsigned char* buffer,int len);
    void close();
private:
    twolame_options *encode;
    unsigned char*   mp2buffer;
    short*           buffer2;
    FILE*            file;
};

#endif
//! ccsdu2004
