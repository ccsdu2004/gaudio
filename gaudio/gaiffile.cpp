#include <stdio.h>
#include <stdlib.h>
#include "gaddons.h"
#include "gdecreator.h"
#include "streamBuffer.h"
#include "gmain.h"

typedef int32_t(*stream_read_ptr)(streamBuffer* stream,void* ptr,int32_t size,int32_t n);
typedef int32_t(*stream_tell_ptr)(streamBuffer* stream);
typedef int32_t(*stream_seek_ptr)(streamBuffer* stream,int32_t offset,int32_t flag);

static stream_tell_ptr stream_tell_ = NULL;
static stream_read_ptr stream_read_ = NULL;
static stream_seek_ptr stream_seek_ = NULL;

inline uint32_t get_big_endian_int32(uint8_t* value)
{
    return value[0] << 24 | value[1] << 16 | value[2] << 8 | value[3];
}

inline int16_t get_big_endian_int16(uint8_t* value)
{
    return value[0] << 8 | value[1];
}

gdecoder* aiff_decoder_create();

gxcreator(aif)

int32_t gaifdecreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_AIFF)
       return TRUE;
    return FALSE;
}

gdecoder* gaifdecreator::create_by_extension(const char* extension)
{
    if(!strncmp(extension,"aif",3) || !strncmp(extension,"aiff",4))
        return aiff_decoder_create();
    return NULL;
}

gdecoder* gaifdecreator::create_by_decid(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_AIFF)
        return aiff_decoder_create();
    return NULL;
}

gdecoder* gaifdecreator::create_by_buffer(int8_t* buffer,uint32_t length)
{
    if(buffer[0] == 'F' && buffer[1] == 'O' && buffer[2] == 'R' && buffer[3] == 'M')
        return aiff_decoder_create();
    return NULL;
}

void gaifdecreator::destroy()
{
    delete this;
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
    uint32_t blockalign;
    uint32_t startsize;
    gdecoderimpl():
    format(2),
    channel(2),
    samplerate(44100),
    duration(0),
    current(0),
    offset(0),
    blockalign(0),
    startsize(0)
    {
    }
};

void GAPIENTRY aiff_decoder_destroy(gdecoder* decoder)
{
    delete decoder->impl;
    delete decoder;
}

uint32_t GAPIENTRY aiff_decoder_read(gdecoder* decoder,float* pcm,int32_t length)
{
    uint32_t size = 0;
    uint8_t buffer[BUFFER_SIZE*8];
    size = stream_read_((streamBuffer*)decoder->stream,buffer,sizeof(uint8_t),length*decoder->impl->format/8);
    decoder->impl->current = stream_tell_((streamBuffer*)decoder->stream);
    uint32_t j = 0;
    if(decoder->impl->format == 16)
    {
        for(uint32_t i = 0;i < size;i+=2)
            pcm[j++] = gtofloat(get_big_endian_int16(buffer+i));
    }
    else if(decoder->impl->format == 32)
    {
        for(uint32_t i = 0;i < size;i+=4)
            pcm[j++] = gtofloat(get_big_endian_int32(buffer+i));
    }
    return size/decoder->impl->format*8;
}

uint32_t read_be80extended(void* stream)
{
    uint8_t buffer[10];
    if(!stream_read_((streamBuffer*)stream,buffer,sizeof(int8_t),10))
        return 0;
    uint32_t mantissa, last = 0;
    uint8_t exp = buffer[1];
    exp = 30 - exp;
    mantissa = (buffer[2]<<24) | (buffer[3]<<16) | (buffer[4]<<8) | buffer[5];
    while(exp--)
    {
        last = mantissa;
        mantissa >>= 1;
    }
    if((last&1))
        mantissa++;
    return mantissa;
}

#define CALC_POSITION(value)\
    value /= decoder->impl->channel;\
    value *= 1000;\
    value /= decoder->impl->samplerate;\
    value *= 8;\
    value /= decoder->impl->format;

