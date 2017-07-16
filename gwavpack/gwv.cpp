#include <stdio.h>
#include <stdlib.h>
#include <wavpack.h>
#include "util.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/gdecreator.h"
#include "../gaudio/gmain.h"

stream_file_ptr stream_file_ = NULL;
stream_tell_ptr stream_tell_ = NULL;
stream_read_ptr stream_read_ = NULL;
stream_seek_ptr stream_seek_ = NULL;

gdecoder* gdecoder_create(); 

gxcreator(wv)

int32_t gwvdecreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_WV)
       return TRUE;
    return FALSE;           
}

gdecoder* gwvdecreator::create_by_extension(const char* extension)
{   
    if(!strncmp(extension,"wv",2))
        return gdecoder_create();
    return NULL;    
}

gdecoder* gwvdecreator::create_by_decid(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_WV)
       return gdecoder_create();
    return NULL;              
}

gdecoder* gwvdecreator::create_by_buffer(int8_t* buffer,uint32_t length)
{
    if(buffer[0] == 'w' && buffer[1] == 'v' && buffer[2] == 'p' && buffer[3] == 'k')
        return gdecoder_create();    
    return NULL;
}

void gwvdecreator::destroy()
{
    delete this; 
}

const char* decoder_maker(gdecoder*)
{
    return "http://www.geek-audio.org/";        
}

struct gdecoderimpl
{   
    WavpackContext* wpc;
    WavpackStreamReader freader;
    int32_t* wvbuffer;
    uint8_t* wvbuffer8;
};

void decoder_destroy(gdecoder* decoder)
{   
    WavpackCloseFile(decoder->impl->wpc);
    if(decoder->impl->wvbuffer)
    {
        delete[] decoder->impl->wvbuffer;
        decoder->impl->wvbuffer = 0;
    }
    if(decoder->impl->wvbuffer8)
    {
        delete[] decoder->impl->wvbuffer8;
        decoder->impl->wvbuffer8 = 0;
    }
    delete decoder->impl;
    decoder->impl = 0;
    delete decoder;     
}

static uint8_t* format_samples(int bps,uint8_t* dst,int32_t* src,uint32_t samcnt)
{
    int32_t temp;

    switch(bps) 
    {
        case 1:
            while(samcnt--)
                *dst++ = *src++ + 128;
            break;
        case 2:
            while(samcnt--) 
            {
                *dst++ = (uint8_t)(temp = *src++);
                *dst++ = (uint8_t)(temp >> 8);
            }
            break;
        case 3:
            while(samcnt--) 
            {
                *dst++ = (uint8_t)(temp = *src++);
                *dst++ = (uint8_t)(temp >> 8);
                *dst++ = (uint8_t)(temp >> 16);
            }
            break;
        case 4:
            while(samcnt--) 
            {
                *dst++ = (uint8_t)(temp = *src++);
                *dst++ = (uint8_t)(temp >> 8);
                *dst++ = (uint8_t)(temp >> 16);
                *dst++ = (uint8_t)(temp >> 24);
            }
            break;
    }
    return dst;
}

static int32_t read_bytes(void* id,void* data,int32_t count)
{
    return stream_read_((streamBuffer*)id,data,sizeof(uint8_t),count);
}

static uint32_t get_pos(void* id)
{
    return stream_tell_((streamBuffer*)id);
}

static int set_pos_abs(void* id,uint32_t pos)
{
    return stream_seek_((streamBuffer*)id,pos,SEEK_SET);
}

static int set_pos_rel(void* id,int32_t delta,int mode)
{
    return stream_seek_((streamBuffer*)id,delta,mode);
}

static int push_back_byte(void* id,int c)
{
    return 0;
}

static uint32_t get_length(void* id)
{   
    streamBuffer* stream = (streamBuffer*)id;
    uint32_t current = stream_tell_(stream);
    stream_seek_(stream,0,SEEK_END);
    uint32_t length = stream_tell_(stream);
    stream_seek_(stream,current,SEEK_SET);
    return length;
}

static int can_seek(void* id)
{
    return 1;
}

static int32_t write_bytes(void* id,void* data,int32_t count)
{
    return 0;
}

uint32_t decoder_read(gdecoder* decoder,float* pcm,int32_t length)
{   
    int32_t format = WavpackGetBitsPerSample(decoder->impl->wpc);
    int32_t read = 0;
    switch(format)
    {
    case 8:
        read = length/4;            
        break;  
    case 16:
        read = length/2;
        break;
    case 24:
        read = length/4*3;
        break;
    case 32:
        read = length;
        break;                        
    }
    
    uint32_t readsize = WavpackUnpackSamples(decoder->impl->wpc,decoder->impl->wvbuffer,read);
    format_samples(format/8,decoder->impl->wvbuffer8,decoder->impl->wvbuffer,readsize*format/8);
    
    read = readsize*sizeof(int32_t);

    format /= 8;
        
    castbuffer cast;
    cast.uchar = decoder->impl->wvbuffer8;

    if(format == 2)
    {   
        read /= 2;
        for(int32_t i=0;i<read;i++)
            pcm[i] = gtofloat(cast.ptr16[i]); 
    }
    else if(format == 4)
    {   
        read /= 4;
        for(int32_t i=0;i<read;i++)
            pcm[i] = gtofloat(cast.ptr32[i]);
    }
    else if(format == 1)
    {   
        for(int32_t i=0;i<read;i++)
            pcm[i] = gtofloat(cast.uchar[i]);
    }
    else if(format == 3)
    {
        read /= 3;
        gint24* int24 = (gint24*)decoder->impl->wvbuffer8;
        for(int32_t i=0;i<read;i++)
            pcm[i] = gtofloat(int24[i]);  
    } 
    return read;
}

