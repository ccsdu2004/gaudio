#include <stdio.h>
#include <stdlib.h>
#include "gaddons.h"
#include "geffect.h"
#include "gmath.h"

geffect* geffect_create();

gecreator(flanger)

int32_t gflangereffectcreator::is_support(int32_t efx,int32_t mask)
{
    if(efx == AUDIO_EFFECT_FLANGER)
       return TRUE;
    return FALSE;
}

geffect* gflangereffectcreator::create_by_id(int32_t efx,int32_t mask)
{
    if(efx == AUDIO_EFFECT_FLANGER)
       return geffect_create();
    return NULL;
}

void gflangereffectcreator::destroy()
{
    delete this;
}

struct geffectimpl
{
    float* buffer[2];
    uint32_t length;
    uint32_t offset;
    uint32_t lforange;
    uint32_t lfoscale;
    int32_t lfodisp;
    float gain[2][AUDIO_CHANNEL_MAX];
    int32_t waveform;
    int32_t delay;
    float depth;
    float feedback;

    int32_t needupdate;
};

void effect_init(geffectimpl* impl)
{
    impl->length = 0;
    impl->buffer[0] = NULL;
    impl->buffer[1] = NULL;
    impl->offset = 0;
    impl->lforange = 1;
    impl->waveform = AUDIO_EFFECT_FLANGER_WAVEFORM_TRIANGLE;
    impl->delay = AUDIO_EFFECT_FLANGER_DEFAULT_DELAY * 44100;
    impl->depth = AUDIO_EFFECT_FLANGER_DEFAULT_DEPTH;
    impl->feedback = AUDIO_EFFECT_FLANGER_DEFAULT_FEEDBACK;
    impl->needupdate = 0;
}

const char* effect_maker(geffect*)
{
    return "http://www.geek-audio.org/";
}

void effect_destroy(geffect* effect)
{
    free(effect->impl->buffer[0]);
    effect->impl->buffer[0] = NULL;
    effect->impl->buffer[1] = NULL;
    delete effect->impl;
    effect->impl = 0;
    effect->destroy(effect);
}

static inline void triangle(int32_t* delayleft,int32_t* delayright,uint32_t offset,const geffect* effect)
{
    geffectimpl* state = effect->impl;

    float lfovalue;

    lfovalue = 2.0f - fabsf(2.0f - state->lfoscale*(offset%state->lforange));
    lfovalue *= state->depth * state->delay;
    *delayleft = fastf2i(lfovalue) + state->delay;

    offset += state->lfodisp;
    lfovalue = 2.0f - fabsf(2.0f - state->lfoscale*(offset%state->lforange));
    lfovalue *= state->depth * state->delay;
    *delayright = fastf2i(lfovalue) + state->delay;
}

static inline void sinusoid(int32_t* delayleft,int32_t* delayright,uint32_t offset,const geffect* effect)
{
    geffectimpl* state = effect->impl;
    float lfovalue;

    lfovalue = 1.0f + sinf(state->lfoscale*(offset%state->lforange));
    lfovalue *= state->depth * state->delay;
    *delayleft = fastf2i(lfovalue) + state->delay;

    offset += state->lfodisp;
    lfovalue = 1.0f + sinf(state->lfoscale*(offset%state->lforange));
    lfovalue *= state->depth * state->delay;
    *delayright = fastf2i(lfovalue) + state->delay;
}

#define DECL_TEMPLATE(func)                                                   \
static void process##func(geffect* effect,const uint32_t length,              \
const float* left,const float* right,float (*out)[2])                         \
{                                                                             \
    geffectimpl* state = effect->impl;                                        \
    const uint32_t bufmask = state->length-1;                           \
    float *leftbuf = state->buffer[0];                                  \
    float *rightbuf = state->buffer[1];                                 \
    uint32_t offset = state->offset;                                          \
    const float feedback = state->feedback;                                   \
    uint32_t it;                                                              \
    for(it = 0;it < length;it++)                                              \
    {                                                                         \
        int32_t delayleft,delayright;                                      \
        func(&delayleft,&delayright,offset,effect);                       \
                                                                              \
        out[it][0] = leftbuf[(offset-delayleft)&bufmask];                    \
        leftbuf[offset&bufmask] = (out[it][0]+left[it]) * feedback;           \
                                                                              \
        out[it][1] = rightbuf[(offset-delayright)&bufmask];                  \
        rightbuf[offset&bufmask] = (out[it][1]+right[it]) * feedback;         \
                                                                              \
        offset++;                                                             \
    }                                                                         \
    state->offset = offset;                                                   \
}

