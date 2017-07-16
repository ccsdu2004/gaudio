#ifndef G_CONTEXT_H
#define G_CONTEXT_H
#include <list>
#include "gaudio.h"
#include "gdriver.h"
#include "geffect.h"
#include "gdecoder.h"
#include "gdecreator.h"
#include "gcatcher.h"
#include "gwriter.h"
#include "gsource.h"
#include "gthread.h"
#include "gmain.h"
#include "glistener.h"

#ifndef MAX_PATH
    #define MAX_PATH 256
#endif

struct gfactory
{
    gdecoderfactory decoderfactory;
    geffectfactory effectfactory;
    gcatcherfactory catcherfactory;
    gwritercontext writerfactory;
};

struct gdevice
{
    gdevice();
    ~gdevice();
    glistener listener;
    int32_t  mode;
    float    gain;
    float    pan;
    float    soundspeed;
    int32_t  mute;

    int32_t  attenuation;
    float    dopplerfactor;
    float    dopplervelocity;

    std::list<geffect*> effect;
    std::list<gsource*> source;
    gdatabuffer<float> frontbuffer;
    gbuffer<int16_t> backbuffer;
    void applyeffect();
    int32_t swap;
};

struct gcontext
{
    gcontext();
    ~gcontext();
    int32_t killed;
    int32_t lasterror;
    struct bs2b* bs2bptr;

    gdriver* driver;
    gdriver_create driver_create;

    gcatcher* recorder;
    gfactory factory;

    gdevice device;

    void*  thread;
    //gmutex locker;
};

extern gcontext scontext;

void GAPIENTRY gexitcallback();
//gcontext* GAPIENTRY getCurrentContext();

#endif
//! by duwenhua2013