uint32_t decoder_get_int32(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
    {
        return WavpackGetNumChannels(decoder->impl->wpc);
    }
    else if(flag == AUDIO_ENUM_SAMPLERATE)
    {   
        return WavpackGetSampleRate(decoder->impl->wpc);
    }
    else if(flag == AUDIO_ENUM_FORMAT)
    {   
        int32_t format = WavpackGetBitsPerSample(decoder->impl->wpc);
        switch(format)
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
        uint32_t length = WavpackGetNumSamples(decoder->impl->wpc);
        if(length == -1)
            return -1;
        length *= 1000;
        length /= WavpackGetSampleRate(decoder->impl->wpc);     
        return length;
    }
    else if(flag == AUDIO_ENUM_BITERATE)
        return WavpackGetNumChannels(decoder->impl->wpc)*WavpackGetBitsPerSample(decoder->impl->wpc)/8*WavpackGetSampleRate(decoder->impl->wpc);
    else if(flag == AUDIO_ENUM_POSITION)
    {
        uint32_t position = WavpackGetSampleIndex(decoder->impl->wpc)*1000/WavpackGetSampleRate(decoder->impl->wpc); 
        return position;
    }
    return FALSE;
}

int32_t decoder_init(gdecoder* decoder)
{   
    char error [80] = {0};
    int32_t flag = OPEN_2CH_MAX | OPEN_NORMALIZE;
    const char* filepath = stream_file_((streamBuffer*)decoder->stream);
    int32_t len = filepath == 0 ? 0 : strlen(filepath);
    if(len == 0)
        decoder->impl->wpc = WavpackOpenFileInputEx(&decoder->impl->freader,decoder->stream,0,error,flag,15); 
    else
        decoder->impl->wpc = WavpackOpenFileInput(filepath,error,flag,15);  
    
    if(decoder->impl->wpc)
    {
        int32_t channel = WavpackGetNumChannels(decoder->impl->wpc);
        int32_t samplerate = WavpackGetSampleRate(decoder->impl->wpc);
        int32_t format = WavpackGetBitsPerSample(decoder->impl->wpc);                    
        if(channel < 1 || samplerate > 48000 || (format != 8 && format != 16 && format != 24 && format != 32))
        {    
            WavpackCloseFile(decoder->impl->wpc);  
            return FALSE;
        }
        decoder->impl->wvbuffer  = new int32_t[4096*8*2];
        decoder->impl->wvbuffer8 = new uint8_t[4096*2*2];         
        return TRUE;
    } 
    return FALSE;
}

int32_t decoder_seek(gdecoder* decoder,uint32_t position)
{      
    uint32_t samplerate = WavpackGetSampleRate(decoder->impl->wpc);
    position /= 1000;
    position *= samplerate;   
    return WavpackSeekSample(decoder->impl->wpc,position);   
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
    decoder->decid = AUDIO_FORMAT_WV;
    decoder->version = 0;
    decoder->maker = decoder_maker;
    decoder->stream = 0;
    decoder->ratio = 1.0f;
    decoder->impl = new gdecoderimpl();
    decoder->impl->wpc = 0;
    decoder->impl->wvbuffer = 0;
    decoder->impl->wvbuffer8 = 0;
    decoder->impl->freader.read_bytes = read_bytes;
    decoder->impl->freader.get_pos = get_pos;
    decoder->impl->freader.set_pos_abs = set_pos_abs;
    decoder->impl->freader.set_pos_rel = set_pos_rel;
    decoder->impl->freader.push_back_byte = push_back_byte;
    decoder->impl->freader.get_length = get_length;
    decoder->impl->freader.can_seek = can_seek;
    decoder->impl->freader.write_bytes = 0;
    decoder->getint = decoder_get_int32;
    return decoder;          
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{   
    stream_file_ = (stream_file_ptr)gaudio_address_get("stream_file");
    stream_tell_ = (stream_tell_ptr)gaudio_address_get("stream_tell");
    stream_seek_ = (stream_seek_ptr)gaudio_address_get("stream_seek"); 
    stream_read_ = (stream_read_ptr)gaudio_address_get("stream_read"); 
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_AUDIO;
    addon.addon = new gwvdecreator();
    return &addon;
}






