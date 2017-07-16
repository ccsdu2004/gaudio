#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dumb/dumb.h>
#include "gaudio.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/gdecreator.h"
#include "../gaudio/streamBuffer.h"
#include "../gaudio/gmain.h"

#define DUMB_AUDIO_FREQ (44100.0f)
#define DUMB_CHANNELS   (2)
#define DUMB_FORMAT     (16)

#undef TORQUE_BIG_ENDIAN
 
typedef int32_t(*stream_read_ptr)(streamBuffer* stream,void* ptr,int32_t size,int32_t n);
typedef int32_t(*stream_tell_ptr)(streamBuffer* stream);
typedef int32_t(*stream_seek_ptr)(streamBuffer* stream,int32_t offset,int32_t flag);

static stream_tell_ptr stream_tell_ = NULL;
static stream_read_ptr stream_read_ = NULL;
static stream_seek_ptr stream_seek_ = NULL;

gdecoder* gdecoder_create();

gxcreator(_200)

int32_t g_200decreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_IT)
        return TRUE;
    else if(dec == AUDIO_FORMAT_S3M)
        return TRUE;
    else if(dec == AUDIO_FORMAT_MOD)
        return TRUE;
    else if(dec == AUDIO_FORMAT_XM)
        return TRUE;         
    return FALSE;
}

void g_200decreator::destroy()
{
    delete this;
}

gdecoder* g_200decreator::create_by_decid(int32_t dec,int32_t mask)
{
    if(is_support(dec,mask))
       return gdecoder_create();
    return NULL;
}

gdecoder* g_200decreator::create_by_extension(const char* extension)
{
    if(!strncmp(extension,"mod",3))
        return gdecoder_create();
    if(!strncmp(extension,"s3m",3))
        return gdecoder_create();
    if(!strncmp(extension,"xm",2))
        return gdecoder_create();
    if(!strncmp(extension,"it",2))
        return gdecoder_create();            
    return NULL;
}

gdecoder* g_200decreator::create_by_buffer(int8_t* buffer,uint32_t length)
{   
    //Exte
    if(buffer[0] == 'E' && buffer[1] == 'x' && buffer[2] == 't' && buffer[3] == 'e')
        return gdecoder_create();
    return NULL;
}

static const char* GAPIENTRY decoder_maker(gdecoder*)
{
    return "duwenhua2013";
}

struct gdecoderimpl
{
    uint32_t format;
    uint32_t channel;
    uint32_t samplerate;
    uint32_t duration;
    uint32_t length;
    DUH *duh; 
    DUH_SIGRENDERER *sr;	
    char* buffer;
    gdecoderimpl():
    format DUMB_FORMAT,
    channel DUMB_CHANNELS,
    samplerate DUMB_AUDIO_FREQ,
    duration(0),
    length(0),
    duh(0),
    sr(0),
    buffer(0)
    {
    }
    
    ~gdecoderimpl()
    {
	    if(sr)
	        duh_end_sigrenderer(sr);
        if(duh)
	        unload_duh(duh);    
	    if(buffer)
            delete []buffer;    
    }
};

static void GAPIENTRY decoder_destroy(gdecoder* decoder)
{
    delete decoder->impl;
    decoder->impl = 0;
    delete decoder;
}

static uint32_t GAPIENTRY decoder_get_long(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
        return decoder->impl->channel;
    else if(flag == AUDIO_ENUM_SAMPLERATE)
        return decoder->impl->samplerate;
    else if(flag == AUDIO_ENUM_FORMAT)
    {
        return AUDIO_FORMAT_TYPE_INT16;
    }
    else if(flag == AUDIO_ENUM_DURATION)
        return decoder->impl->duration;
    else if(flag == AUDIO_ENUM_POSITION)
    {
        float f = duh_sigrenderer_get_position(decoder->impl->sr)/(float)duh_get_length(decoder->impl->duh);
        f *= decoder->impl->duration;
        return f;
    }
    else if(flag == AUDIO_ENUM_BITERATE)
    {
        return decoder->impl->channel*decoder->impl->samplerate*decoder->impl->format;
    }
    return FALSE;
}

#define TRYREAD(ptr,fn)\
    if(ptr == 0)\
    {\
        dumb = dumbfile_open_memory(decoder->impl->buffer,decoder->impl->length);\
        if(dumb == 0)\
            return FALSE;\
        decoder->impl->duh = fn(dumb);\
        if(decoder->impl->duh == NULL)\
        {\
            dumbfile_close(dumb);\
	    }\
    }

