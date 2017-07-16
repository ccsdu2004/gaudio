#ifndef G_MAIN_H
#define G_MAIN_H
#include <math.h>
#include "gaudio.h"
#include "gdecoder.h"
//! decoder -> demuxer -> recast -> resample -> effect -> mixer -> effect -> play
#define GAUDIO_ADUIO_VERSION 2110

#define AUDIO_MAX_SOURCE   8
#define AUDIO_MAX_DECODER 32
#define AUDIO_MAX_EFFECT   8

#define CHANNEL_FL  0
#define CHANNEL_FR  (CHANNEL_FL+1)
#define CHANNEL_FC  (CHANNEL_FR+1)
#define CHANNEL_LFE (CHANNEL_FC+1)
#define CHANNEL_BL  (CHANNEL_LFE+1)
#define CHANNEL_BR  (CHANNEL_BL+1)
#define CHANNEL_BC  (CHANNEL_BR+1)
#define CHANNEL_SL  (CHANNEL_BC+1)
#define CHANNEL_SR  (CHANNEL_SL+1)
#define CHANNEL_MAX (CHANNEL_SR+1)

#define OUTPUTCHANNELS 4

#define AUDIO_PAN_LOW      -1.0f
#define AUDIO_PAN_HIGH      1.0f
#define AUDIO_PAN_DEFAULT    .0f

#define AUDIO_SOUND_SPEED_DEFAULT 340.0f

#define BUFFER_SIZE 45200
//8920
//(4096*2)
#define BUFFER_COUNT 4

#define AUDIO_CHANNEL_MAX 8
#define AUDIO_CHANNEL_CUR 2

#define AUDIO_MIN_DIFF (0.01f)
#define GAIN_SILENCE_THRESHOLD 0.0001f

#ifndef FLT_MAX
    #define FLT_MAX    3.402823466e+38F
#endif

template<class T>
struct gbuffer
{
    gbuffer():buffer(NULL),length(0),use(0)
    {
    }
    T* buffer;
    uint32_t length;
    uint32_t use;
};

template<class T,uint8_t C = AUDIO_CHANNEL_CUR>
struct gdatabuffer
{
    T* data[C];
    uint32_t use;
    uint32_t length;
    gdatabuffer(){use = 0;}
};

void gerror_set(int32_t);

uint32_t gloop(void* ptr);
void gdecprocess();

#define gFreeData(data)\
    if(data != NULL)\
        delete data;\
    data = NULL;

#define gFreeBuffer(buffer)\
    if(buffer != NULL)\
        delete []buffer;\
    buffer = NULL;


//void gcopy(gfloat* dst,const gvoid* src,guint step,gint format,guint count);
int32_t get_suitable_buffer_size(int32_t channel,int32_t samplerate);

gdecoder* gdecoder_create_raw(uint32_t channel,uint32_t format,uint32_t samplerate);
int is_little_endian_cpu();

#define AUDIO_STANDARD_SAMPLERATE      (44100.0f)
#define AUDIO_STANDARD_SAMPLERATE_LOW  (8000.0f)
#define AUDIO_STANDARD_SAMPLERATE_HIGH (48000.0f)

#define MAX_INT24 8388607

#define STD_BUFFER_SIZE 4096

inline bool gisequal(float v,float k,float t = 0.002f)
{
    return (v<k+t) && (v>k-t);
}

template<class T>
inline float gtofloat(T val)
{
    return val;
}

template<>
inline float gtofloat(int8_t val)
{
    return val*(1.0f/127.0f);
}

template<>
inline float gtofloat(uint8_t val)
{
    return val*(1.0f/256.0f);
}

template<>
inline float gtofloat(int16_t val)
{
    return val*(1.0f/32767.0f);
}

template<>
inline float gtofloat(uint16_t val)
{
    return val*(1.0f/65535.0f);
}

template<>
inline float gtofloat(int32_t val)
{
    return val*(1.0f/2147483647.0f);
}

struct gint24
{
    char buffer[3];
};

inline float gint24_to_float(const gint24& value)
{
    int32_t val = value.buffer[0] + (value.buffer[1] << 8) + (value.buffer[2] << 16);
    return val*1.0f/MAX_INT24;
}

template<>
inline float gtofloat(gint24 val)
{
    return gint24_to_float(val);
}

template<class T,class M>
inline int32_t gmagic_check(T* l,M* m,int n)
{
    for(int i = 0;i<n;i++)
        if(l[i] != m[i])
            return 0;
    return 1;
}

union castbuffer
{
    uint8_t* uchar;
    int16_t* ptr16;
    int32_t* ptr32;
};

inline int32_t gvalid_check(/*int32_t format,*/int32_t channel,int32_t samplerate)
{
    //if(format != 4 && format != 8 && format != 16 && format != 24 && format != 32)
    //    return FALSE;
    if(samplerate < AUDIO_STANDARD_SAMPLERATE_LOW || samplerate > AUDIO_STANDARD_SAMPLERATE_HIGH)
        return FALSE;
    if(channel != 1 && channel != 2)
        return FALSE;
    return TRUE;
}

int32_t check_valid_format(int32_t format,int32_t channel,int32_t samplerate);
bool panprocess();

#endif
















