#ifndef LIBAUDIO_H
#define LIBAUDIO_H

//#ifndef EAPI_EXPORT
    // #define EAPI_EXPORT_DLL __declspec(dllimport)
//#else
//     #define EAPI_EXPORT_DLL __declspec(dllexport)
//#endif
#define EAPI_EXPORT_DLL

/*#define AUDIO_ENC_WAV     0
#define AUDIO_ENC_AU      1
#define AUDIO_ENC_AIFF    2
#define AUDIO_ENC_FLAC    3
#define AUDIO_ENC_MP2     4
#define AUDIO_ENC_MP3     5
#define AUDIO_ENC_APE     6
#define AUDIO_ENC_AAC     7
#define AUDIO_ENC_CAF     8
#define AUDIO_ENC_RAW     9
#define AUDIO_ENC_OGG     10
#define AUDIO_ENC_VOC     11
#define AUDIO_ENC_XM      12
#define AUDIO_ENC_S3M     13
#define AUDIO_ENC_MOD     14
#define AUDIO_ENC_IT      15
#define AUDIO_ENC_WV      16
#define AUDIO_ENC_TTA     17*/

struct  lib_Encoder;
typedef lib_Encoder lib_Encoder;

struct  lib_Decoder;
typedef lib_Decoder lib_Decoder;

extern "C"
{
     EAPI_EXPORT_DLL void         lib_Encoder_init(void);
	 EAPI_EXPORT_DLL void         lib_Encoder_deinit(void);
     EAPI_EXPORT_DLL lib_Encoder* lib_Encoder_new(int enc);
	 EAPI_EXPORT_DLL void         lib_Encoder_destroy(lib_Encoder* encoder);
	 EAPI_EXPORT_DLL int          lib_Encoder_start(lib_Encoder* encoder,const char* file,int channels,int rate,int bite);
	 EAPI_EXPORT_DLL int          lib_Encoder_add_pcm(lib_Encoder* encoder,unsigned char* pcm,long len);
	 EAPI_EXPORT_DLL void         lib_Encoder_close(lib_Encoder* encoder);
}

#endif
//! ccsdu2004
