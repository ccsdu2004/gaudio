#include "gaudio.h"
#include "internal.h"
#include "encode/lib_Encoder.h"
#include "encode/lib_Sndfile.h"
#include "libaudio.h"

extern "C"
{

EAPI_EXPORT_DLL void         lib_Encoder_init(void)
{
}

EAPI_EXPORT_DLL void         lib_Encoder_deinit(void)
{
}

EAPI_EXPORT_DLL lib_Encoder* lib_Encoder_new(int enc)
{
    if(enc == AUDIO_FORMAT_AIFF ||
       enc == AUDIO_FORMAT_AU   ||
       enc == AUDIO_FORMAT_WAV  ||
       enc == AUDIO_FORMAT_OGG  ||
       enc == AUDIO_FORMAT_VOC  ||
       enc == AUDIO_FORMAT_FLAC ||
       enc == AUDIO_FORMAT_CAF)
        return new lib_Sndfile(enc);
    return NULL;
}

EAPI_EXPORT_DLL void         lib_Encoder_destroy(lib_Encoder* encoder)
{
    if(encoder != NULL)
    {
        delete encoder;
        encoder = NULL;
    }
}

EAPI_EXPORT_DLL int          lib_Encoder_start(lib_Encoder* encoder,const char* file,int channels,int rate,int bite)
{
    if(encoder)
        return encoder->open(file,channels,rate,bite);
    return 0;
}

EAPI_EXPORT_DLL int          lib_Encoder_add_pcm(lib_Encoder* encoder,unsigned char* buffer,long len)
{
    if(encoder)
        return encoder->write(buffer,len);
    return 0;
}

EAPI_EXPORT_DLL void         lib_Encoder_close(lib_Encoder* encoder)
{
    if(encoder)
        return encoder->close();
}

}
