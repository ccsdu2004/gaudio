#include "..\internal.h"
#include "lib_Aac.h"

#ifndef MILLION
#define MILLION 1000000
#endif

lib_Aac::lib_Aac()
{
    handle = 0;
    config = 0;
    outBuffer = 0;
    channel = 0;
}

lib_Aac::~lib_Aac()
{
}

int lib_Aac::open(const char* snd,int channels,int rate,int bite)
{
    this->channel = channels;
    handle = faacEncOpen(rate,channels,&inputSamples,&maxOutputBytes);
    if(handle == 0)
        return 0;

    /*int fMicrosecondsPerFrame = (MILLION*inputSamples)/(channels*rate);
    int fMicrosecondsPerByte = (1.0*MILLION)/(rate*channels*sizeof (unsigned short));
    int  fInputSampleBufferBytesDesired = inputSamples*sizeof(short);
    int  fInputSampleBufferSize = 10*fInputSampleBufferBytesDesired;
    //unsigned char* fInputSampleBuffer = new unsigned char[fInputSampleBufferSize];
    int fInputSampleBufferBytesFull = 0;*/

    config = faacEncGetCurrentConfiguration(handle);
    config->inputFormat    = FAAC_INPUT_16BIT;
    config->bandWidth      = 0;//16000;
    config->outputFormat = 0;
    config->quantqual      = 100;
    config->mpegVersion = 0;
    config->aacObjectType = 0;
    if(!faacEncSetConfiguration(handle,config))
    {
        return 0;
    }

	DELETE_BUFFER(outBuffer);
    outBuffer = new unsigned char[maxOutputBytes*2];

    file = fopen(snd,"wb");
    if(file == 0)
         return 0;
    return 1;
}

union data_Holder
{
    unsigned char* uint8_Ptr;
    int32_t*       int32_Ptr;
};

int  lib_Aac::write(unsigned char* buffer,int len)
{
    data_Holder dH;
    dH.uint8_Ptr = buffer;
    int bytes = faacEncEncode(handle,dH.int32_Ptr,len/2,outBuffer,maxOutputBytes*2);
    if(!len && !bytes)
        return 0;
    if(bytes <= 0)
        return 0;
    return fwrite(outBuffer,1,bytes,file);
}

void lib_Aac::close()
{
    if(handle)
        faacEncClose(handle);
    if(file)
    {
        fclose(file);
        file = 0;
    }
    DELETE_BUFFER(outBuffer);
}
