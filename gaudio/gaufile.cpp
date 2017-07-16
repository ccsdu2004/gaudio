#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gaddons.h"
#include "gdecreator.h"
#include "streamBuffer.h"

#define AU_VERSION 101
#define AU_HEADER  24

gdecoder* au_gdecoder_create();

enum encoding
{
    AU_ULAW_8 = 1,
    AU_PCM_8 = 2,
    AU_PCM_16 = 3,
    AU_PCM_24 = 4,
    AU_PCM_32 = 5,
    AU_FLOAT_32 = 6,
    AU_FLOAT_64 = 7,
    AU_ALAW_8 = 27
};

typedef int32_t(*stream_read_ptr)(streamBuffer* stream,void* ptr,int32_t size,int32_t n);
typedef int32_t(*stream_tell_ptr)(streamBuffer* stream);
typedef int32_t(*stream_seek_ptr)(streamBuffer* stream,int32_t offset,int32_t flag);

static stream_tell_ptr stream_tell_ = NULL;
static stream_read_ptr stream_read_ = NULL;
static stream_seek_ptr stream_seek_ = NULL;

inline uint32_t get_big_endian_uint32(uint8_t* value)
{
    return value[0] << 24 | value[1] << 16 | value[2] << 8 | value[3];
}

inline int32_t get_format_by_encoding(int32_t encoding)
{
    switch(encoding)
    {
    case AU_ULAW_8:
        return 16;
    case AU_PCM_8:
        return 8;
    case AU_PCM_16:
        return 16;
    case AU_ALAW_8:
        return 16;
    }
    return 8;
}

static int32_t is_little_endian()
{
    union
    {
       long l;
       char c[sizeof(long)];
    }u;
    u.l = 1;
    return (u.c[0] == 1) ? TRUE: FALSE;
}

gxcreator(au)

int32_t gaudecreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_AU)
       return TRUE;
    return FALSE;
}

gdecoder* gaudecreator::create_by_extension(const char* extension)
{
    if(!strncmp(extension,"au",2) || !strncmp(extension,"snd",3))
        return au_gdecoder_create();
    return NULL;
}

gdecoder* gaudecreator::create_by_decid(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_AU)
       return au_gdecoder_create();
    return NULL;
}

gdecoder* gaudecreator::create_by_buffer(int8_t* buffer,uint32_t length)
{
    if(buffer[0] == '.' && buffer[1] == 's' && buffer[2] == 'n' && buffer[3] == 'd')
        return au_gdecoder_create();
    return NULL;
}

void gaudecreator::destroy()
{
    delete this;
}

const char* decoder_maker(gdecoder*)
{
    return "ccsdu2004";
}

struct gdecoderimpl
{
    uint32_t format;
    uint32_t channel;
    uint32_t samplerate;
    uint32_t duration;
    uint32_t current;
    uint32_t length;
    uint32_t offset;
    gdecoderimpl():
    format(AUDIO_FORMAT_TYPE_INT16),
    channel(2),
    samplerate(44100),
    duration(0),
    current(0),
    offset(0)
    {
    }
};

void au_decoder_destroy(gdecoder* decoder)
{
    delete decoder->impl;
    delete decoder;
}

#define COPY(type)\
   type buffer[BUFFER_SIZE];\
   uint32_t size = stream_read_((streamBuffer*)decoder->stream,buffer,sizeof(type),length);\
   for(int32_t i=0;i<size;i++)\
      pcm[i] = gtofloat(buffer[i]);\
   return size;

uint32_t decoder_read_int16l(gdecoder* decoder,float* pcm,int32_t length)
{
    if(length <= 0)
        return 0;

    int16_t* buffer = new int16_t[length];
    uint32_t ret = stream_read_((streamBuffer*)decoder->stream,buffer,sizeof(int16_t),length);

    for(uint32_t i=0;i<ret;i++)
    {
        int16_t x = buffer[i];
        x = ((x << 8) & 0xFF00) | ((x >> 8) & 0x00FF);
        pcm[i] = gtofloat(x);
    }
    delete []buffer;
    decoder->impl->current = stream_tell_((streamBuffer*)decoder->stream);
    return ret;
}

uint32_t decoder_read_int16b(gdecoder* decoder,float* pcm,int32_t length)
{
    COPY(int16_t);
}

