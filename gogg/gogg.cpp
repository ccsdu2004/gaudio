#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ogg/ogg.h>
#include <vorbis/vorbisfile.h>
#include "../gaudio/gaddons.h"
#include "../gaudio/gdecreator.h"
#include "../gaudio/streamBuffer.h"
#include "../gaudio/gmain.h"

typedef int32_t(*stream_read_ptr)(streamBuffer* stream,void* ptr,int32_t size,int32_t n);
typedef int32_t(*stream_tell_ptr)(streamBuffer* stream);
typedef int32_t(*stream_seek_ptr)(streamBuffer* stream,int32_t offset,int32_t flag);

static stream_tell_ptr stream_tell_ = NULL;
static stream_read_ptr stream_read_ = NULL;
static stream_seek_ptr stream_seek_ = NULL;

size_t gread_func(void* ptr,size_t size,size_t n,void* datasource)
{
    return stream_read_((streamBuffer*)datasource,ptr,size,n);
}

int gseek_func(void* datasource,ogg_int64_t offset,int whence)
{
    return stream_seek_((streamBuffer*)datasource,offset,whence);
}

int gclose_func(void* datasource)
{
    return 0;
}

long gtell_func(void* datasource)
{
    return stream_tell_((streamBuffer*)datasource);
}

gdecoder* gdecoder_create();

gxcreator(ogg)

int32_t goggdecreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_OGG)
       return TRUE;
    return FALSE;
}

gdecoder* goggdecreator::create_by_extension(const char* extension)
{
    if(!strncmp(extension,"ogg",3))
        return gdecoder_create();
    return NULL;
}

gdecoder* goggdecreator::create_by_decid(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_OGG)
       return gdecoder_create();
    return NULL;
}

gdecoder* goggdecreator::create_by_buffer(int8_t* buffer,uint32_t length)
{
    if(buffer[0] == 'O' && buffer[1] == 'g' && buffer[2] == 'g' && buffer[3] == 'S')
        return gdecoder_create();
    return NULL;
}

void goggdecreator::destroy()
{
    delete this;
}

const char* decoder_maker(gdecoder*)
{
    return "duwenhua2013";
}

struct gdecoderimpl
{
    OggVorbis_File  vorbis;
    vorbis_info*    info;
    vorbis_comment* comment;
    ov_callbacks    callbacks;
    gdecoderimpl():
    info(0),
    comment(0)
    {
        info = 0;
        comment = 0;
        callbacks.read_func = gread_func;
        callbacks.seek_func = gseek_func;
        callbacks.close_func = gclose_func;
        callbacks.tell_func = gtell_func;
    }
};

static void decoder_destroy(gdecoder* decoder)
{
    ov_clear(&decoder->impl->vorbis);
    delete decoder->impl;
    decoder->impl = 0;
    delete decoder;
}

static uint32_t decoder_get_long(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
        return decoder->impl->info->channels;
    else if(flag == AUDIO_ENUM_SAMPLERATE)
        return decoder->impl->info->rate;
    else if(flag == AUDIO_ENUM_FORMAT)
    {
        return AUDIO_FORMAT_TYPE_VORBIS;
    }
    else if(flag == AUDIO_ENUM_DURATION)
        return 1000*ov_time_total(&decoder->impl->vorbis,-1);
    else if(flag == AUDIO_ENUM_POSITION)
    {
        return 1000*ov_time_tell(&decoder->impl->vorbis);
    }
    else if(flag == AUDIO_ENUM_BITERATE)
    {
        return ov_bitrate(&decoder->impl->vorbis,-1);
    }
    return FALSE;
}

static int32_t decoder_init(gdecoder* decoder)
{
    int32_t ret = ov_open_callbacks(decoder->stream,&decoder->impl->vorbis,0,0,decoder->impl->callbacks);
    if(ret == 0)
        decoder->impl->info = ov_info(&decoder->impl->vorbis,-1);
    return ret == 0;
}

static int32_t decoder_seek(gdecoder* decoder,uint32_t position)
{
    return ov_pcm_seek(&decoder->impl->vorbis,position) == 0;
}

static int32_t decoder_meta(gdecoder* decoder,int32_t flag,char* buffer)
{
    if(!decoder)
        return FALSE;
    char** ptr = ov_comment(&decoder->impl->vorbis,-1)->user_comments;
    while(*ptr)
    {
        char* str = *ptr;
        if(flag == AUDIO_ENUM_TITLE && !strncmp(str,"TITLE=",6))
        {
            strcpy(buffer,str+6);
            return TRUE;
        }
        else if(flag == AUDIO_ENUM_ALBUM && !strncmp(str,"ALBUM=",6))
        {
            strcpy(buffer,*ptr+6);
            return TRUE;
        }
        else if(flag == AUDIO_ENUM_ARTIST && !strncmp(str,"ARTIST=",7))
        {
            strcpy(buffer,*ptr+7);
            return TRUE;
        }
        else if(flag == AUDIO_ENUM_PUBLISHDATE && !strncmp(str,"DATE=",5))
        {
            strcpy(buffer,*ptr+5);
            return TRUE;
        }
        else  if(flag == AUDIO_ENUM_COMMENT && !strncmp(str,"COMMENT=",8))
        {
            strcpy(buffer,*ptr+8);
            return TRUE;
        }
        else if(flag == AUDIO_ENUM_GENRE && !strncmp(str,"GENRE=",6))
        {
            strcpy(buffer,*ptr+6);
            return TRUE;
        }
        ++ptr;
    }
    return FALSE;
}

static uint32_t decoder_read(gdecoder* decoder,float* pcm,int32_t length)
{
    if(length <= 1)
        return 0;
    int lendian = 0;

    int32_t read = 0;
    int  section;
    int  result;
    int32_t len2 = length*2;
    int32_t size = 0;
    char* buffer = new char[len2];

    while(size<len2)
    {
        result = ov_read(&decoder->impl->vorbis,buffer+size,len2-size,lendian,2,1,&section);
        if(result>0)
            size += result;
        else
            break;
    }

    len2 = size/2;
    int32_t j=0;
    int16_t v=0;
    for(int32_t i=0;i<len2;i++)
    {
        v = buffer[j++];
        v += (buffer[j++]<<8);
        pcm[i] = gtofloat<int16_t>(v);
    }

    delete []buffer;
    return size/2;
}

gdecoder* gdecoder_create()
{
    gdecoder* decoder = new gdecoder();

    decoder->meta = decoder_meta;
    decoder->destroy = decoder_destroy;
    decoder->init = decoder_init;
    decoder->seek = decoder_seek;
    decoder->read = decoder_read;
    decoder->resample[0] = decoder->resample[1] = 0;
    decoder->decid = AUDIO_FORMAT_OGG;
    decoder->version = 1001;
    decoder->maker = decoder_maker;

    decoder->stream = 0;
    decoder->ratio = 1.0f;
    decoder->impl = new gdecoderimpl();

    decoder->getint = decoder_get_long;
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
    addon.addon = new goggdecreator();
    return &addon;
}






