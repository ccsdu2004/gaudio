#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <mpg123.h>
#include "gaudio.h"
#include "gid3v1tag.h"
#include "genre.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/gdecreator.h"
#include "../gaudio/streamBuffer.h"
#include "../gaudio/gmain.h"

#define MP3_VERSION 1010

typedef char*  (*stream_file_ptr)(streamBuffer* stream);
typedef int32_t(*stream_read_ptr)(streamBuffer* stream,void* ptr,int32_t size,int32_t n);
typedef int32_t(*stream_tell_ptr)(streamBuffer* stream);
typedef int32_t(*stream_seek_ptr)(streamBuffer* stream,int32_t offset,int32_t flag);

static stream_file_ptr stream_file_ = NULL;
static stream_tell_ptr stream_tell_ = NULL;
static stream_read_ptr stream_read_ = NULL;
static stream_seek_ptr stream_seek_ = NULL;

off_t snd_vio_seek(void* user,off_t offset,int whence)
{
    off_t ret = stream_seek_((streamBuffer*)user,offset,whence);
    return ret;
}

ssize_t snd_vio_read(void* user,void* ptr,size_t count)
{
    return stream_read_((streamBuffer*)user,ptr,1,count);
}

off_t snd_vio_tell(void* user)
{
    return stream_tell_((streamBuffer*)user);
}

void snd_vio_close(void* user)
{
}

gdecoder* gdecoder_create();

gxcreator(mp3)

int32_t gmp3decreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_MP3)
       return TRUE;
    return FALSE;
}

gdecoder* gmp3decreator::create_by_extension(const char* extension)
{
    if(!strncmp(extension,"mp3",3))
        return gdecoder_create();
    return NULL;
}

gdecoder* gmp3decreator::create_by_decid(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_MP3)
       return gdecoder_create();
    return NULL;
}

gdecoder* gmp3decreator::create_by_buffer(int8_t* buffer,uint32_t length)
{
    int n = buffer[1] >>8;
    if(buffer[0] == 'I' && buffer[1] == 'D' && buffer[2] == '3')
        return gdecoder_create();
    else if(buffer[0] == 127 && n == 0xF)
        return gdecoder_create();
    return NULL;
}

void gmp3decreator::destroy()
{
    mpg123_exit();
    delete this;
}

static const char* GAPIENTRY decoder_maker(gdecoder*)
{
    return "http://www.geek-audio.org/";
}

struct gdecoderimpl
{
    mpg123_handle* mpg123;
    long   rate;
    int    channel,encoding;
    off_t  current_frame;
    off_t  frames_left;
    double current_seconds;
    double seconds_left;
    id3v1tag id3v1;
    gdecoderimpl():mpg123(0)
    {
        memset(&id3v1,0,sizeof(id3v1tag));
    }
};

static void GAPIENTRY decoder_destroy(gdecoder* decoder)
{
    mpg123_close(decoder->impl->mpg123);
    mpg123_delete(decoder->impl->mpg123);
    delete decoder->impl;
    decoder->impl = 0;
    delete decoder;
}

static uint32_t GAPIENTRY decoder_get_uint32(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
        return decoder->impl->channel;
    else if(flag == AUDIO_ENUM_SAMPLERATE)
        return decoder->impl->rate;
    else if(flag == AUDIO_ENUM_FORMAT)
    {
        return AUDIO_FORMAT_TYPE_INT16;
    }
    else if(flag == AUDIO_ENUM_DURATION)
    {
        double time = decoder->impl->current_seconds + decoder->impl->seconds_left;
        return time*1000;
    }
    else if(flag == AUDIO_ENUM_POSITION)
    {
        return decoder->impl->current_seconds*1000;
    }
    else if(flag == AUDIO_ENUM_BITERATE)
    {
        return 16*decoder_get_uint32(decoder,AUDIO_ENUM_CHANNEL)*decoder_get_uint32(decoder,AUDIO_ENUM_SAMPLERATE);
    }
    return FALSE;
}

