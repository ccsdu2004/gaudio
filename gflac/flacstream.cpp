#include "util.h"
#include "flacstream.h"

#define strm (streamBuffer*)((flacStream*)client)->decoder->stream

FLAC__StreamDecoderWriteStatus 
flacStream::WriteCallback(const FLAC__StreamDecoder*,const FLAC__Frame *frame,const FLAC__int32 *const buffer[],void *client)
{
    flacStream *self = static_cast<flacStream*>(client);
    if(self->format == 0)
    {
        uint32_t bps = frame->header.bits_per_sample;
        self->blockAlign = frame->header.channels * bps/8;
        self->samplerate = frame->header.sample_rate;
    }

    uint8_t *data = self->outBytes + self->outLen;
    int32_t todo = std::min<int32_t>((self->outMax-self->outLen) / self->blockAlign,frame->header.blocksize);
    if(frame->header.bits_per_sample == 8)
        CopySamples<0,128>((uint8_t*)data, buffer,0,todo, frame->header.channels);
    else if(frame->header.bits_per_sample == 16)
        CopySamples<0,0>((int16_t*)data, buffer,0,todo,frame->header.channels);
    else if(frame->header.bits_per_sample == 24)
    {
        CopySamples<8,0>((int16_t*)data, buffer, 0,todo, frame->header.channels);
    }
    else if(frame->header.bits_per_sample == 32)
        CopySamples<16,0>((int16_t*)data, buffer, 0,todo, frame->header.channels);
        
    self->outLen += self->blockAlign * todo;

    if(todo < frame->header.blocksize)
    {
        int32_t offset = todo;
        todo = frame->header.blocksize - todo;

        int32_t blocklen = todo * self->blockAlign;
        int32_t start = self->initialData.size();

        self->initialData.resize(start+blocklen);
        data = &self->initialData[start];

        if(frame->header.bits_per_sample == 8)
            CopySamples<0,128>((uint8_t*)data, buffer, offset,todo, frame->header.channels);
        else if(frame->header.bits_per_sample == 16)
            CopySamples<0,0>((int16_t*)data, buffer, offset,todo, frame->header.channels);
        else if(frame->header.bits_per_sample == 24)
            CopySamples<8,0>((int16_t*)data, buffer, offset,todo, frame->header.channels);
        else if(frame->header.bits_per_sample == 32)
            CopySamples<16,0>((int16_t*)data, buffer, offset,todo, frame->header.channels);
    }

    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void flacStream::MetadataCallback(const FLAC__StreamDecoder*,const FLAC__StreamMetadata* metadata,void* client)
{
    if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) 
    {
        //((flacStream*)client)->samples = metadata->data.stream_info.total_samples;
  		((flacStream*)client)->format = metadata->data.stream_info.bits_per_sample;
        ((flacStream*)client)->channel = metadata->data.stream_info.channels;
  		((flacStream*)client)->samplerate = metadata->data.stream_info.sample_rate;	
        ((flacStream*)client)->blockAlign = metadata->data.stream_info.channels * metadata->data.stream_info.bits_per_sample/8;
   	}
   	else if(metadata->type == FLAC__METADATA_TYPE_VORBIS_COMMENT)
   	{
   	}             
}
    
void flacStream::ErrorCallback(const FLAC__StreamDecoder*,FLAC__StreamDecoderErrorStatus,void*)
{
}

FLAC__StreamDecoderReadStatus flacStream::ReadCallback(const FLAC__StreamDecoder* flac,FLAC__byte buffer[],size_t* bytes,void* client)
{
    if(*bytes > 0)
    {
        *bytes = stream_read_(strm,buffer,1,*bytes);
        if(*bytes == 0) 
            return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM; 
        else 
            return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
    }
    else
        return FLAC__STREAM_DECODER_READ_STATUS_ABORT;  
}

FLAC__StreamDecoderSeekStatus flacStream::SeekCallback(const FLAC__StreamDecoder*,FLAC__uint64 offset,void* client)
{
    int32_t ret = stream_seek_(strm,offset,SEEK_SET); 
    if(ret == TRUE)
        return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
    return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;  
}