int32_t GAPIENTRY aiff_decoder_init(gdecoder* decoder)
{
    //stream_seek_((streamBuffer*)decoder->stream,0,SEEK_END);
    //uint64_t filesize = stream_tell_((streamBuffer*)decoder->stream);
    //stream_seek_((streamBuffer*)decoder->stream,decoder->metainfo.id3v2length,SEEK_SET);
    stream_seek_((streamBuffer*)decoder->stream,0,SEEK_SET);
    decoder->impl->offset = 0;

    char buffer[25];
    int length = 0;

    if(!stream_read_((streamBuffer*)decoder->stream,buffer,sizeof(char),12) ||
        memcmp(buffer,"FORM",4) != 0 || memcmp(buffer+8,"AIFF",4) != 0)
        return FALSE;

    while(!decoder->impl->startsize || decoder->impl->format == 0)
    {
        char tag[4];
        if(!stream_read_((streamBuffer*)decoder->stream,tag,sizeof(char),4))
            break;

        uint8_t v[4];
        if(!stream_read_((streamBuffer*)decoder->stream,v,sizeof(char),4))
            break;

        length = get_big_endian_int32(v);

        if(memcmp(tag,"COMM",4) == 0 && length >= 18)
        {
            stream_read_((streamBuffer*)decoder->stream,v,sizeof(char),2);
            decoder->impl->channel = get_big_endian_int16(v);
            stream_read_((streamBuffer*)decoder->stream,v,sizeof(char),4);
            stream_read_((streamBuffer*)decoder->stream,v,sizeof(char),2);
            decoder->impl->format = get_big_endian_int16(v);
            decoder->impl->samplerate = read_be80extended(decoder->stream);
            decoder->impl->blockalign = decoder->impl->channel*decoder->impl->format;
            length -= 18;
        }
        else if(memcmp(tag,"SSND",4) == 0)
        {
            decoder->impl->startsize = stream_tell_((streamBuffer*)decoder->stream);
            decoder->impl->startsize += 8;
            decoder->impl->length = length - 8;
        }

        stream_seek_((streamBuffer*)decoder->stream,length,SEEK_CUR);
    }

    if(decoder->impl->startsize > 0 && decoder->impl->format != 0)
        stream_seek_((streamBuffer*)decoder->stream,decoder->impl->startsize,SEEK_SET);

    uint64_t value = length;
    CALC_POSITION(value)
    decoder->impl->duration = value;
    return TRUE;
}

int32_t GAPIENTRY aiff_decoder_seek(gdecoder* decoder,uint32_t position)
{
    uint64_t length = position*decoder->impl->format/8*decoder->impl->samplerate/1000*decoder->impl->channel;
    stream_seek_((streamBuffer*)decoder->stream,decoder->impl->offset+length,SEEK_SET);
    return TRUE;
}

uint32_t GAPIENTRY aiff_decoder_get_long(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
        return decoder->impl->channel;
    else if(flag == AUDIO_ENUM_SAMPLERATE)
        return decoder->impl->samplerate;
    else if(flag == AUDIO_ENUM_FORMAT)
        return decoder->impl->format;
    else if(flag == AUDIO_ENUM_DURATION)
        return decoder->impl->duration;
    else if(flag == AUDIO_ENUM_POSITION)
    {
        uint64_t current = decoder->impl->current - decoder->impl->offset;
        CALC_POSITION(current)
        return current;
    }
    else if(flag == AUDIO_ENUM_BITERATE)
        return decoder->impl->channel*decoder->impl->samplerate*decoder->impl->format;
    return FALSE;
}

gdecoder* aiff_decoder_create()
{
    gdecoder* decoder = new gdecoder();
    decoder->meta = 0;
    decoder->destroy = aiff_decoder_destroy;
    decoder->init = aiff_decoder_init;
    decoder->seek = aiff_decoder_seek;
    decoder->read = aiff_decoder_read;
    decoder->resample[0] = decoder->resample[1] = 0;
    decoder->decid = AUDIO_FORMAT_AIFF;
    decoder->version = 1010;
    decoder->maker = NULL;

    decoder->stream = 0;
    decoder->ratio = 1.0f;

    decoder->impl = new gdecoderimpl();
    decoder->getint = aiff_decoder_get_long;
    return decoder;
}

static gaddons addon;

gaddons* GAPIENTRY gaiff_instance(void* ctx)
{
    stream_tell_ = (stream_tell_ptr)gaudio_address_get("stream_tell");
    stream_seek_ = (stream_seek_ptr)gaudio_address_get("stream_seek");
    stream_read_ = (stream_read_ptr)gaudio_address_get("stream_read");

    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_AUDIO;
    addon.addon = new gaifdecreator();
    return &addon;
}






