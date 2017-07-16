#ifndef LIB_SNDFILE_H
#define LIB_SNDFILE_H
#include <sndfile.h>
#include "lib_Encoder.h"

class lib_Sndfile : public lib_Encoder
{
public:
    lib_Sndfile(int format);
    ~lib_Sndfile();
public:
    int  open(const char* name,int channels,int rate,int bite);
    int  write(unsigned char* buffer,int len);
    void close();
private:
    SF_INFO  info;
    SNDFILE *file;
};

#endif
//! ccsdu2004
