#include <stdio.h>
#include <stdlib.h>
#include "gaddons.h"
#include "gdecreator.h"
#include "streamBuffer.h"
#include "gmain.h"

struct gdecoderimpl
{
    uint32_t format;
    uint32_t channel;
    uint32_t samplerate;
    uint32_t duration;
    uint32_t current;
    uint32_t length;
    gdecoderimpl():
    format(2),
    channel(2),
    samplerate(44100),
    duration(0),
    current(0)
    {
    }
};

void GAPIENTRY raw_decoder_destroy(gdecoder* decoder)
{
    delete decoder->impl;
    decoder->impl = 0;
    delete decoder;
}

#define CALC_POSITION(value)\
    value /= decoder->impl->channel;\
    value *= 1000;\
    value /= decoder->impl->samplerate;\
    value *= 8;\
    value /= decoder->impl->format;

uint32_t GAPIENTRY raw_decoder_get_int32(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
        return decoder->impl->channel;
    else if(flag == AUDIO_ENUM_SAMPLERATE)
        return decoder->impl->samplerate;
    else if(flag == AUDIO_ENUM_FORMAT)
    {
        uint32_t format = decoder->impl->format;
        if(format == 8)
            return AUDIO_FORMAT_TYPE_UINT8;
        else if(format == 16)
            return AUDIO_FORMAT_TYPE_INT16;
        else if(format == 24)
            return AUDIO_FORMAT_TYPE_INT24;
        else if(format == 32)
            return AUDIO_FORMAT_TYPE_INT32;
        return uint32_t(-1);
    }
    else if(flag == AUDIO_ENUM_DURATION)
        return decoder->impl->duration;
    else if(flag == AUDIO_ENUM_POSITION)
    {
        uint32_t current = decoder->impl->current;
        CALC_POSITION(current)
        return current;
    }
    else if(flag == AUDIO_ENUM_BITERATE)
    {
        return decoder->impl->channel*decoder->impl->samplerate*decoder->impl->format;
    }
    return FALSE;
}

int32_t GAPIENTRY raw_decoder_init(gdecoder* decoder)
{
    stream_seek((streamBuffer*)decoder->stream,0,SEEK_END);
    decoder->impl->length = stream_tell((streamBuffer*)decoder->stream);
    stream_seek((streamBuffer*)decoder->stream,0,SEEK_SET);

    uint64_t value = decoder->impl->length;
    CALC_POSITION(value)
    decoder->impl->duration = value;
    return TRUE;
}

int32_t GAPIENTRY raw_decoder_seek(gdecoder* decoder,uint32_t position)
{
    uint64_t length = position*decoder->impl->format/8*decoder->impl->samplerate/1000*decoder->impl->channel;
    stream_seek((streamBuffer*)decoder->stream,length,SEEK_SET);
    return TRUE;
}

#define COPY(type)\
   type buffer[BUFFER_SIZE];\
   size = stream_read((streamBuffer*)decoder->stream,buffer,sizeof(type),length);\
   for(int32_t i=0;i<size;i++)\
      pcm[i] = gtofloat(buffer[i]);

uint32_t GAPIENTRY raw_decoder_read(gdecoder* decoder,float* pcm,int32_t length)
{
    int32_t size = 0;
    if(decoder->impl->format == 8)
    {
        COPY(uint8_t);
    }
    else if(decoder->impl->format == 16)
    {
        COPY(int16_t);
    }
    else if(decoder->impl->format == 24)
    {
        COPY(gint24);
    }
    else if(decoder->impl->format == 32)
    {
        COPY(int32_t);
    }
    decoder->impl->current = stream_tell((streamBuffer*)decoder->stream);
    if(decoder->impl->format == 4)
        size *= 2;
    return size;
}

gdecoder* gdecoder_create_raw(uint32_t channel,uint32_t format,uint32_t samplerate)
{
    if(channel < 1 || channel > 2)
        return NULL;
    if(samplerate < 8000 || samplerate > 48000)
        return NULL;
    if(format != AUDIO_FORMAT_TYPE_INT8 ||
       format != AUDIO_FORMAT_TYPE_INT16 ||
       format != AUDIO_FORMAT_TYPE_INT24 ||
       format != AUDIO_FORMAT_TYPE_INT32)
        return NULL;

    if(format == AUDIO_FORMAT_TYPE_INT8)
        format = 8;
    else if(format == AUDIO_FORMAT_TYPE_INT16)
        format = 16;
    else if(format == AUDIO_FORMAT_TYPE_INT24)
        format = 24;
    if(format == AUDIO_FORMAT_TYPE_INT32)
        format = 32;
    else
    {
        gerror_set(AUDIO_BAD_VALUE);
        return NULL;
    }

    gdecoder* decoder = new gdecoder();
    decoder->impl = new gdecoderimpl();
    decoder->impl->channel = channel;
    decoder->impl->format = format;
    decoder->impl->samplerate = samplerate;
    decoder->impl->current = 0;

    decoder->meta = 0;
    decoder->destroy = raw_decoder_destroy;
    decoder->init = raw_decoder_init;
    decoder->seek = raw_decoder_seek;
    decoder->read = raw_decoder_read;
    decoder->resample[0] = decoder->resample[1] = 0;
    decoder->decid = AUDIO_FORMAT_RAW;
    decoder->version = 1003;
    decoder->maker = NULL;

    decoder->stream = 0;
    decoder->ratio = 1.0f;

    decoder->getint = raw_decoder_get_int32;
    return decoder;
}




