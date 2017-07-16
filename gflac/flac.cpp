#include "util.h"
#include "flac.h"
#include "../gaudio/gmain.h"

#define strm (streamBuffer*)((flac*)client)->decoder->stream

template<uint32_t shift,int32_t offset,typename T>
static void copySamples(T* data,const FLAC__int32* const buffer[],uint32_t off,uint32_t todo,uint32_t channels)
{
    for(uint32_t i = 0;i < todo;i++)
    {
        for(uint32_t c = 0;c < channels;c++)
            *(data++) = (buffer[c][off+i]>>shift) + offset;
    }
}
    
template<uint32_t bits>
static void copySamplesFloat(float *data,const FLAC__int32* const buffer[],uint32_t off,uint32_t todo,uint32_t channels)
{
    for(uint32_t i = 0;i < todo;i++)
    {
        for(uint32_t c = 0;c < channels;c++)
            *(data++) = buffer[c][off+i] * (1./((1u<<(bits-1))-1));
    }
}


flac::~flac()
{
    if(flc) 
    {
        FLAC__stream_decoder_finish(flc);
        FLAC__stream_decoder_delete(flc);
    }           
}

FLAC__StreamDecoderWriteStatus 
flac::write(const FLAC__Frame* frame,const FLAC__int32* const buf[])
{
    int channel_count = frame->header.channels;
    int samples_per_channel = frame->header.blocksize;
    int bytes_per_sample = frame->header.bits_per_sample / 8;
    int total_size = channel_count * samples_per_channel * bytes_per_sample;
    pos += samples_per_channel;
 
    uint8_t* data = out + outlen;
    uint32_t todo = std::min<uint32_t>((outmax-outlen)/align,frame->header.blocksize);
    if(frame->header.bits_per_sample == 8)
        copySamples<0,128>((uint8_t*)data,buf,0,todo,frame->header.channels);
    else if(frame->header.bits_per_sample == 16)
        copySamples<0,0>((int16_t*)data,buf,0,todo,frame->header.channels);
    else if(frame->header.bits_per_sample == 24)
    {
        copySamples<8,0>((int16_t*)data,buf,0,todo,frame->header.channels);
    }
    else if(frame->header.bits_per_sample == 32)
    {
        copySamples<16,0>((int16_t*)data,buf,0,todo,frame->header.channels);
    }
    
    outlen += align *todo;

    if(todo < frame->header.blocksize)
    {
        uint32_t offset = todo;
        todo = frame->header.blocksize - todo;

        uint32_t blocklen = todo * align;
        uint32_t start = buffer.size();
        buffer.resize(start+blocklen);
        data = &buffer[start];

        if(frame->header.bits_per_sample == 8)
            copySamples<0,128>((uint8_t*)data,buf,offset,todo,frame->header.channels);
        else if(frame->header.bits_per_sample == 16)
            copySamples<0,0>((int16_t*)data,buf,offset,todo,frame->header.channels);
            /*else if(frame->header.bits_per_sample == 24)
            {
                    CopySamples<8,0>((ALshort*)data, buffer, offset,
                                     todo, frame->header.channels);
            }
            else if(frame->header.bits_per_sample == 32)
            {
                    CopySamples<16,0>((ALshort*)data, buffer, offset,
                                      todo, frame->header.channels);
            }*/
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;       
}

FLAC__StreamDecoderReadStatus 
flac::read_callback(const FLAC__StreamDecoder* decoder,FLAC__byte buffer[],uint32_t* bytes,void* client)
{   
    if(*bytes > 0)
    {
        *bytes = stream_read_(strm,buffer,1,*bytes);
        if(*bytes == 0) 
        {
            return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
        } 
        else 
        {
            return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
        }
    }
    else
        return FLAC__STREAM_DECODER_READ_STATUS_ABORT;                           
}

FLAC__StreamDecoderSeekStatus 
flac::seek_callback(const FLAC__StreamDecoder* decoder,FLAC__uint64 offset,void* client)
{
    int32_t ret = stream_seek_(strm,offset,SEEK_SET); 
    if(ret == TRUE)
        return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
    return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;                            
}

FLAC__StreamDecoderTellStatus 
flac::tell_callback(const FLAC__StreamDecoder* decoder,FLAC__uint64* offset,void* client)
{
    int32_t pos = stream_tell_(strm); 
    if(pos<0)
        return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
    else    
    {   
        *offset = pos;
        return FLAC__STREAM_DECODER_TELL_STATUS_OK;
    }                          
}

FLAC__StreamDecoderLengthStatus 
flac::length_callback(const FLAC__StreamDecoder* decoder,FLAC__uint64* length,void* client)
{
    int32_t current = stream_tell_(strm); 
    if(current<0)
        return FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
    stream_seek_(strm,0,SEEK_END);
    int32_t total = stream_tell_(strm);    
    stream_seek_(strm,current,SEEK_SET);
    *length = total; 
    return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;                            
}

FLAC__bool 
flac::eof_callback(const FLAC__StreamDecoder* decoder,void* client)
{
    int32_t current = stream_tell_(strm); 
    stream_seek_(strm,0,SEEK_END);
    int32_t total = stream_tell_(strm);     
    stream_seek_(strm,current,SEEK_SET);
    if(current == total)
        return true;
    return false;                           
}

FLAC__StreamDecoderWriteStatus 
flac::write_callback(const FLAC__StreamDecoder* decoder,const FLAC__Frame* frame,const FLAC__int32* const buffer[],void* client)
{
    ((flac*)client)->write(frame,buffer);                       
}

void 
flac::metadata_callback(const FLAC__StreamDecoder* decoder,const FLAC__StreamMetadata* metadata,void* client)
{
	if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) 
    {
		((flac*)client)->samples = metadata->data.stream_info.total_samples;
		metadata->data.stream_info.bits_per_sample;
	    metadata->data.stream_info.channels;
		metadata->data.stream_info.sample_rate;	
        ((flac*)client)->align = 
             metadata->data.stream_info.channels * metadata->data.stream_info.bits_per_sample/8;
		/*FLAC__uint64 l = (FLAC__uint64)((double)(FLAC__int64)decoder->impl->total_samples/(double)decoder->impl->sample_rate*1000.0+0.5);
		if(l>INT_MAX)
		    l = INT_MAX;
		decoder->impl->duration = (unsigned)l;*/
	}
	else if(metadata->type == FLAC__METADATA_TYPE_VORBIS_COMMENT)
	{
	}                              
}

