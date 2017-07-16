#include "gaudio.h"
#include "gcontext.h"

gwriter* gaudio_writer_create(int32_t flag,int32_t channel,int32_t samplerate,int32_t format)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return NULL;
    }
    return 0;
}

int32_t gaudio_writer_open(gwriter* writer,const char* filename)
{

}

int32_t gaudio_writer_write(uint8_t* data,int32_t length)
{

}

void gaudio_writer_close(gwriter* writer)
{

}

void gaudio_writer_destroy(gwriter* writer)
{

}


