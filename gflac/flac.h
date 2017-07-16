#ifndef FLAC_DECODER_H
#define FLAC_DECODER_H
#include <vector>
#include <FLAC/stream_decoder.h>
#include "../gaudio/gdecreator.h"

class flac 
{
public:
    flac(gdecoder* dec)
    {
        decoder = dec; 
        flc = 0; 
        length = 0;
        pos = 0;
        channel = 2;
        format = 16;
        samplerate = 44100;
        samples = 0;  
        align = 4;  
        out = 0;
        outmax = outlen = 0;         
    }      
    ~flac();
public:
    bool init(int32_t& channel,int32_t& format,int32_t& samplerate);
    void reset();
    int32_t position();
    int32_t read(uint8_t* buffer,int32_t length);
    void seek(int32_t position);
    int32_t total();  
private:
    FLAC__StreamDecoderWriteStatus write(const FLAC__Frame* frame,const FLAC__int32* const buffer[]);

    static FLAC__StreamDecoderReadStatus read_callback(const FLAC__StreamDecoder* decoder,FLAC__byte buffer[],uint32_t* bytes,void* client);
    static FLAC__StreamDecoderSeekStatus seek_callback(const FLAC__StreamDecoder* decoder,FLAC__uint64 offset,void* client);
    static FLAC__StreamDecoderTellStatus tell_callback(const FLAC__StreamDecoder* decoder,FLAC__uint64* offset,void* client);
    static FLAC__StreamDecoderLengthStatus length_callback(const FLAC__StreamDecoder* decoder,FLAC__uint64* length,void* client);
    static FLAC__bool eof_callback(const FLAC__StreamDecoder* decoder,void* client);
    static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder* decoder,const FLAC__Frame* frame,const FLAC__int32* const buffer[],void* client);
    static void metadata_callback(const FLAC__StreamDecoder* decoder,const FLAC__StreamMetadata* metadata,void* client);
    static void error_callback(const FLAC__StreamDecoder* decoder,FLAC__StreamDecoderErrorStatus status,void* client);
private:
    gdecoder* decoder;
    FLAC__StreamDecoder* flc;
    //queueBuffer quque;  
    //sizedBuffer backqueue;
    int32_t length;
    int32_t pos; 
    int32_t channel;
    int32_t format;
    int32_t samplerate;
    int32_t samples;
    int32_t align; 
    std::vector<uint8_t> buffer;
    uint8_t *out;
    int32_t outmax;
    int32_t outlen;        
};

#endif
