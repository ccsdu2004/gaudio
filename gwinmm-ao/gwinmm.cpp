#include <cstdio>
#include <cstdlib>
#include <ao/ao.h>
#include "gaudio.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/gdriver.h"
#include "../gaudio/gmain.h"
 
const char* driver_maker(geffect*)
{
    return "http://www.geek-audio.org/";        
}

struct gdriverimpl
{   
    ao_device* handler;
    ao_sample_format format;        
    
    gdriverimpl(int32_t bits,int32_t channel,int32_t samplerate)
    {   
        memset(&format,0,sizeof(format));
        format.bits = bits;
        format.channels = channel;
        format.rate = samplerate;
        format.byte_format = AO_FMT_LITTLE;
        ao_initialize();
        int default_driver = ao_default_driver_id();
        handler = ao_open_live(default_driver,&format,NULL);                
    }
    ~gdriverimpl()
    {   
        ao_shutdown();
        handler = NULL;        
    }    
};

void GAPIENTRY driver_play(gdriver* driver,char* buffer,uint32_t length)
{
    ao_device* handler = driver->impl->handler; 
    ao_play(handler,buffer,length);    
}

void GAPIENTRY driver_close(gdriver* driver)
{ 
    ao_close(driver->impl->handler);
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
    driver->id = AUDIO_DRIVER_WINMM;
    driver->play = driver_play;
    driver->destroy = driver_destroy;
    driver->capture_start = 0;
    driver->capture_stop = 0;
    driver->close = driver_close;
    driver->impl = new gdriverimpl(format,channel,samplerate);
    return driver;          
}

static gaddons addon;

#if defined(__cplusplus)
extern "C"
{
#endif

gaddons* GAPIENTRY ginstance(void* ctx)
{             
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_DRIVER;
    addon.addon = (void*)driver_create;
    return &addon;
}

#if defined(__cplusplus)
}
#endif





