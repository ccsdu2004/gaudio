#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sndfile.h>
#include "gaudio.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/gdecreator.h"
#include "../gaudio/streamBuffer.h"
#include "../gaudio/gmain.h"

#define SNDFILE_VERSION 1010

typedef int32_t(*stream_read_ptr)(streamBuffer* stream,void* ptr,int32_t size,int32_t n);
typedef int32_t(*stream_tell_ptr)(streamBuffer* stream);
typedef int32_t(*stream_seek_ptr)(streamBuffer* stream,int32_t offset,int32_t flag);

static stream_tell_ptr stream_tell_ = NULL;
static stream_read_ptr stream_read_ = NULL;
static stream_seek_ptr stream_seek_ = NULL;

sf_count_t snd_vio_get_filelen(void* user)
{   
    sf_count_t current = stream_tell_((streamBuffer*)user);
    stream_seek_((streamBuffer*)user,0,SEEK_END);
    sf_count_t length = stream_tell_((streamBuffer*)user);
    stream_seek_((streamBuffer*)user,current,SEEK_SET);
    return length;           
}

sf_count_t snd_vio_seek(sf_count_t offset,int whence,void* user)
{
    return stream_seek_((streamBuffer*)user,offset,whence);           
}

sf_count_t snd_vio_read(void* ptr,sf_count_t count,void* user)
{
    return stream_read_((streamBuffer*)user,ptr,1,count);           
}

sf_count_t snd_vio_tell(void* user)
{
    return stream_tell_((streamBuffer*)user);       
}

gdecoder* gdecoder_create();

gxcreator(sndfile)

int32_t gsndfiledecreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_WAV)
       return TRUE;
    else if(dec == AUDIO_FORMAT_AU)
       return TRUE;   
    else if(dec == AUDIO_FORMAT_AIFF)
       return TRUE;   
    else if(dec == AUDIO_FORMAT_FLAC)
       return TRUE;   
    if(dec == AUDIO_FORMAT_VOC)
       return TRUE;   
    if(dec == AUDIO_FORMAT_CAF)
       return TRUE;   
    if(dec == AUDIO_FORMAT_OGG)
       return TRUE;   
    return FALSE;
}

gdecoder* gsndfiledecreator::create_by_extension(const char* extension)
{
    if(!strncmp(extension,"wav",3) ||
       !strncmp(extension,"flac",4) ||
       !strncmp(extension,"au",2) ||
       !strncmp(extension,"aif",3) ||
       !strncmp(extension,"aiff",4) ||
       !strncmp(extension,"voc",3) ||
       !strncmp(extension,"caf",3) ||
       !strncmp(extension,"ogg",3)
       )
        return gdecoder_create();
    return NULL;
}

gdecoder* gsndfiledecreator::create_by_decid(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_WAV ||
       dec == AUDIO_FORMAT_AU  ||
       dec == AUDIO_FORMAT_AIFF ||
       dec == AUDIO_FORMAT_FLAC ||
       dec == AUDIO_FORMAT_VOC ||
       dec == AUDIO_FORMAT_OGG ||
       dec == AUDIO_FORMAT_CAF ||
       dec == AUDIO_FORMAT_MPC
       )
       return gdecoder_create();
    return NULL;
}

gdecoder* gsndfiledecreator::create_by_buffer(int8_t* buffer,uint32_t length)
{   
    //! wav
    if(buffer[0] == 'R' && buffer[1] == 'I' && buffer[2] == 'F' && buffer[3] == 'F')
        return gdecoder_create();
    //! aiff form    
    if(buffer[0] == 'F' && buffer[1] == 'O' && buffer[2] == 'R' && buffer[3] == 'M')
        return gdecoder_create();   
    //! au     
    if(buffer[0] == '.' && buffer[1] == 's' && buffer[2] == 'n' && buffer[3] == 'd')
        return gdecoder_create();   
    //! OggS
    if(buffer[0] == 'O' && buffer[1] == 'g' && buffer[2] == 'g' && buffer[3] == 'S')
        return gdecoder_create(); 
    //! MP+          
    if(buffer[0] == 'M' && buffer[1] == 'P' && buffer[2] == '+')
        return gdecoder_create(); 
    //! FLAC    
    if(buffer[0] == 'f' && buffer[1] == 'L' && buffer[2] == 'a' && buffer[3] == 'C')
        return gdecoder_create();     
    return NULL;
}

void gsndfiledecreator::destroy()
{
    delete this;
}

static const char* GAPIENTRY decoder_maker(gdecoder*)
{
    return "http://geek-audio.org";
}

struct gdecoderimpl
{
    SNDFILE*   sndfile;
	SF_INFO    info;
	sf_count_t counts;
	SF_VIRTUAL_IO io;
	
    gdecoderimpl():sndfile(0),counts(0){}
};

static void GAPIENTRY decoder_destroy(gdecoder* decoder)
{
    delete decoder->impl;
    decoder->impl = 0;
    delete decoder;
}

