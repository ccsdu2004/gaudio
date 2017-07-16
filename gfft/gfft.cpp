#include <stdio.h>
#include <stdlib.h>
#include "gaudio.h"
#include "fftimpl.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmath.h"
#include "Spectrum.h"

#define current_channel

geffect* geffect_create();

gecreator(fft)

int32_t gffteffectcreator::is_support(int32_t id,int32_t mask)
{
    if(id == AUDIO_EFFECT_FFT)
       return TRUE;
    return FALSE;
}

struct geffectimpl
{
    geffectimpl():
        capture_size(AUDIO_EFFECT_FFT_CAPTURE_SIZE_VALUE_DEFAULT),
        capture_channel(AUDIO_EFFECT_FFT_CHHANEL_VALUE_AVG),
        update(false){}
    int32_t capture_size;
    int32_t capture_channel;
    bool update;
    float data[AUDIO_EFFECT_FFT_CAPTURE_SIZE_VALUE_MAX];
};

geffect* gffteffectcreator::create_by_id(int32_t id,int32_t mask)
{
    if(id == AUDIO_EFFECT_FFT)
        return geffect_create();
    return NULL;
}

void gffteffectcreator::destroy()
{
    delete this;
}
/*
char* effect_maker(geffect*)
{
    return "duwenhua2013";
}*/

void effect_destroy(geffect* effect)
{
    delete effect->impl;
    effect->destroy(effect);
}

void GAPIENTRY effect_process(geffect* effect,gdatabuffer<float>& buffer)
{
    if(buffer.use <= 0)
        return;
    int32_t capture_size = effect->impl->capture_size;
    int32_t capture_channel = effect->impl->capture_channel;
    float* in = new float[buffer.use];
    for(int32_t i=0;i<buffer.use;i++)
    {
        in[i] = capture_channel == AUDIO_EFFECT_FFT_CHHANEL_VALUE_LEFT ? buffer.data[0][i] :
            AUDIO_EFFECT_FFT_CHHANEL_VALUE_RIGHT ? buffer.data[1][i] :
                (buffer.data[0][i]+buffer.data[1][i])*0.5f;
    }

    ComputeSpectrum(in,buffer.use,capture_size,44100.0f,effect->impl->data,true,3);
    if(effect->fftcb)
        effect->fftcb(effect,effect->impl->data,capture_size,0,0);
}

void GAPIENTRY effect_set_int32(geffect* effect,int32_t flag,int32_t value)
{
    if(flag == AUDIO_EFFECT_FFT_CAPTURE_SIZE_VALUE)
    {
        effect->impl->capture_size = gclamp(value,AUDIO_EFFECT_FFT_CAPTURE_SIZE_VALUE_MIN,AUDIO_EFFECT_FFT_CAPTURE_SIZE_VALUE_MAX);
        effect->impl->update = true;
    }
    else if(flag ==  AUDIO_EFFECT_FFT_CHHANEL_VALUE)
    {
        effect->impl->capture_channel = gclamp(value,AUDIO_EFFECT_FFT_CHHANEL_VALUE_LEFT,AUDIO_EFFECT_FFT_CHHANEL_VALUE_AVG);
        effect->impl->update = true;
    }
}

void GAPIENTRY effect_get_int32(geffect* effect,int32_t flag,int32_t* value)
{
    if(flag == AUDIO_EFFECT_FFT_CAPTURE_SIZE_VALUE)
        *value = effect->impl->capture_size;
    else if(flag == AUDIO_EFFECT_FFT_CHHANEL_VALUE)
        *value = effect->impl->capture_channel;
}

geffect* geffect_create()
{
    geffect* effect = new geffect();
    effect->mask = EFFECT_SUPPORT_BIND_TO_GLOBAL | EFFECT_SUPPORT_BIND_TO_SOURCE;
    effect->binded = FALSE;
    effect->fftcb = 0;
    effect->pcmcb = 0;
	effect->process = effect_process;
    effect->order = -1;
    effect->source = 0;
    effect->destroy = effect_destroy;
    effect->setfloat = 0;
    effect->getfloat = 0;
    effect->setint = effect_set_int32;
    effect->getint = effect_get_int32;
    effect->state = TRUE;
    effect->curid = -1;
    effect->impl = new geffectimpl();
    return effect;
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new gffteffectcreator();
    return &addon;
}






