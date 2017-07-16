#include <malloc.h>
#include <string.h>
#include "wavpack_Encoder.h"

#define INPUT_SAMPLES 4096

int write(FILE* file,void* lpBuffer, uint32_t nNumberOfBytesToWrite, uint32_t *lpNumberOfBytesWritten)
{
    uint32_t bcount;

    *lpNumberOfBytesWritten = 0;

    while (nNumberOfBytesToWrite) 
    {
        bcount = fwrite ((unsigned char*)lpBuffer + *lpNumberOfBytesWritten, 1, nNumberOfBytesToWrite,file);
        if(bcount) 
        {
            *lpNumberOfBytesWritten += bcount;
            nNumberOfBytesToWrite -= bcount;
        }
        else
            break;
    }

    return !ferror (file);
}

int write_block(void* id,void* data,int32_t length)
{
    write_id* wid = (write_id*)id;
    uint32_t bcount;

    if(wid->error)
        return 0;

    if(wid && wid->file && data && length) 
    {
        if(!write(wid->file, data, length, &bcount) || bcount != length) 
        {
            fclose (wid->file);
            wid->file = NULL;
            wid->error = 1;
            return 0;
        }
        else 
        {
            wid->bytes_written += length;
            if(!wid->first_block_size)
                wid->first_block_size = bcount;
        }
    }

    return 1;
}

wavpack_Encoder::wavpack_Encoder()
{
    wpc = NULL;
    sample_buffer = NULL;
}

wavpack_Encoder::~wavpack_Encoder()
{
}

int wavpack_Encoder::open(const char* name,int channels,int rate,int bite)
{   
    close();
    memset(&wv,0,sizeof(wv));
    WavpackConfig config;
    memset(&config,0,sizeof(config));
    config.flags |= CONFIG_HIGH_FLAG;
    config.flags |= CONFIG_HYBRID_FLAG;
    config.bitrate = 1024;
    config.bits_per_sample = bite;
    config.bytes_per_sample = 2;
    config.num_channels = channels;
    config.sample_rate = rate;  
    
    wpc = WavpackOpenFileOutput(write_block,&wv,NULL);
    if(wpc == NULL)
        return 0;
    wv.file = fopen(name,"w+b");
    if(wv.file == NULL)
    {
        close();
        return 0;
    } 
    
    WavpackSetConfiguration(wpc,&config,-1);
    WavpackPackInit(wpc);
    sample_buffer = (int32_t*)malloc(INPUT_SAMPLES*sizeof(int32_t)*WavpackGetNumChannels(wpc));
    return 1;
}
    
int wavpack_Encoder::write(unsigned char* buffer,int len)
{
    int bytes_per_sample = WavpackGetBytesPerSample(wpc)*WavpackGetNumChannels(wpc);
    int count = len/bytes_per_sample;
    if(count) 
    {
        unsigned int cnt = count * WavpackGetNumChannels (wpc);
        unsigned char* sptr = buffer;
        int32_t* dptr = sample_buffer;

        switch(WavpackGetBytesPerSample(wpc)) 
        {
        case 1:
             while(cnt--)
                 *dptr++ = *sptr++ - 128;
                 break;
        case 2:
             while(cnt--) 
             {
                 *dptr++ = sptr [0] | ((int32_t)(signed char) sptr [1] << 8);
                 sptr += 2;
             }
             break;
        case 3:
             while(cnt--) 
             {
                  *dptr++ = sptr [0] | ((int32_t) sptr [1] << 8) | ((int32_t)(signed char) sptr [2] << 16);
                  sptr += 3;
             }
             break;
        }
    }

    if(!WavpackPackSamples(wpc,sample_buffer,count)) 
    {
        //fprintf(stderr, "%s\n", WavpackGetErrorMessage (wpc));
        return 0;
    }
    return 1;    
}

void wavpack_Encoder::close()
{
    if(wpc != NULL)
    {   
        WavpackFlushSamples(wpc);
        WavpackCloseFile(wpc);
        wpc = NULL;
    }
    if(sample_buffer != NULL)
    {
        free(sample_buffer);
        sample_buffer = NULL; 
    }     
}
