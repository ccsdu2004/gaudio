#include "gdecoder.h"

#define COPY(type)\
   type buffer[BUFFER_SIZE];\
   uint32_t size = stream_read_((streamBuffer*)decoder->stream,buffer,sizeof(type),length);\
   for(int32_t i=0;i<size;i++)\
      pcm[i] = gtofloat(buffer[i]);\
   return size;

uint32_t GAPIENTRY decoder_linear_read(gdecoder* decoder,float* pcm,int32_t length)
{
    int32_t size = 0;
    if(decoder->impl->format == 8)
    {
        COPY(uint8_t);
    }
    else if(decoder->impl->format == 16)
    {
        COPY(int16_t);
    }
    else if(decoder->impl->format == 24)
    {
        COPY(gint24);
    }
    else if(decoder->impl->format == 32)
    {
        COPY(int32_t);
    }
    decoder->impl->current = stream_tell((streamBuffer*)decoder->stream);
    if(decoder->impl->format == 4)
        size *= 2;
    return size;
}