static uint32_t GAPIENTRY decoder_get_uint32(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
        return decoder->impl->info.channels;
    else if(flag == AUDIO_ENUM_SAMPLERATE)
        return decoder->impl->info.samplerate;
    else if(flag == AUDIO_ENUM_FORMAT)
    {   
        decoder->impl->info.format &= 0xFFFF;
        switch(decoder->impl->info.format)
        {
        case SF_FORMAT_PCM_S8:
             return AUDIO_FORMAT_TYPE_INT8;
        case SF_FORMAT_PCM_16:
             return AUDIO_FORMAT_TYPE_INT16;
        case SF_FORMAT_PCM_24:
             return AUDIO_FORMAT_TYPE_INT24;
        case SF_FORMAT_PCM_32:
             return AUDIO_FORMAT_TYPE_INT32;
        case SF_FORMAT_PCM_U8:
             return AUDIO_FORMAT_TYPE_UINT8;
        case SF_FORMAT_FLOAT:
             return AUDIO_FORMAT_TYPE_FLOAT;
        case SF_FORMAT_DOUBLE:      
             return AUDIO_FORMAT_TYPE_DOUBLE;     
        case SF_FORMAT_ULAW:
             return AUDIO_FORMAT_TYPE_ULAW;
        case SF_FORMAT_ALAW:
             return AUDIO_FORMAT_TYPE_ALAW;
        case SF_FORMAT_VORBIS:
             return AUDIO_FORMAT_TYPE_VORBIS;   
        case SF_FORMAT_DPCM_8:
             return AUDIO_FORMAT_TYPE_DPCM8;
        case SF_FORMAT_DPCM_16:             
             return AUDIO_FORMAT_TYPE_DPCM16;                                                              
        }
        return AUDIO_FORMAT_TYPE_NONE;
    }
    else if(flag == AUDIO_ENUM_DURATION)
    {   
        float blocks = decoder->impl->info.frames/(float)decoder->impl->info.samplerate;
        return 1000*blocks;
    }
    else if(flag == AUDIO_ENUM_POSITION)
    {
        float blocks = decoder->impl->counts/(float)decoder->impl->info.samplerate;
        return 1000*blocks;
    }
    else if(flag == AUDIO_ENUM_BITERATE)
    {
        return decoder->impl->info.channels*decoder->impl->info.samplerate;
    }
    return FALSE;
}

static int32_t GAPIENTRY decoder_init(gdecoder* decoder)
{
    stream_seek_((streamBuffer*)decoder->stream,0/*decoder->metainfo.id3v2length*/,SEEK_SET);
    
    decoder->impl->io.get_filelen = snd_vio_get_filelen;
    decoder->impl->io.seek = snd_vio_seek;
    decoder->impl->io.read = snd_vio_read;
    decoder->impl->io.tell = snd_vio_tell;
    
    decoder->impl->sndfile = 
         sf_open_virtual(&decoder->impl->io,SFM_READ,&decoder->impl->info,decoder->stream);
         
    if(decoder->impl->sndfile == 0)
    {
        stream_seek_((streamBuffer*)decoder->stream,/*decoder->metainfo.id3v2length*/0,SEEK_SET);                          
        return FALSE;                         
    }     
    return TRUE;
}

static int32_t GAPIENTRY decoder_seek(gdecoder* decoder,uint32_t position)
{
    float pos = position/1000.0f;
    pos *= decoder->impl->info.samplerate;   
    sf_seek(decoder->impl->sndfile,pos,SEEK_SET);
    return TRUE;
}

static uint32_t GAPIENTRY decoder_read(gdecoder* decoder,float* pcm,int32_t length)
{
    uint32_t read = sf_read_float(decoder->impl->sndfile,pcm,length);
    decoder->impl->counts += read;
    return read;
}

static int32_t GAPIENTRY decoder_meta(gdecoder* decoder,int32_t flag,char* buffer)
{       
    switch(flag)
    {
    case AUDIO_ENUM_TITLE:
         flag = SF_STR_TITLE;
         break;
    case AUDIO_ENUM_ALBUM:
         flag = SF_STR_ALBUM;
         break;
    case AUDIO_ENUM_ARTIST:
         flag = SF_STR_ARTIST;
         break;
    case AUDIO_ENUM_GENRE:
         flag = SF_STR_GENRE;
         break;    
    case AUDIO_ENUM_PUBLISHDATE:
         flag = SF_STR_DATE;
         break;
    default:
         return FALSE;                        
    }
    
    const char* value = sf_get_string(decoder->impl->sndfile,flag);
    if(value == 0 || strlen(value) == 0)
        return FALSE;
    strcpy(buffer,value);
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
    decoder->version = SNDFILE_VERSION;
    decoder->maker = decoder_maker;

    decoder->stream = 0;
    decoder->ratio = 1.0f;
    decoder->impl = new gdecoderimpl();

    decoder->getint = decoder_get_uint32;
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
    addon.addon = new gsndfiledecreator();
    return &addon;
}






