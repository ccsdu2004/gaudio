#include <stdio.h>
#include <stdlib.h>
#include <mpcdec/mpcdec.h>
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

mpc_int32_t gread_func(void* datasource,void* ptr,mpc_int32_t n)
{
    return stream_read_((streamBuffer*)datasource,ptr,1,n);       
}

mpc_bool_t gseek_func(void* datasource,mpc_int32_t offset)
{
    return !stream_seek_((streamBuffer*)datasource,offset,SEEK_SET);
}
  
mpc_int32_t gtell_func(void* datasource)
{
    return stream_tell_((streamBuffer*)datasource); 
}

mpc_int32_t get_size_func(void* data)
{
    mpc_int32_t cur = gtell_func(data);
    stream_seek_((streamBuffer*)data,0,SEEK_END);
    mpc_int32_t total = gtell_func(data);        
    stream_seek_((streamBuffer*)data,cur,SEEK_SET);
    return total;
}

mpc_bool_t canseek_impl(void *data)
{
    return true;
}

gdecoder* gdecoder_create(); 

gxcreator(mpc)

int32_t gmpcdecreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_MPC)
       return TRUE;
    return FALSE;           
}

gdecoder* gmpcdecreator::create_by_extension(const char* extension)
{   
    if(!strncmp(extension,"mpc",3))
        return gdecoder_create();
    return NULL;    
}

gdecoder* gmpcdecreator::create_by_decid(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_MPC)
       return gdecoder_create();
    return NULL;              
}

gdecoder* gmpcdecreator::create_by_buffer(int8_t* buffer,uint32_t length)
{
    if(buffer[0] == 'M' && buffer[1] == 'P' && buffer[2] == '+')// && buffer[3] == 'F')
        return gdecoder_create();    
    return NULL;
}

void gmpcdecreator::destroy()
{
    delete this; 
}

const char* decoder_maker(gdecoder*)
{
    return "duwenhua2013";        
}

struct gdecoderimpl
{   
    mpc_streaminfo info;
    mpc_reader     reader;
    mpc_decoder    decoder;
    MPC_SAMPLE_FORMAT* buffer;
    uint32_t       use;
    uint32_t       current;
 
    gdecoderimpl()                        
    {    
        buffer = new MPC_SAMPLE_FORMAT[4096*2+10124];
        use = 0;
        current = 0;                                 
    }      
    
    ~gdecoderimpl()
    {
        delete []buffer; 
    }
};

void decoder_destroy(gdecoder* decoder)
{   
    delete decoder->impl;
    decoder->impl = 0;
    delete decoder;     
}
 
uint32_t decoder_get_long(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
        return decoder->impl->info.channels;
    else if(flag == AUDIO_ENUM_SAMPLERATE)
        return decoder->impl->info.sample_freq;
    else if(flag == AUDIO_ENUM_FORMAT)
    {
        return AUDIO_FORMAT_TYPE_MPC;                  
    }             
    else if(flag == AUDIO_ENUM_DURATION)
        return mpc_streaminfo_get_length(&decoder->impl->info)*1000;
    else if(flag == AUDIO_ENUM_POSITION)
    {
        mpc_int64_t total = mpc_streaminfo_get_length_samples(&decoder->impl->info);
        return mpc_streaminfo_get_length(&decoder->impl->info)*1000*decoder->impl->current/total;
    }
}

int32_t decoder_init(gdecoder* decoder)
{   
    decoder->impl->reader.read = gread_func;
    decoder->impl->reader.seek = gseek_func;
    decoder->impl->reader.tell = gtell_func;
    decoder->impl->reader.get_size = get_size_func;
    decoder->impl->reader.canseek = canseek_impl;
    decoder->impl->reader.data = decoder->stream;
    
    mpc_streaminfo_init(&decoder->impl->info);  
    if(mpc_streaminfo_read(&decoder->impl->info,&decoder->impl->reader) != ERROR_CODE_OK) 
        return FALSE;
    mpc_decoder_setup(&decoder->impl->decoder,&decoder->impl->reader);
    if(!mpc_decoder_initialize(&decoder->impl->decoder,&decoder->impl->info)) 
        return FALSE;
    return TRUE;
}

int32_t decoder_seek(gdecoder* decoder,uint32_t position)
{   
    mpc_int64_t total = mpc_streaminfo_get_length_samples(&decoder->impl->info);
    mpc_int64_t duration = mpc_streaminfo_get_length(&decoder->impl->info)*1000;
    decoder->impl->current = (float)position/duration*total;
    return mpc_decoder_seek_seconds(&decoder->impl->decoder,position*1000);
}
 
int32_t decoder_meta(gdecoder* decoder,int32_t flag,char* buffer)
{   
    return FALSE;                
}

uint32_t decoder_read(gdecoder* decoder,float* pcm,int32_t length)
{   
    while(true)
    {    
        mpc_uint32_t read = mpc_decoder_decode(&decoder->impl->decoder,decoder->impl->buffer+decoder->impl->use,0,0);
        if(read == 0)
        {
            for(uint32_t i=0;i<decoder->impl->use;i++)
                pcm[i] = gtofloat(decoder->impl->buffer[i]); 
            uint32_t ret = decoder->impl->use;
            decoder->impl->use = 0;
            return ret;        
        }
        else 
        {   
            decoder->impl->use += 2*read;
            if(decoder->impl->use >= length)
            {
                for(uint32_t i=0;i<decoder->impl->use;i++)
                    pcm[i] = gtofloat(decoder->impl->buffer[i]);
                std::copy(decoder->impl->buffer+length,decoder->impl->buffer+decoder->impl->use,decoder->impl->buffer);    
                decoder->impl->use -= length;
                decoder->impl->current += length/2;
                return length;          
            }
        }
    }   
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
    addon.addon = new gmpcdecreator();
    return &addon;
}






