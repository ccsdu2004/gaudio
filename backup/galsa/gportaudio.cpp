#include <cstdio>
#include <cstdlib>
#ifdef __linux
    #include<unistd.h>
#endif
#include "gaudio.h"
#include "header.h"
#include "portaudio.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/gdriver.h"
#include "../gaudio/gexternal.h"

char* driver_maker(geffect*)
{
    return "duwenhua2013";
}

struct gdriverimpl
{
    audio_output_t ao;

    gdriverimpl(int32_t bits,int32_t channel,int32_t samplerate)
    {
        ao.channels = channel;
        ao.format = 16;
        ao.rate = samplerate;
    }
    ~gdriverimpl()
    {
    }
};

void GAPIENTRY driver_play(gdriver* driver,char* buffer,uint32_t length)
{
    audio_output_t* ao = &driver->impl->ao;
    ao->write(ao,(unsigned char*)buffer,length);
}

void GAPIENTRY driver_close(gdriver* driver)
{
    audio_output_t* ao = &driver->impl->ao;
    ao->close(ao);
    ao->deinit(ao);
}

void GAPIENTRY driver_destroy(gdriver* driver)
{
    delete driver->impl;
    delete driver;
    driver = 0;
}

gdriver* GAPIENTRY driver_create(int32_t format,int32_t channel,int32_t samplerate)
{
    gdriver* driver = new gdriver;
    driver->id = AUDIO_DRIVER_PORTAUDIO;
    driver->play = driver_play;
    driver->destroy = driver_destroy;
    driver->capture_start = 0;
    driver->capture_stop = 0;
    driver->close = driver_close;
    driver->impl = new gdriverimpl(format,channel,samplerate);
    int ret = init_portaudio(&driver->impl->ao);
    if(ret != 0)
    {
        driver_destroy(driver);
        return 0;
    }
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




