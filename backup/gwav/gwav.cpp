#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "gaudio.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/gdecreator.h"
#include "../gaudio/streamBuffer.h"
#include "../gaudio/gmain.h"
#include "../gaudio/gmath.h"

#define WAV_VERSION 101
#define WAV_HEADER  44

struct gwaveheader
{
    uint8_t	 riff[4];
    int32_t	 riffcount;
    uint8_t	 wave[4];
    uint8_t	 fmt[4];
    uint32_t fmtcount;
    uint16_t format;
    uint16_t channels;
    uint32_t samplespersec;
    uint32_t bytespersec;
    uint16_t blockalign;
    uint16_t bitspersample;
    uint8_t	 data[4];
    uint32_t size;
};

typedef int32_t(*stream_read_ptr)(streamBuffer* stream,void* ptr,int32_t size,int32_t n);
typedef int32_t(*stream_tell_ptr)(streamBuffer* stream);
typedef int32_t(*stream_seek_ptr)(streamBuffer* stream,int32_t offset,int32_t flag);

static stream_tell_ptr stream_tell_ = NULL;
static stream_read_ptr stream_read_ = NULL;
static stream_seek_ptr stream_seek_ = NULL;

gdecoder* gdecoder_create();

gxcreator(wav)

int32_t gwavdecreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_WAV)
       return TRUE;
    return FALSE;
}

gdecoder* gwavdecreator::create_by_extension(const char* extension)
{
    if(!strncmp(extension,"wav",3))
        return gdecoder_create();
    return NULL;
}

gdecoder* gwavdecreator::create_by_decid(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_WAV)
       return gdecoder_create();
    return NULL;
}

gdecoder* gwavdecreator::create_by_buffer(int8_t* buffer,uint32_t length)
{
    if(buffer[0] == 'R' && buffer[1] == 'I' && buffer[2] == 'F' && buffer[3] == 'F')
        return gdecoder_create();
    return NULL;
}

void gwavdecreator::destroy()
{
    delete this;
}

static const char* GAPIENTRY decoder_maker(gdecoder*)
{
    return "http://www.geek-audio.org/";
}

struct gdecoderimpl
{
    uint32_t format;
    uint32_t channel;
    uint32_t samplerate;
    uint32_t duration;
    uint32_t length;
    gdecoderimpl():
    format(2),
    channel(2),
    samplerate(44100),
    duration(0)
    {
    }
};

static void GAPIENTRY decoder_destroy(gdecoder* decoder)
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

static uint32_t GAPIENTRY decoder_get_int32(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
        return decoder->impl->channel;
    else if(flag == AUDIO_ENUM_SAMPLERATE)
        return decoder->impl->samplerate;
    else if(flag == AUDIO_ENUM_FORMAT)
    {
        uint32_t format = decoder->impl->format;
        if(format == 4)
            return AUDIO_FORMAT_TYPE_INT4;
        else if(format == 8)
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
        uint64_t current = stream_tell_((streamBuffer*)decoder->stream) - WAV_HEADER;
        CALC_POSITION(current)
        return current;
    }
    else if(flag == AUDIO_ENUM_BITERATE)
    {
        return decoder->impl->channel*decoder->impl->samplerate*decoder->impl->format;
    }
    return FALSE;
}

static int32_t GAPIENTRY decoder_init(gdecoder* decoder)
{
    stream_seek_((streamBuffer*)decoder->stream,0,SEEK_END);
    decoder->impl->length = stream_tell_((streamBuffer*)decoder->stream) - WAV_HEADER;
    //stream_seek_((streamBuffer*)decoder->stream,decoder->metainfo.id3v2length,SEEK_SET);
    stream_seek_((streamBuffer*)decoder->stream,0,SEEK_SET);
    
    gwaveheader header;
    memset(&header,0,sizeof(gwaveheader));
    stream_read_((streamBuffer*)decoder->stream,&header,sizeof(gwaveheader),1);
    int ret1 = gmagic_check(header.riff,"RIFF",4);
    int ret2 = gmagic_check(header.wave,"WAVE",4);
    int ret3 = gmagic_check(header.fmt,"fmt",3);
    if(ret1 == 0 || ret2 == 0 || ret3 == 0)
    {
        stream_seek_((streamBuffer*)decoder->stream,0/*decoder->metainfo.id3v2length*/,SEEK_SET);
        return FALSE;
    }

    decoder->impl->format = header.bitspersample;
    decoder->impl->channel = header.channels;
    decoder->impl->samplerate = header.samplespersec;

    uint64_t value = gmin<uint32_t>(decoder->impl->length,header.size);
    CALC_POSITION(value)
    decoder->impl->duration = value;
    return TRUE;
}

static int32_t GAPIENTRY decoder_seek(gdecoder* decoder,uint32_t position)
{
    uint64_t length = position*decoder->impl->format/8*decoder->impl->samplerate/1000*decoder->impl->channel;
    stream_seek_((streamBuffer*)decoder->stream,WAV_HEADER+length/*+decoder->metainfo.id3v2length*/,SEEK_SET);
    return TRUE;
}

#define COPY(type)\
   type buffer[BUFFER_SIZE];\
   size = stream_read_((streamBuffer*)decoder->stream,buffer,sizeof(type),length);\
   for(int32_t i=0;i<size;i++)\
      pcm[i] = gtofloat(buffer[i]);

static uint32_t GAPIENTRY decoder_read(gdecoder* decoder,float* pcm,int32_t length)
{   
    uint32_t size = 0;
    if(decoder->impl->format == 4)
    {
        int8_t buffer[BUFFER_SIZE];
        size = stream_read_((streamBuffer*)decoder->stream,buffer,sizeof(int8_t),length/2);
        uint32_t j=0;
        for(int32_t i=0;i<size;i++)
        {
            pcm[j++] = (float)(((buffer[i] & 0xF0) >> 4)+8)/16.0f;
            pcm[j++] = (float)((buffer[i] & 0x0F+8))/16.0f;
        }
    }
    else if(decoder->impl->format == 8)
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
    if(decoder->impl->format == 4)
        size *= 2;
    return size;
}

gdecoder* gdecoder_create()
{
    gdecoder* decoder = new gdecoder();

    decoder->meta = 0;
    decoder->destroy = decoder_destroy;
    decoder->init = decoder_init;
    decoder->seek = decoder_seek;
    decoder->read = decoder_read;
    decoder->resample[0] = decoder->resample[1] = 0;
    decoder->decid = AUDIO_FORMAT_WAV;
    decoder->version = WAV_VERSION;
    decoder->maker = decoder_maker;

    decoder->stream = 0;
    decoder->ratio = 1.0f;
    decoder->impl = new gdecoderimpl();

    decoder->getint = decoder_get_int32;
    return decoder;
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    stream_tell_ = (stream_tell_ptr)gaudio_address_get("stream_tell");
    stream_seek_ = (stream_seek_ptr)gaudio_address_get("stream_seek");
    stream_read_ = (stream_read_ptr)gaudio_address_get("stream_read");

    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_AUDIO;
    addon.addon = new gwavdecreator();
    return &addon;
}






