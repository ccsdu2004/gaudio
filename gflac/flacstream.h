#ifndef FLAC_STREAM_H
#define FLAC_STREAM_H
#include <vector>
#include <FLAC/stream_decoder.h>
#include "../gaudio/gdecreator.h"
#include "../gaudio/streamBuffer.h"

struct flacStream
{
private:
    gdecoder* decoder;    
    FLAC__StreamDecoder *flacFile;
    int32_t format;
    int32_t samplerate;
    int32_t blockAlign;
    int32_t channel;

    std::vector<uint8_t> initialData;

    uint8_t *outBytes;
    int32_t outMax;
    int32_t outLen;
public:
    int32_t GetData(uint8_t *data,int32_t bytes);
    bool Rewind();
    bool init(int32_t *fmt,int32_t *frequency,int32_t* channel,int32_t *blockalign);
    
    uint64_t GetLength()
    {
        return FLAC__stream_decoder_get_total_samples(flacFile);
    }
    
    void Seek(FLAC__uint64 position)
    {
        FLAC__stream_decoder_seek_absolute(flacFile,position); 
    }
    
    uint64_t GetCurrent()
    {   
        FLAC__uint64 position = 0;
        FLAC__stream_decoder_get_decode_position(flacFile,&position);
        return position;
    }

    flacStream(gdecoder* dec): 
        decoder(dec),
        flacFile(NULL), 
        format(0), 
        samplerate(0),
        blockAlign(0)
    {
    }

    ~flacStream();
private:
    template<int32_t shift,int32_t offset,typename T>
    static void CopySamples(T *data, const FLAC__int32 *const buffer[],int32_t off,int32_t todo,int32_t channels)
    {
        for(int32_t i = 0;i < todo;i++)
        {
            for(int32_t c = 0;c < channels;c++)
                *(data++) = (buffer[c][off+i]>>shift) + offset;
        }
    }

    template<int32_t bits>
    static void CopySamplesFloat(float *data,const FLAC__int32 *const buffer[],int32_t off,int32_t todo,int32_t channels)
    {
        for(int32_t i = 0;i < todo;i++)
        {
            for(int32_t c = 0;c < channels;c++)
                *(data++) = buffer[c][off+i] * (1./((1u<<(bits-1))-1));
        }
    }

    static FLAC__StreamDecoderWriteStatus WriteCallback(const FLAC__StreamDecoder*,const FLAC__Frame*,const FLAC__int32* const[],void*);
    static void MetadataCallback(const FLAC__StreamDecoder*,const FLAC__StreamMetadata* metadata,void*);
    static void ErrorCallback(const FLAC__StreamDecoder*,FLAC__StreamDecoderErrorStatus,void*);
    static FLAC__StreamDecoderReadStatus ReadCallback(const FLAC__StreamDecoder*,FLAC__byte[],size_t*,void*);
    static FLAC__StreamDecoderSeekStatus SeekCallback(const FLAC__StreamDecoder*,FLAC__uint64,void*);
    static FLAC__StreamDecoderTellStatus TellCallback(const FLAC__StreamDecoder*,FLAC__uint64*,void*);
    static FLAC__StreamDecoderLengthStatus LengthCallback(const FLAC__StreamDecoder*,FLAC__uint64*,void*);
    static FLAC__bool EofCallback(const FLAC__StreamDecoder*,void *client);
};

#endif
//! by duwenhua10123494
