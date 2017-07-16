#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "util.h"
#include "flacstream.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/gdecreator.h"
#include "../gaudio/gmain.h"

stream_tell_ptr stream_tell_ = NULL;
stream_read_ptr stream_read_ = NULL;
stream_seek_ptr stream_seek_ = NULL;

gdecoder* gdecoder_create(); 

gxcreator(flac)

int32_t gflacdecreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_FLAC)
       return TRUE;
    return FALSE;           
}

gdecoder* gflacdecreator::create_by_extension(const char* extension)
{   
    if(!strncmp(extension,"flac",4))
        return gdecoder_create();
    return NULL;    
}

gdecoder* gflacdecreator::create_by_decid(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_FLAC)
       return gdecoder_create();
    return NULL;              
}

gdecoder* gflacdecreator::create_by_buffer(int8_t* buffer,uint32_t length)
{
    if(buffer[0] == 'f' && buffer[1] == 'L' && buffer[2] == 'a' && buffer[3] == 'C')
        return gdecoder_create();    
    return NULL;
}

void gflacdecreator::destroy()
{
    delete this; 
}

const char* decoder_maker(gdecoder*)
{
    return "http://www.geek-audio.org/";        
}

struct gdecoderimpl
{   
    flacStream* flc;
    int32_t format;
    int32_t channel;
    int32_t samplerate;
};

void decoder_destroy(gdecoder* decoder)
{   
    delete decoder->impl;
    decoder->impl = 0;
    delete decoder;     
}

uint32_t decoder_read(gdecoder* decoder,float* pcm,int32_t length)
{   
    uint32_t read;
    uint32_t size = decoder->impl->format/8*length;
    uint8_t* buffer = new uint8_t[size];
    read = decoder->impl->flc->GetData((uint8_t*)buffer,size);

    castbuffer cast;
    cast.uchar = buffer;
    if(decoder->impl->format == 16)
    {   
        read /= 2;
        for(int32_t i=0;i<read;i++)
            pcm[i] = gtofloat(cast.ptr16[i]); 
    }
    else if(decoder->impl->format == 32)
    {   
        read /= 4;
        for(int32_t i=0;i<read;i++)
            pcm[i] = gtofloat(cast.ptr32[i]);
    }
    else if(decoder->impl->format == 8)
    {   
        for(int32_t i=0;i<read;i++)
            pcm[i] = gtofloat(cast.uchar[i]);
    }
    else if(decoder->impl->format == 24)
    {
        read /= 3;
        gint24* int24 = (gint24*)buffer; 
        for(int32_t i=0;i<read;i++)
            pcm[i] = gtofloat(int24[i]);  
    }
    delete []buffer;
    return read;
}

uint32_t decoder_get_int32(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
    {
        return decoder->impl->channel;
    }
    else if(flag == AUDIO_ENUM_SAMPLERATE)
    {   
        return decoder->impl->samplerate;
    }
    else if(flag == AUDIO_ENUM_FORMAT)
    {
        switch(decoder->impl->format)
        {
        case 8:
            flag = AUDIO_FORMAT_TYPE_UINT8;
            break;
        case 16:
            flag = AUDIO_FORMAT_TYPE_INT16;
            break;
        case 24:
            flag = AUDIO_FORMAT_TYPE_INT24;
            break;
        case 32:
            flag = AUDIO_FORMAT_TYPE_INT32;
            break;
        default:
            flag = AUDIO_FORMAT_TYPE_INT16;
            break;                                                     
        }    
        return flag;
    }             
    else if(flag == AUDIO_ENUM_DURATION)
    {
        int64_t duration = 1000*decoder->impl->flc->GetLength()/decoder->impl->samplerate;
        return duration;
    }
    else if(flag == AUDIO_ENUM_BITERATE)
        return decoder->impl->channel * decoder->impl->format * decoder->impl->samplerate/8;
    else if(flag == AUDIO_ENUM_POSITION)
    {
        int64_t position = 1000*decoder->impl->flc->GetCurrent()/decoder->impl->samplerate;
        return position;
    }
    return FALSE;
}

int32_t decoder_init(gdecoder* decoder)
{   
    int32_t align;
    if(!decoder->impl->flc->init(&decoder->impl->format,&decoder->impl->samplerate,&decoder->impl->channel,&align))
    {   
        decoder_destroy(decoder);
        return FALSE;                                                                                                      
    }  

    return TRUE;  
}

int32_t decoder_seek(gdecoder* decoder,uint32_t position)
{   
    FLAC__uint64 offset = position*decoder->impl->samplerate;
    FLAC__uint64 total = decoder->impl->flc->GetLength();
    if(offset > total)
        return FALSE;
    decoder->impl->flc->Seek(total);    
    return TRUE;
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
    decoder->decid = AUDIO_FORMAT_FLAC;
    decoder->version = 0;
    decoder->maker = decoder_maker;
    decoder->stream = 0;
    decoder->ratio = 1.0f;
    decoder->impl = new gdecoderimpl();
    decoder->impl->flc = new flacStream(decoder);
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
    addon.addon = new gflacdecreator();
    return &addon;
}