FLAC__StreamDecoderTellStatus flacStream::TellCallback(const FLAC__StreamDecoder*,FLAC__uint64* offset,void *client)
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

FLAC__StreamDecoderLengthStatus flacStream::LengthCallback(const FLAC__StreamDecoder* flac,FLAC__uint64 *stream_length,void* client)
{
    int32_t current = stream_tell_(strm); 
    if(current<0)
        return FLAC__STREAM_DECODER_LENGTH_STATUS_ERROR;
    stream_seek_(strm,0,SEEK_END);
    int32_t total = stream_tell_(strm);    
    stream_seek_(strm,current,SEEK_SET);
    *stream_length = total; 
    return FLAC__STREAM_DECODER_LENGTH_STATUS_OK; 
}

FLAC__bool flacStream::EofCallback(const FLAC__StreamDecoder*,void *client)
{
    int32_t current = stream_tell_(strm); 
    stream_seek_(strm,0,SEEK_END);
    int32_t total = stream_tell_(strm);     
    stream_seek_(strm,current,SEEK_SET);
    if(current == total)
        return true;
    return false; 
}

int32_t flacStream::GetData(uint8_t *data,int32_t bytes)
{
    outBytes = data;
    outLen = 0;
    outMax = bytes;

    if(initialData.size() > 0)
    {
        size_t rem = std::min(initialData.size(), (size_t)bytes);
        memcpy(data, &initialData[0], rem);
        outLen += rem;
        initialData.erase(initialData.begin(), initialData.begin()+rem);
    }

    while(outLen < outMax)
    {
        if(FLAC__stream_decoder_process_single(flacFile) == false ||
            FLAC__stream_decoder_get_state(flacFile) == FLAC__STREAM_DECODER_END_OF_STREAM)
        break;
    }

    return outLen;
}
    
bool flacStream::Rewind()
{
    if(FLAC__stream_decoder_seek_absolute(flacFile, 0) != false)
    {
        initialData.clear();
        return true;
    }
    return false;
}

bool flacStream::init(int32_t *fmt,int32_t *frequency,int32_t* ch,int32_t *blockalign)
{
    flacFile = FLAC__stream_decoder_new();
    if(flacFile)
    {   
        FLAC__stream_decoder_set_md5_checking(flacFile,false);
        FLAC__stream_decoder_set_metadata_ignore_all(flacFile);
        FLAC__stream_decoder_set_metadata_respond(flacFile,FLAC__METADATA_TYPE_STREAMINFO);
        FLAC__stream_decoder_set_metadata_respond(flacFile,FLAC__METADATA_TYPE_VORBIS_COMMENT);  
        if(FLAC__stream_decoder_init_stream(flacFile,ReadCallback,SeekCallback,TellCallback,LengthCallback, EofCallback, WriteCallback, MetadataCallback, ErrorCallback, this) == FLAC__STREAM_DECODER_INIT_STATUS_OK)
        {   
            if(!FLAC__stream_decoder_process_until_end_of_metadata(flacFile)) 
            {
                FLAC__stream_decoder_finish(flacFile);
                FLAC__stream_decoder_delete(flacFile);
                flacFile = 0;
                return false;
            }     
                    
            outBytes = NULL;
            outMax = 0;
            outLen = 0;
        
            if(!FLAC__stream_decoder_process_single(flacFile)) 
            {
                FLAC__stream_decoder_finish(flacFile);
                FLAC__stream_decoder_delete(flacFile);
                flacFile = 0;
                return false;
            }     

            *fmt = format;
            *frequency = samplerate;
            *blockalign = blockAlign;
            *ch = this->channel;
            return true;
        }
        FLAC__stream_decoder_delete(flacFile);
        flacFile = NULL;
    } 
    return false;
}    
    
flacStream::~flacStream()
{
    if(flacFile)
    {
        FLAC__stream_decoder_finish(flacFile);
        FLAC__stream_decoder_delete(flacFile);
        flacFile = NULL;
    }
}