static int32_t GAPIENTRY decoder_init(gdecoder* decoder)
{
    stream_seek_((streamBuffer*)decoder->stream,0,SEEK_END);
    decoder->impl->length = stream_tell_((streamBuffer*)decoder->stream);
    //stream_seek_((streamBuffer*)decoder->stream,decoder->metainfo.id3v2length,SEEK_SET);
    stream_seek_((streamBuffer*)decoder->stream,0,SEEK_SET);
    
    decoder->impl->buffer = new char[decoder->impl->length];
    if(decoder->impl->buffer == 0)
        return FALSE;
        
    stream_read_((streamBuffer*)decoder->stream,decoder->impl->buffer,sizeof(char),decoder->impl->length);
    decoder->impl->duh = 0;
    
    DUMBFILE* dumb = 0;
    TRYREAD(decoder->impl->duh,dumb_read_xm)
    TRYREAD(decoder->impl->duh,dumb_read_mod)
    TRYREAD(decoder->impl->duh,dumb_read_it)
    TRYREAD(decoder->impl->duh,dumb_read_s3m)
    
    if(dumb != 0)
        dumbfile_close(dumb);
    
    if(decoder->impl->duh == NULL)
	    return false;
	
    decoder->impl->sr = duh_start_sigrenderer(decoder->impl->duh,0,DUMB_CHANNELS,0);
    DUMB_IT_SIGRENDERER* itsr = duh_get_it_sigrenderer(decoder->impl->sr);
    dumb_it_set_loop_callback(itsr,&dumb_it_callback_terminate,NULL);
    dumb_it_set_xm_speed_zero_callback(itsr,&dumb_it_callback_terminate,NULL);
    if(decoder->impl->duh != 0)
    {
        float t = (float)duh_get_length(decoder->impl->duh)/(float)DUMB_AUDIO_FREQ; 
        decoder->impl->duration = 500*t;
	}
    return (decoder->impl->sr != NULL);      
}

static int32_t GAPIENTRY decoder_seek(gdecoder* decoder,uint32_t position)
{
    uint64_t length = position*decoder->impl->format/8*decoder->impl->samplerate/1000*decoder->impl->channel;
    stream_seek_((streamBuffer*)decoder->stream,length/*+decoder->metainfo.id3v2length*/,SEEK_SET);
    return TRUE;
}

union castbuffer
{
    unsigned char* uchar;
    short* sptr;
};

long read_(gdecoderimpl* impl,unsigned char* buffer,int slength,int bigendianp)
{
    int depth = DUMB_FORMAT;
    int bigendian = bigendianp;
    int unsign = 0;
    int n_channels = DUMB_CHANNELS;
    float volume = 1.0f;
    float delta= 65536.0f/(float)DUMB_AUDIO_FREQ;

    long l = duh_render(impl->sr,depth,unsign,volume,delta,slength/4,buffer);
/*#ifdef TORQUE_BIG_ENDIAN
        short* sbuffer = (short*)buffer;
        for (int c=0; c < l * n_channels; c++)
        {
                short val = sbuffer[c];
                buffer[i*2] = (char)(val >> 8);
                buffer[i*2+1] = (char)val;
        }
#endif*/
    return l*4;
}

static uint32_t GAPIENTRY decoder_read(gdecoder* decoder,float* pcm,int32_t length)
{
    int slength = length*2;
    unsigned char* buffer = new unsigned char[4*length];
    int32_t l = read_(decoder->impl,buffer,slength,0);
    castbuffer cast;
    cast.uchar = buffer;
    l /= 2;
    for(int32_t i=0;i<l;i++)
        pcm[i] = gtofloat(cast.sptr[i]);
    delete []buffer;
    return l;
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
    decoder->decid = 0;
    decoder->version = 0;
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
    atexit(&dumb_exit);
    dumb_register_stdfiles(); 
	    
    stream_tell_ = (stream_tell_ptr)gaudio_address_get("stream_tell");
    stream_seek_ = (stream_seek_ptr)gaudio_address_get("stream_seek");
    stream_read_ = (stream_read_ptr)gaudio_address_get("stream_read");

    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_AUDIO;
    addon.addon = new g_200decreator();
    return &addon;
}






