#include <stdio.h>
#include <stdlib.h>
#include "gaudio.h"
#include "distortion.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmain.h"
#include "../gaudio/gmath.h"

geffect* geffect_create(int32_t type);

gecreator(distortion)

int32_t gdistortioneffectcreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_DISTORTION || dec == AUDIO_EFFECT_FOLDBACK_DISTORTION)
       return TRUE;
    return FALSE;
}

geffect* gdistortioneffectcreator::create_by_id(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_DISTORTION || dec == AUDIO_EFFECT_FOLDBACK_DISTORTION)
       return geffect_create(dec);
    return NULL;
}

void gdistortioneffectcreator::destroy()
{
    delete this;
}

struct geffectimpl
{
    bool    update;
    int32_t type;
    float   gain;
    float   threshold;
};

void GAPIENTRY effect_set_float(geffect* effect,int32_t flag,float value)
{
    geffectimpl* impl = effect->impl;
    switch(flag)
    {
    case AUDIO_EFFECT_DISTORTION_THRESHOLD:
         impl->threshold =
             gclamp(value,AUDIO_EFFECT_DISTORTION_THRESHOLD_LOW,
                          AUDIO_EFFECT_DISTORTION_THRESHOLD_HIGH);
         impl->update = TRUE;
         break;
    case AUDIO_EFFECT_DISTORTION_GAIN:
         impl->gain =
             gclamp(value,AUDIO_EFFECT_DISTORTION_GAIN_LOW,
                          AUDIO_EFFECT_DISTORTION_GAIN_HIGH);
         impl->update = TRUE;
         break;
    default:
         return;
    }
}

void GAPIENTRY effect_get_float(geffect* effect,int32_t flag,float* value)
{
    geffectimpl* impl = effect->impl;
    switch(flag)
    {
    case AUDIO_EFFECT_DISTORTION_THRESHOLD:
         *value = impl->threshold;
         break;
    case AUDIO_EFFECT_DISTORTION_GAIN:
         *value = impl->gain;
         break;
    default:
         return;
    }
}

inline void effect_init(geffectimpl* impl)
{
    impl->update = false;
    impl->threshold = AUDIO_EFFECT_DISTORTION_THRESHOLD_LOW;
    impl->gain = AUDIO_EFFECT_DISTORTION_GAIN_LOW;
}

inline void effect_reset(geffectimpl* impl)
{
    effect_init(impl);
}

const char* effect_maker(geffect*)
{
    return "duwenhua2013";
}

void effect_destroy(geffect* effect)
{
    delete effect->impl;
    effect->impl = 0;
    delete effect;
    effect = 0;
}

inline float distortion_impl(float k,geffectimpl* impl)
{
    return(impl->gain*(k*(fabsf(k)+impl->threshold )/(k*k +(impl->threshold-1)*fabsf(k) + 1)));
}

inline float distortion_foldback_impl(float k,geffectimpl* impl)
{
    if(k >= impl->threshold || k < -impl->threshold)
        return (fabsf(fabsf(fmodf(k - impl->threshold,impl->threshold*4))-impl->threshold*2 )-impl->threshold)*impl->gain;
    return k * impl->gain;
}

void GAPIENTRY effect_process(geffect* effect,gdatabuffer<float>& buffer)
{
    geffectimpl* impl = effect->impl;
    if(effect->update)
        effect->update = FALSE;

    for(uint32_t i=0;i<buffer.use;i++)
    {
        if(effect->impl->type == AUDIO_EFFECT_FOLDBACK_DISTORTION)
        {
            buffer.data[0][i] = distortion_foldback_impl(buffer.data[0][i],effect->impl);
            buffer.data[1][i] = distortion_foldback_impl(buffer.data[1][i],effect->impl);
        }
        else if(effect->impl->type == AUDIO_EFFECT_DISTORTION)
        {
            buffer.data[0][i] = distortion_impl(buffer.data[0][i],effect->impl);
            buffer.data[1][i] = distortion_impl(buffer.data[1][i],effect->impl);
        }
    }
}

geffect* geffect_create(int32_t type)
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
    effect->setfloat = effect_set_float;
    effect->getfloat = effect_get_float;
    effect->setint = 0;
    effect->getint = 0;
    effect->state = TRUE;

    geffectimpl* impl = new geffectimpl();
    impl->type = type;
    effect_init(impl);
    effect_reset(impl);
    effect->impl = impl;
    return effect;
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new gdistortioneffectcreator();
    return &addon;
}






