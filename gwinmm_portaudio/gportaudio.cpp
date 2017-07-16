#include <cstdio>
#include <cstdlib>
#include <portaudio/portaudio.h>
#include "gaudio.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/gdriver.h"
#include "../gaudio/gmain.h"

#define BUFFER_LEN      4096

const char* driver_maker(geffect*)
{
    return "http://www.geek-audio.org/";
}

struct gdriverimpl
{
    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError   err;

    gdriverimpl(int32_t bits,int32_t channel,int32_t samplerate)
    {
        int format = bits;
        switch(bits)
        {
        case   8:
            format = paInt8;
            break;
        case  16:
            format = paInt16;
            break;
        case  24:
            format = paInt24;
            break;
        case  32:
            format = paInt32;
            break;
        default:
            format = paInt16;
            break;
        }

        outputParameters.device = Pa_GetDefaultOutputDevice();
        outputParameters.channelCount = channel;
        outputParameters.sampleFormat = format;
        outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
        outputParameters.hostApiSpecificStreamInfo = NULL;
        err = Pa_OpenStream(&stream,NULL,&outputParameters,samplerate,BUFFER_LEN,paClipOff,NULL,NULL);
    }
    ~gdriverimpl()
    {
    }
};

void GAPIENTRY driver_play(gdriver* driver,char* buffer,uint32_t length)
{
    Pa_WriteStream(driver->impl->stream,buffer,length/4);
}

void GAPIENTRY driver_close(gdriver* driver)
{
    Pa_CloseStream(driver->impl->stream);
}

void GAPIENTRY driver_destroy(gdriver* driver)
{
    delete driver->impl;
    delete driver;
    driver = 0;
    Pa_Initialize();
}

gdriver* GAPIENTRY driver_create(int32_t format,int32_t channel,int32_t samplerate)
{
    PaError err = Pa_Initialize();
    if(err != paNoError)
        return NULL;

    gdriver* driver = new gdriver;
    driver->id = AUDIO_DRIVER_PORTAUDIO;
    driver->play = driver_play;
    driver->destroy = driver_destroy;
    driver->capture_start = 0;
    driver->capture_stop = 0;
    driver->close = driver_close;
    driver->impl = new gdriverimpl(format,channel,samplerate);
    if(driver->impl->err != paNoError)
    {
        driver_destroy(driver);
        return NULL;
    }
    Pa_StartStream(driver->impl->stream);
    return driver;
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_DRIVER;
    addon.addon = (void*)driver_create;
    return &addon;
}