void 
flac::error_callback(const FLAC__StreamDecoder* decoder,FLAC__StreamDecoderErrorStatus status,void* client)
{
    //fprintf(stderr, "Got error callback: %s\n", FLAC__StreamDecoderErrorStatusString[status]);                           
}

bool flac::init(int32_t& ch,int32_t& fmt,int32_t& rate)
{
    flc = FLAC__stream_decoder_new();      
    if(flc == 0)
        return false;
        
    FLAC__stream_decoder_set_md5_checking(flc,false);
   	FLAC__stream_decoder_set_metadata_ignore_all(flc);
	FLAC__stream_decoder_set_metadata_respond(flc,FLAC__METADATA_TYPE_STREAMINFO);
	FLAC__stream_decoder_set_metadata_respond(flc,FLAC__METADATA_TYPE_VORBIS_COMMENT);        

    FLAC_API FLAC__StreamDecoderInitStatus status;
    status = FLAC__stream_decoder_init_stream(flc,read_callback,seek_callback,tell_callback,
        length_callback,eof_callback,write_callback,metadata_callback,error_callback,this);
        
    if(status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
    {     
        FLAC__stream_decoder_delete(flc);
        flc = 0;
        //delete decoder->impl;
        //decoder_destroy(decoder);  
        return false;
    }     
    
    if(!FLAC__stream_decoder_process_until_end_of_metadata(flc)) 
    {
        FLAC__stream_decoder_finish(flc);
        FLAC__stream_decoder_delete(flc);
        flc = 0;
        return false;
    }

    // process one frame so we can do something!
    if(!FLAC__stream_decoder_process_single(flc)) 
    {
        FLAC__stream_decoder_finish(flc);
        FLAC__stream_decoder_delete(flc);
        flc = 0;
        return false;
    }    
    
    //if(buffer.size() > 0)
    //    return false;
    
    ch = FLAC__stream_decoder_get_channels(flc);
    rate   = FLAC__stream_decoder_get_sample_rate(flc);
    format = FLAC__stream_decoder_get_bits_per_sample(flc); 
    
    channel = ch;
    fmt = format;
    samplerate = rate;
    //printf("%d,%d,%d,%d\n",channel,format,samplerate,samples);
    return true;    
}

int32_t flac::total()
{   
    return samples;
}   

void flac::reset()
{
    stream_seek_((streamBuffer*)decoder->stream,0,SEEK_SET);
    FLAC__stream_decoder_seek_absolute(flc,0);
    pos = 0;  
}

void flac::seek(int32_t position)
{
    if(FLAC__stream_decoder_seek_absolute(flc,position)) 
        pos = position;     
}

int32_t flac::position()
{
    int bytes_per_frame = channel * format/8;
    return 0;//pos - (quque.size() / bytes_per_frame);        
}

int32_t flac::read(uint8_t* data,int32_t bytes)
{
    out = data;
    outlen = 0;
    outmax = bytes;

    if(buffer.size() > 0)
    {
        size_t rem = std::min(buffer.size(),(size_t)bytes);
        memcpy(data,&buffer[0],rem);
        outlen += rem;
        buffer.erase(buffer.begin(),buffer.begin()+rem);
    }

    while(outlen < outmax)
    {
        if(FLAC__stream_decoder_process_single(flc) == false ||
             FLAC__stream_decoder_get_state(flc) == FLAC__STREAM_DECODER_END_OF_STREAM)
             break;
    }
    return outlen;      
}