uint32_t decoder_read_int8s(gdecoder* decoder,float* pcm,int32_t length)
{
    if(length <= 0)
        return 0;

    int8_t* buffer = new int8_t[length];
    uint32_t ret = stream_read_((streamBuffer*)decoder->stream,buffer,sizeof(int8_t),length);

    for(uint32_t i=0;i<ret;i++)
    {
        int8_t x = buffer[i];// + (int8_t)128;
        pcm[i] = gtofloat(x);
    }
    delete []buffer;
    decoder->impl->current = stream_tell_((streamBuffer*)decoder->stream);
    return ret;
}

static int16_t mulaw2linear(uint8_t mulawbyte)
{
    static const int16_t exp_lut[8] =
    {
        0,132,396,924,1980,4092,8316,16764
    };

    int16_t sign,exponent,mantissa,sample;
    mulawbyte = ~mulawbyte;
    sign = (mulawbyte & 0x80);
    exponent = (mulawbyte >> 4) & 0x07;
    mantissa = mulawbyte & 0x0F;
    sample = exp_lut[exponent] + (mantissa << (exponent + 3));
    if(sign != 0)
       sample = -sample;
    return sample;
}

uint32_t decoder_read_ulaw(gdecoder* decoder,float* pcm,int32_t length)
{
    if(length <= 0)
        return 0;

    uint8_t* buffer = new uint8_t[length];
    uint32_t ret = stream_read_((streamBuffer*)decoder->stream,buffer,sizeof(uint8_t),length);

    for(uint32_t i=0;i<ret;i++)
    {
        int16_t x = mulaw2linear(buffer[i]);
        pcm[i] = gtofloat(x);
    }
    delete []buffer;
    decoder->impl->current = stream_tell_((streamBuffer*)decoder->stream);
    return ret;
}

//! stem from: http://www.multimedia.cx/simpleaudio.html#tth_sEc6.1
#define SIGN_BIT (0x80)
#define QUANT_MASK (0xf)
#define SEG_SHIFT (4)
#define SEG_MASK (0x70)
static int16_t alaw2linear(uint8_t a_val)
{
    int16_t t, seg;
    a_val ^= 0x55;
    t = (a_val & QUANT_MASK) << 4;
    seg = ((int16_t) a_val & SEG_MASK) >> SEG_SHIFT;
    switch(seg)
    {
    case 0:
        t += 8;
        break;
    case 1:
        t += 0x108;
        break;
    default:
        t += 0x108;
        t <<= seg - 1;
    }
    return (a_val & SIGN_BIT) ? t : -t;
}

uint32_t decoder_read_alaw(gdecoder* decoder,float* pcm,int32_t length)
{
    if(length <= 0)
        return 0;

    uint8_t* buffer = new uint8_t[length];
    uint32_t ret = stream_read_((streamBuffer*)decoder->stream,buffer,sizeof(uint8_t),length);

    for(uint32_t i=0;i<ret;i++)
    {
        int16_t x = alaw2linear(buffer[i]);
        pcm[i] = gtofloat(x);
    }
    delete []buffer;
    decoder->impl->current = stream_tell_((streamBuffer*)decoder->stream);
    return ret;
}

#define CALC_POSITION(value)\
    value /= decoder->impl->channel;\
    value *= 1000;\
    value /= decoder->impl->samplerate;\
    value <<= 1;\
    value *= 8;\
    value /= get_format_by_encoding(decoder->impl->format);

