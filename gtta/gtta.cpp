#include <stdio.h>
#include <stdlib.h>
#include <libtta.h>
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

typedef struct 
{
	TTA_io_callback io;
	streamBuffer*   handle;
}TTA_io_callback_wrapper;

gdecoder* gdecoder_create(); 

gxcreator(tta)

int32_t gttadecreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_TTA)
       return TRUE;
    return FALSE;           
}

gdecoder* gttadecreator::create_by_extension(const char* extension)
{   
    if(!strncmp(extension,"tta",3))
        return gdecoder_create();
    return NULL;    
}

gdecoder* gttadecreator::create_by_decid(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_TTA)
       return gdecoder_create();
    return NULL;              
}

gdecoder* gttadecreator::create_by_buffer(int8_t* buffer,uint32_t length)
{
    if(buffer[0] == 'T' && buffer[1] == 'T' && buffer[2] == 'A' && buffer[3] == '1')
        return gdecoder_create();    
    return NULL;
}

void gttadecreator::destroy()
{
    delete this; 
}

const char* decoder_maker(gdecoder*)
{
    return "duwenhua2013";        
}

struct gdecoderimpl
{
    TTA_io_callback_wrapper io;
    tta::tta_decoder*       decoder;
    TTA_info                info;
    TTAuint8*               buffer;
    uint32_t                block;
    float                   current;
    gdecoderimpl():decoder(NULL),buffer(NULL),current(0.0f)
    {
    }      
};

void decoder_destroy(gdecoder* decoder)
{   
    delete decoder->impl->decoder;
    if(decoder->impl->buffer)
        delete []decoder->impl->buffer;
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
 
uint32_t decoder_get_long(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
        return decoder->impl->info.nch;
    else if(flag == AUDIO_ENUM_SAMPLERATE)
        return decoder->impl->info.sps;
    else if(flag == AUDIO_ENUM_FORMAT)
    {
        uint32_t format = decoder->impl->info.bps;
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
    {   
        float duration = (float)decoder->impl->info.samples/(float)decoder->impl->info.sps;
        return duration*1000.0f;
    }
    else if(flag == AUDIO_ENUM_POSITION)
        return 1000.0f*decoder->impl->current; 
    else if(flag == AUDIO_ENUM_BITERATE)
        return decoder->impl->info.nch*decoder->impl->info.sps*decoder->impl->info.bps;
    return FALSE;
}

TTAint32 tta_read(TTA_io_callback* io,TTAuint8* buffer,TTAuint32 length)
{    
    TTA_io_callback_wrapper* iocb = (TTA_io_callback_wrapper*)io; 
    return stream_read_((streamBuffer*)iocb->handle,buffer,sizeof(TTAuint8),length);    
}

TTAint64 tta_seek(TTA_io_callback* io,TTAint64 offset)
{
    TTA_io_callback_wrapper* iocb = (TTA_io_callback_wrapper*)io;
    return stream_seek_((streamBuffer*)iocb->handle,offset,SEEK_SET);      
}

void tta_position(TTAuint32 _1,TTAuint32 _2,TTAuint32 _3)
{  
}

int32_t decoder_init(gdecoder* decoder)
{   
    stream_seek_((streamBuffer*)decoder->stream,/*decoder->metainfo.id3v2length*/0,SEEK_SET);
    
    decoder->impl->io.io.read = &tta_read;
    decoder->impl->io.io.write = NULL;
    decoder->impl->io.io.seek = &tta_seek;
    decoder->impl->io.handle = (streamBuffer*)decoder->stream;
    
    decoder->impl->decoder = new tta::tta_decoder(&decoder->impl->io.io);           
    decoder->impl->decoder->init_get_info(&decoder->impl->info,0);
	
	decoder->impl->block = decoder->impl->info.nch * ((decoder->impl->info.bps + 7) / 8);
	decoder->impl->buffer = new TTAuint8[4096*decoder->impl->block+4];
	return TRUE;
}

int32_t decoder_seek(gdecoder* decoder,uint32_t position)
{   
    TTAuint32 k = position/1000;
    TTAuint32 v;
    decoder->impl->decoder->set_position(k,&v);
    if(v == k)
    {   
        decoder->impl->current = position;
        return  TRUE;
    }
    return FALSE;       
}

#define TTA_CAST(type)\
    type* short_p = (type*)decoder->impl->buffer;\
    for(uint32_t i=0;i<val;i++)\
        pcm[i] = gtofloat(short_p[i]);

uint32_t decoder_read(gdecoder* decoder,float* pcm,int32_t length)
{   
    if(length <= 0)
         return 0;
    int32_t format = decoder->impl->block/decoder->impl->info.nch;     
    uint32_t size = format*length;     
    uint32_t val = decoder->impl->decoder->process_stream((TTAuint8*)decoder->impl->buffer,size,tta_position);
    int32_t cnt = val;
    val *= format;
    
    if(format == 2)
    {
        TTA_CAST(int16_t)
    }
    else if(format == 4)
    {
        TTA_CAST(int32_t)
    }
    else
    {
        TTA_CAST(int8_t)
    }
    decoder->impl->current +=  cnt/(float)decoder->impl->info.sps;     
    return val;
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
    decoder->decid = AUDIO_FORMAT_TTA;
    decoder->version = 1013;
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
    addon.addon = new gttadecreator();
    return &addon;
}






