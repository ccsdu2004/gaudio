#ifndef LIB_AAC_HPP
#define LIB_AAC_HPP
#include <stdio.h>
#include <faac.h>
#include "lib_Encoder.h"

class lib_Aac : public lib_Encoder
{
public:
    lib_Aac();
    ~lib_Aac();
public:
    int  open(const char* snd,int channels,int rate,int bite);
    int  write(unsigned char* buffer,int len);
    void close();
private:
    faacEncHandle            handle;
    faacEncConfigurationPtr  config;
    unsigned long            inputSamples;
    unsigned long            maxOutputBytes;
    unsigned char*           outBuffer;
    int                      channel;
    FILE*                    file;
};

#endif
//! ccsdu2004
