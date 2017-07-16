#include <stdio.h>
#include <stdlib.h>
#include "gaddons.h"
#include "geffect.h"
#include "gmath.h"

/*
* 2-pole filters designed by Robert Bristow-Johnson <rbj@audioimagination.com>
 *   see http://www.musicdsp.org/files/Audio-EQ-Cookbook.txt
 *
 * 1-pole filters based on code (c) 2000 Chris Bagwell <cbagwell@sprynet.com>
 *   Algorithms: Recursive single pole low/high pass filter
 *   Reference: The Scientist and Engineer's Guide to Digital Signal Processing
 *
 *   low-pass: output[N] = input[N] * A + output[N-1] * B
 *     X = exp(-2.0 * pi * Fc)
 *     A = 1 - X
 *     B = X
 *     Fc = cutoff freq / sample rate
 *
 *     Mimics an RC low-pass filter:
 *
 *     ---/\/\/\/\----------->
 *                   |
 *                  --- C
 *                  ---
 *                   |
 *                   |
 *                   V
 *
 *   high-pass: output[N] = A0 * input[N] + A1 * input[N-1] + B1 * output[N-1]
 *     X  = exp(-2.0 * pi * Fc)
 *     A0 = (1 + X) / 2
 *     A1 = -(1 + X) / 2
 *     B1 = X
 *     Fc = cutoff freq / sample rate
 *
 *     Mimics an RC high-pass filter:
 *
 *         || C
 *     ----||--------->
 *         ||    |
 *               <
 *               > R
 *               <
 *               |
 *               V
 */

geffect* geffect_create();

gecreator(filter)

int32_t gfiltereffectcreator::is_support(int32_t efx,int32_t mask)
{
    if(efx == AUDIO_EFFECT_FILTER_LOWPASS ||
       efx == AUDIO_EFFECT_FILTER_HIGHPASS ||
       efx == AUDIO_EFFECT_FILTER_BANDPASS ||
       efx == AUDIO_EFFECT_FILTER_NOTCH ||
       efx == AUDIO_EFFECT_FILTER_LOW_SHELF ||
       efx == AUDIO_EFFECT_FILTER_HIGH_SHELF)
       return TRUE;
    return FALSE;
}

geffect* gfiltereffectcreator::create_by_id(int32_t efx,int32_t mask)
{
    if(efx == AUDIO_EFFECT_FILTER_LOWPASS ||
       efx == AUDIO_EFFECT_FILTER_HIGHPASS ||
       efx == AUDIO_EFFECT_FILTER_BANDPASS ||
       efx == AUDIO_EFFECT_FILTER_NOTCH ||
       efx == AUDIO_EFFECT_FILTER_LOW_SHELF ||
       efx == AUDIO_EFFECT_FILTER_HIGH_SHELF)
       return geffect_create();
    return NULL;
}

void gfiltereffectcreator::destroy()
{
    delete this;
}

struct gfilterimpl
{
    float a0, a1, a2, a3, a4;
    float x1, x2, y1, y2;
};

struct geffectimpl
{
public:
    geffectimpl();
    ~geffectimpl();
public:
    void  update();
    gfilterimpl filter[AUDIO_CHANNEL_CUR];
};

geffectimpl::geffectimpl()
{
}

geffectimpl::~geffectimpl()
{
}

void geffectimpl::update()
{
}

inline void effect_init(geffectimpl* echo)
{
}

const char* effect_maker(geffect*)
{
    return "http://www.geek-audio.org/";
}

void effect_destroy(geffect* effect)
{
    delete effect->impl;
    effect->impl = 0;
    effect->destroy(effect);
}

inline float dofilter(gfilterimpl* filter,float data)
{
    float result;
    result = filter->a0 * data + filter->a1 * filter->x1 + filter->a2 * filter->x2 -
        filter->a3 * filter->y1 - filter->a4 * filter->y2;

    /* shift x1 to x2, data to x1 */
    filter->x2 = filter->x1;
    filter->x1 = data;

    /* shift y1 to y2, result to y1 */
    filter->y2 = filter->y1;
    filter->y1 = result;
}

void GAPIENTRY effect_process(geffect* effect,gdatabuffer<float>& buffer)
{
    geffectimpl* filter = effect->impl;
    for(int i=0;i<buffer.use;i++)
        for(int c=0;c<AUDIO_CHANNEL_CUR;c++)
            buffer.data[c][i] = dofilter(&filter->filter[c],buffer.data[c][i]);
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
    effect->setint = 0;
    effect->getint = 0;
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
    addon.addon = new gfiltereffectcreator();
    return &addon;
}






