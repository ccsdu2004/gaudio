#include <stdio.h>
#include <stdlib.h>
#include <FLAC/stream_decoder.h>
#include "../gaudio/gaddons.h"
#include "../gaudio/gdecreator.h"
#include "../gaudio/streamBuffer.h"
#include "../gaudio/gexternal.h"
#include "dither.h"

typedef long (*stream_read_ptr)(streamBuffer* stream,void* ptr,int32_t size,long n);
typedef long (*stream_tell_ptr)(streamBuffer* stream);
typedef long (*stream_seek_ptr)(streamBuffer* stream,long offset,int32_t flag);

static stream_tell_ptr stream_tell_ = NULL;
static stream_read_ptr stream_read_ = NULL;
static stream_seek_ptr stream_seek_ = NULL;

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
    if(buffer[0] == 'f' && buffer[1] == 'l' && buffer[2] == 'a' && buffer[3] == 'C')
        return gdecoder_create();    
    return NULL;
}

void gflacdecreator::destroy()
{
    delete this; 
}

char* decoder_maker(gdecoder*)
{
    return "duwenhua2013";        
}

struct gdecoderimpl
{   
    FLAC__StreamDecoder* decoder;
	FLAC__uint64 total_samples;
	unsigned bits_per_sample;
	unsigned channels;
	unsigned sample_rate;
	int length_in_msec;
	unsigned average_bps;
    FLAC__int32* buffer;
    unsigned framelength;
};

void decoder_destroy(gdecoder* decoder)
{   
    FLAC__stream_decoder_delete(decoder->impl->decoder);
    delete []decoder->impl->buffer;
    delete decoder->impl;
    decoder->impl = 0;
    delete decoder;     
}

FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder* dec,const FLAC__Frame *frame,const FLAC__int32* const buffer[],void* client)
{
    printf("this\n");                           
	gdecoder* decoder = (gdecoder*)client;
	const unsigned channels = decoder->impl->channels;
    const unsigned wide_samples = frame->header.blocksize;

    //printf("size:%d\n",channels);
	//decoder->impl->framelength += channels*wide_samples;
	//l2 += channels*wide_samples;
	//printf("__%d,%d,%d\n",l2,channel,wide_samples);
	printf("%d\n",wide_samples);
	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void metadata_callback(const FLAC__StreamDecoder* dec,const FLAC__StreamMetadata *metadata,void* client)
{
	gdecoder* decoder = (gdecoder*)client;
	if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) 
    {
		decoder->impl->total_samples = metadata->data.stream_info.total_samples;
		decoder->impl->bits_per_sample = metadata->data.stream_info.bits_per_sample;
		decoder->impl->channels = metadata->data.stream_info.channels;
		decoder->impl->sample_rate = metadata->data.stream_info.sample_rate;
		
		FLAC__uint64 l = (FLAC__uint64)((double)(FLAC__int64)decoder->impl->total_samples/(double)decoder->impl->sample_rate*1000.0+0.5);
		if(l>INT_MAX)
		    l = INT_MAX;
		decoder->impl->length_in_msec = (int)l;
	}
	else if(metadata->type == FLAC__METADATA_TYPE_VORBIS_COMMENT)
	{
	}	
}

void error_callback(const FLAC__StreamDecoder *decoder,FLAC__StreamDecoderErrorStatus status,void* client_data)
{
    fprintf(stderr, "Got error callback: %s\n", FLAC__StreamDecoderErrorStatusString[status]);
}

FLAC__StreamDecoderReadStatus read_callback(const FLAC__StreamDecoder* decoder,FLAC__byte buffer[],size_t* bytes,void* client)
{   
    if(*bytes > 0)
    {  
        gdecoder* impl = (gdecoder*)client;            
        *bytes = stream_read_((streamBuffer*)impl->stream,buffer,sizeof(FLAC__byte),*bytes); 
        if(*bytes > 0)
            return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
        else
            return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
    }     
    else
       return FLAC__STREAM_DECODER_READ_STATUS_ABORT;              
}

FLAC__StreamDecoderTellStatus tell_callback(const FLAC__StreamDecoder* decoder,FLAC__uint64* absolute_byte_offset,void* client)
{   
    gdecoder* impl = (gdecoder*)client; 
    int32_t pos = stream_tell_((streamBuffer*)impl->stream); 
    if(pos<0)
        return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
    else    
    {   
        *absolute_byte_offset = pos;
        return FLAC__STREAM_DECODER_TELL_STATUS_OK;
    }
}