static int32_t GAPIENTRY decoder_init(gdecoder* decoder)
{
    stream_seek_((streamBuffer*)decoder->stream,-128,SEEK_END);
    stream_read_((streamBuffer*)decoder->stream,&decoder->impl->id3v1,128,1);
    stream_seek_((streamBuffer*)decoder->stream,0,SEEK_SET);

    int error;
    decoder->impl->mpg123 = mpg123_new(mpg123_decoders()[0],&error);
    int32_t flag = 0;
    const char* filepath = stream_file_((streamBuffer*)decoder->stream);

    int32_t len = filepath == 0 ? 0 : strlen(filepath);
    if(len == 0)
    {
        flag = mpg123_replace_reader_handle(decoder->impl->mpg123,snd_vio_read,snd_vio_seek,snd_vio_close);
        if(flag != MPG123_OK)
            return FALSE;
        flag = mpg123_open_handle(decoder->impl->mpg123,decoder->stream);
    }
    else
    {
        flag = mpg123_open(decoder->impl->mpg123,filepath);
    }

    if(flag != MPG123_OK)
        return FALSE;

    mpg123_scan(decoder->impl->mpg123);

    mpg123_getformat(decoder->impl->mpg123,&decoder->impl->rate,&decoder->impl->channel,&decoder->impl->encoding);
    mpg123_position(decoder->impl->mpg123,0,0,&decoder->impl->current_frame,&decoder->impl->frames_left,&decoder->impl->current_seconds,&decoder->impl->seconds_left);
    return TRUE;
}

static int32_t GAPIENTRY decoder_seek(gdecoder* decoder,uint32_t position)
{
    float pos = position/1000.0f;
    int32_t ret = mpg123_seek(decoder->impl->mpg123,pos*decoder->impl->rate,SEEK_SET);
    return TRUE;
}

union castbuffer
{
    unsigned char* uchar;
    short* sptr;
};

static uint32_t GAPIENTRY decoder_read(gdecoder* decoder,float* pcm,int32_t length)
{
    uint32_t read;
    unsigned char* buffer = new unsigned char[2*length];
    mpg123_read(decoder->impl->mpg123,(unsigned char*)buffer,2*length,&read);
    read >>= 1;
    castbuffer cast;
    cast.uchar = buffer;
    for(int32_t i=0;i<read;i++)
        pcm[i] = gtofloat(cast.sptr[i]);
    delete []buffer;
    mpg123_position(decoder->impl->mpg123,0,0,&decoder->impl->current_frame,&decoder->impl->frames_left,&decoder->impl->current_seconds,&decoder->impl->seconds_left);
    return read;
}

static int32_t GAPIENTRY decoder_meta(gdecoder* decoder,int32_t flag,char* buffer)
{
    if(strncmp(decoder->impl->id3v1.tag,"TAG",3))
        return FALSE;

    id3v1tag* tag = &decoder->impl->id3v1;

    switch(flag)
    {
    case AUDIO_ENUM_TITLE:
        strcpy(buffer,tag->title);
        break;
    case AUDIO_ENUM_ALBUM:
        strcpy(buffer,tag->album);
        break;
    case AUDIO_ENUM_ARTIST:
        strcpy(buffer,tag->artist);
        break;
    case AUDIO_ENUM_GENRE:
    {
        const char* genre = getGenreByTag(tag->genre);
        strcpy(buffer,genre);
        break;
    }
    case AUDIO_ENUM_PUBLISHDATE:
        strcpy(buffer,tag->year);
        break;
    default:
        return FALSE;
    }
    return TRUE;
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
    decoder->version = MP3_VERSION;
    decoder->maker = 0;

    decoder->stream = 0;
    decoder->ratio = 1.0f;
    decoder->impl = new gdecoderimpl();

    decoder->getint = decoder_get_uint32;
    return decoder;
}

static gaddons addon;

#if defined(__cplusplus)
extern "C"
{
#endif

gaddons* GAPIENTRY ginstance(void* ctx)
{
    int error = mpg123_init();

    stream_file_ = (stream_file_ptr)gaudio_address_get("stream_file");
    stream_tell_ = (stream_tell_ptr)gaudio_address_get("stream_tell");
    stream_seek_ = (stream_seek_ptr)gaudio_address_get("stream_seek");
    stream_read_ = (stream_read_ptr)gaudio_address_get("stream_read");

    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_AUDIO;
    addon.addon = new gmp3decreator();
    return &addon;
}

#if defined(__cplusplus)
}
#endif