static int32_t au_decoder_init(gdecoder* decoder)
{
    stream_seek_((streamBuffer*)decoder->stream,0,SEEK_END);
    uint64_t filesize = stream_tell_((streamBuffer*)decoder->stream);
    //stream_seek_((streamBuffer*)decoder->stream,decoder->metainfo.id3v2length,SEEK_SET);
    stream_seek_((streamBuffer*)decoder->stream,0,SEEK_SET);
    uint8_t val[4] = {0};
    stream_read_((streamBuffer*)decoder->stream,val,sizeof(uint8_t),4);
    uint32_t magic = get_big_endian_uint32(val);
    stream_read_((streamBuffer*)decoder->stream,val,sizeof(uint8_t),4);
    uint32_t offset = get_big_endian_uint32(val);
    stream_read_((streamBuffer*)decoder->stream,val,sizeof(uint8_t),4);
    uint32_t length = get_big_endian_uint32(val);
    stream_read_((streamBuffer*)decoder->stream,val,sizeof(uint8_t),4);
    uint32_t encoding = get_big_endian_uint32(val);
    stream_read_((streamBuffer*)decoder->stream,val,sizeof(uint8_t),4);
    uint32_t samplerate = get_big_endian_uint32(val);
    stream_read_((streamBuffer*)decoder->stream,val,sizeof(uint8_t),4);
    uint32_t channel = get_big_endian_uint32(val);
    if(magic != 0x2e736e64 || offset < 24)
        return FALSE;
    if(0xffffffff == length)
        length = filesize - offset;

    decoder->impl->format = encoding;//get_format_by_encoding(encoding);
    decoder->impl->channel = channel;
    decoder->impl->samplerate = samplerate;
    decoder->impl->current = 0;

    uint64_t value = length;
    CALC_POSITION(value)
    decoder->impl->duration = value;

    if(offset != 24)
        stream_seek_((streamBuffer*)decoder->stream,offset-24,SEEK_CUR);

    switch(encoding)
    {
    case AU_ULAW_8:
        decoder->impl->format = 16;
        decoder->read = decoder_read_ulaw;
        break;
    case AU_PCM_8:
        decoder->impl->format = 8;
        decoder->read = decoder_read_int8s;
        break;
    case AU_PCM_16:
        decoder->impl->format = 16;
        decoder->read = is_little_endian() ? decoder_read_int16l : decoder_read_int16b;
        break;
    case AU_ALAW_8:
        decoder->impl->format = 16;
        decoder->read = decoder_read_alaw;
        break;
    default:
        return FALSE;
    }

    decoder->impl->offset = offset;
    return TRUE;
}

int32_t au_decoder_seek(gdecoder* decoder,uint32_t position)
{
    uint64_t length = position*get_format_by_encoding(decoder->impl->format)/8*decoder->impl->samplerate/1000*decoder->impl->channel;
    //stream_seek_((streamBuffer*)decoder->stream,decoder->impl->offset+length+decoder->metainfo.id3v2length,SEEK_SET);
    stream_seek_((streamBuffer*)decoder->stream,decoder->impl->offset+length,SEEK_SET);
    decoder->impl->current = get_format_by_encoding(decoder->impl->format);
    decoder->impl->current *= decoder->impl->samplerate;
    decoder->impl->current >>= 1;
    decoder->impl->current /= 1000;
    decoder->impl->current *= decoder->impl->channel;
    return TRUE;
}

uint32_t decoder_get_int32(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
        return decoder->impl->channel;
    else if(flag == AUDIO_ENUM_SAMPLERATE)
        return decoder->impl->samplerate;
    else if(flag == AUDIO_ENUM_FORMAT)
    {
        uint32_t format = decoder->impl->format;
        if(format == AU_ULAW_8)
            return AUDIO_FORMAT_TYPE_ULAW;
        else if(format == AU_ALAW_8)
            return AUDIO_FORMAT_TYPE_ALAW;
        else if(format == AU_PCM_8)
            return AUDIO_FORMAT_TYPE_INT8;
        else if(format == AU_PCM_16)
            return AUDIO_FORMAT_TYPE_INT16;
        return AUDIO_FORMAT_TYPE_NONE;
    }
    else if(flag == AUDIO_ENUM_DURATION)
    {
        return decoder->impl->duration;
    }
    else if(flag == AUDIO_ENUM_POSITION)
    {
        uint64_t current = decoder->impl->current - decoder->impl->offset;
        CALC_POSITION(current)
        return current/8;
    }
    else if(flag == AUDIO_ENUM_BITERATE)
        return decoder->impl->channel*decoder->impl->samplerate*decoder->impl->format;
    return FALSE;
}

gdecoder* au_gdecoder_create()
{
    gdecoder* decoder = new gdecoder();
    decoder->meta = 0;
    decoder->destroy = au_decoder_destroy;
    decoder->init = au_decoder_init;
    decoder->seek = au_decoder_seek;
    decoder->read = 0;
    decoder->resample[0] = decoder->resample[1] = 0;
    decoder->decid = AUDIO_FORMAT_AU;
    decoder->version = AU_VERSION;
    decoder->maker = decoder_maker;

    decoder->stream = 0;
    decoder->ratio = 1.0f;

    decoder->impl = new gdecoderimpl();
    decoder->getint = decoder_get_int32;
    return decoder;
}

static gaddons addon;

gaddons* GAPIENTRY gau_instance(void* ctx)
{
    stream_tell_ = (stream_tell_ptr)gaudio_address_get("stream_tell");
    stream_seek_ = (stream_seek_ptr)gaudio_address_get("stream_seek");
    stream_read_ = (stream_read_ptr)gaudio_address_get("stream_read");

    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_AUDIO;
    addon.addon = new gaudecreator();
    return &addon;
}