DECL_TEMPLATE(triangle)
DECL_TEMPLATE(sinusoid)

#undef DECL_TEMPLATE

void GAPIENTRY effect_process(geffect* effect,gdatabuffer<float>& buffer)
{
    uint32_t it,kt;
    uint32_t base;

    geffectimpl* state = effect->impl;

    for(base = 0;base < buffer.use;)
    {
        float temps[64][2];
        uint32_t td = gmin<uint32_t>(buffer.use-base,64);

        switch(state->waveform)
        {
            case AUDIO_EFFECT_FLANGER_WAVEFORM_SINUSOID:
                processtriangle(effect,td,buffer.data[0]+base,buffer.data[1]+base,temps);
                break;
            case AUDIO_EFFECT_FLANGER_WAVEFORM_TRIANGLE:
                processsinusoid(effect,td,buffer.data[0]+base,buffer.data[1]+base,temps);
                break;
        }

        for(kt = 0;kt < AUDIO_CHANNEL_CUR;kt++)
        {
            float gain = state->gain[0][kt];
            if(gain > GAIN_SILENCE_THRESHOLD)
            {
                for(it = 0;it < td;it++)
                    buffer.data[kt][it+base] += temps[it][0] * gain;
            }

            gain = state->gain[1][kt];
            if(gain > GAIN_SILENCE_THRESHOLD)
            {
                for(it = 0;it < td;it++)
                    buffer.data[kt][it+base] += temps[it][1] * gain;
            }
        }

        base += td;
    }
}

void GAPIENTRY effect_set_int32(geffect* effect,int32_t flag,int32_t value)
{
    geffectimpl* impl = effect->impl;
    if(flag == AUDIO_EFFECT_FLANGER_WAVEFORM)
    {
        if(value == AUDIO_EFFECT_FLANGER_WAVEFORM_SINUSOID ||
           value == AUDIO_EFFECT_FLANGER_WAVEFORM_TRIANGLE)
        {
            impl->waveform = value;
            impl->needupdate = 1;
        }
    }
}

void GAPIENTRY effect_get_int32(geffect* effect,int32_t flag,int32_t* value)
{

}

void GAPIENTRY effect_set_float(geffect* effect,int32_t flag,float value)
{
    geffectimpl* echo = effect->impl;
    switch(flag)
    {
    case AUDIO_EFFECT_ECHO_DELAY:
         //echo->set_delay(gclamp(value,AUDIO_EFFECT_ECHO_DELAY_MIN,AUDIO_EFFECT_ECHO_DELAY_MAX));
         //effect->update = TRUE;
         break;
    case AUDIO_EFFECT_ECHO_DECAY:
         //echo->set_decay(gclamp(value,AUDIO_EFFECT_ECHO_DECAY_MIN,AUDIO_EFFECT_ECHO_DECAY_MAX));
         break;
    default:
         return;
    }
}

void GAPIENTRY effect_get_float(geffect* effect,int32_t flag,float* value)
{
    geffectimpl* echo = effect->impl;
    switch(flag)
    {
    case AUDIO_EFFECT_ECHO_DELAY:
         //*value = echo->get_delay();
         break;
    case AUDIO_EFFECT_ECHO_DECAY:
         //*value = echo->get_decay();
         break;
    default:
         return;
    }
}

geffect* geffect_create()
{
    geffect* effect = new geffect();
    effect->mask = EFFECT_SUPPORT_BIND_TO_GLOBAL | EFFECT_SUPPORT_BIND_TO_SOURCE;
    effect->binded = FALSE;
    effect->fftcb = 0;
    effect->process = effect_process;
    effect->order = -1;
    effect->source = 0;
    effect->destroy = effect_destroy;
    effect->setfloat = effect_set_float;
    effect->getfloat = effect_get_float;
    effect->setint = effect_set_int32;
    effect->getint = effect_get_int32;
    effect->state = TRUE;

    geffectimpl* filter = new geffectimpl();
    effect_init(filter);
    effect->impl = filter;
    return effect;
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new gflangereffectcreator();
    return &addon;
}






