#ifndef LIB_SNDFILE_H
#define LIB_SNDFILE_H
#include <sndfile.h>
#include "lib_Encoder.h"

inline int getSndfileType(int id)
{
    int type = -1;
    switch(id)
    {
    case AUDIO_FORMAT_WAV:
        type = SF_FORMAT_WAV;
        break;
    case AUDIO_FORMAT_AU:
        type = SF_FORMAT_AU;
        break;
    case AUDIO_FORMAT_AIFF:
        type = SF_FORMAT_AIFF;
        break;
    case AUDIO_FORMAT_RAW:
        type = SF_FORMAT_RAW;
        break;
    case AUDIO_FORMAT_PAF:
        type = SF_FORMAT_PAF;
        break;
    case AUDIO_FORMAT_SVX:
        type = SF_FORMAT_SVX;
        break;
    case AUDIO_FORMAT_SF:
        type = SF_FORMAT_IRCAM;
        break;
    case AUDIO_FORMAT_VOC:
        type = SF_FORMAT_VOC;
        break;
    case AUDIO_FORMAT_W64:
        type = SF_FORMAT_W64;
        break;
    case AUDIO_FORMAT_PVF:
        type = SF_FORMAT_PVF;
        break;
    case AUDIO_FORMAT_MAT4:
        type = SF_FORMAT_MAT4;
        break;
    case AUDIO_FORMAT_MAT5:
        type = SF_FORMAT_MAT5;
        break;
    case AUDIO_FORMAT_CAF:
        type = SF_FORMAT_CAF;
        break;
    case AUDIO_FORMAT_SD2:
        type = SF_FORMAT_SD2;
        break;
    case AUDIO_FORMAT_FLAC:
        type = SF_FORMAT_FLAC;
        break;
    case AUDIO_FORMAT_OGG:
        type = SF_FORMAT_OGG;
        break;
    case AUDIO_FORMAT_HTK:
        type = SF_FORMAT_HTK;
        break;
    }
    return type;
}

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
