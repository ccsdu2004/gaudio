#ifndef LIB_FLAC_HPP
#define LIB_FLAC_HPP
#include <FLAC/stream_encoder.h>
#include "lib_Encoder.h"

class lib_Flac : public lib_Encoder
{
public:
    lib_Flac();
    ~lib_Flac();
public:
    int  open(const char* snd,int channels,int rate,int bite);
    int  write(unsigned char* buffer,int len);   
    void close();                
private:
    FLAC__bool ok;
	FLAC__StreamEncoder *encoder;
	FLAC__StreamEncoderInitStatus status; 
    FLAC__int32 pcm[2048];             
};

#endif
//! ccsdu2004