FLAC__StreamDecoderLengthStatus length_callback(const FLAC__StreamDecoder* decoder,FLAC__uint64* length,void* client)
{   
    gdecoder* impl = (gdecoder*)client;  
    int32_t current = stream_tell_((streamBuffer*)impl->stream); 
    if(current<0)
        return FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
    stream_seek_((streamBuffer*)impl->stream,0,SEEK_END);
    int32_t total = stream_tell_((streamBuffer*)impl->stream);    
    stream_seek_((streamBuffer*)impl->stream,current,SEEK_SET);
    *length = total; 
    return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

FLAC__StreamDecoderSeekStatus seek_callback(const FLAC__StreamDecoder* decoder,FLAC__uint64 absolute_byte_offset,void* client)
{   
    gdecoder* impl = (gdecoder*)client; 
    int32_t ret = stream_seek_((streamBuffer*)impl->stream,absolute_byte_offset,SEEK_SET); 
    if(ret == TRUE)
        return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
    return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;    
}

FLAC__bool eof_callback(const FLAC__StreamDecoder*decoder,void* client)
{  
    gdecoder* impl = (gdecoder*)client;        
    int32_t current = stream_tell_((streamBuffer*)impl->stream); 
    stream_seek_((streamBuffer*)impl->stream,0,SEEK_END);
    int32_t total = stream_tell_((streamBuffer*)impl->stream);     
    stream_seek_((streamBuffer*)impl->stream,current,SEEK_SET);
    if(current == total)
        return true;
    return false;               
}

inline float gfloat(int32_t val)
{
    return val*(1.0f/2147483647.0f);
}

uint32_t decoder_read(gdecoder* decoder,float* pcm,int32_t length)
{
	return length;
}

uint32_t decoder_get_long(gdecoder* decoder,uint32_t flag)
{
    if(flag == CHANNEL)
    {
        return decoder->impl->channels;
    }
    else if(flag == SAMPLERATE)
    {   
        return decoder->impl->sample_rate;
    }
    else if(flag == FORMAT)
    {
        /*uint32_t format = decoder->impl->bits_per_sample;
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
        return uint32_t(-1);  */
        return AUDIO_FORMAT_TYPE_INT16;                
    }             
    else if(flag == DURATION)
    {
        return decoder->impl->length_in_msec;  
    }
    else if(flag == POSITION)
    {
        /*FLAC__uint64 position;
        bool ret = FLAC__stream_decoder_get_decode_position(decoder->impl->decoder,&position);
        if(ret == true)
        {
            float percent = position/FLAC__stream_decoder_get_total_samples(decoder->impl->decoder);   
            return decoder_get_long(decoder,DURATION);
        }
        else*/
            return 0;
    }
}

int32_t decoder_init(gdecoder* decoder)
{   
    FLAC_API FLAC__StreamDecoderInitStatus status;
    status = FLAC__stream_decoder_init_stream(
        decoder->impl->decoder,read_callback,
        seek_callback,tell_callback,
        length_callback,eof_callback,
        write_callback,metadata_callback,
        error_callback,decoder);
    if(status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
    {     
        FLAC__stream_decoder_delete(decoder->impl->decoder);
        delete decoder->impl;
        decoder_destroy(decoder);  
        return FALSE;          
    }    
    return FLAC__stream_decoder_process_until_end_of_metadata(decoder->impl->decoder); 
    //average_bps = (unsigned)(filesize / (125.*(double)(FLAC__int64)decoder->impl->total_samples/(double)decoder->impl->sample_rate));
    //return TRUE;
}

int32_t decoder_seek(gdecoder* decoder,uint32_t position)
{
    //uint64_t length = position*decoder->impl->format/8*decoder->impl->samplerate/1000*decoder->impl->channel;
    //stream_seek_((streamBuffer*)decoder->stream,WAV_HEADER+length,SEEK_SET);
    //FLAC__stream_decoder_seek_absolute(decoder->impl->decoder,position);
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
    decoder->impl->decoder = FLAC__stream_decoder_new();
    if(!decoder->impl->decoder)
    {    
        FLAC__stream_decoder_delete(decoder->impl->decoder);
        delete decoder->impl;
        delete decoder;
        return 0;
    }
    
    FLAC__stream_decoder_set_md5_checking(decoder->impl->decoder,false);
   	FLAC__stream_decoder_set_metadata_ignore_all(decoder->impl->decoder);
	FLAC__stream_decoder_set_metadata_respond(decoder->impl->decoder,FLAC__METADATA_TYPE_STREAMINFO);
	FLAC__stream_decoder_set_metadata_respond(decoder->impl->decoder,FLAC__METADATA_TYPE_VORBIS_COMMENT);
    decoder->getlong = decoder_get_long;
    decoder->impl->buffer = new FLAC__int32[4096*2];
    //decoder->impl->framelength = 0; 
    return decoder;          
}

static gaddons addon;

gaddons* ginstance(void* ctx)
{
    stream_tell_ = (stream_tell_ptr)gaudio_address_get("stream_tell");
    stream_seek_ = (stream_seek_ptr)gaudio_address_get("stream_seek"); 
    stream_read_ = (stream_read_ptr)gaudio_address_get("stream_read"); 
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_AUDIO;
    addon.addon = new gflacdecreator();
    return &addon;
}






