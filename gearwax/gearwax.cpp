#include <stdio.h>
#include <stdlib.h>
#include "gaddons.h"
#include "geffect.h"
#include "gmath.h"

/**
 * @file
 * Stereo Widening Effect. Adds audio cues to move stereo image in
 * front of the listener. Adapted from the libsox earwax effect.
 */

#define NUMTAPS 64

static const int8_t filt[NUMTAPS] =
{
/* 30бу  330бу */
    4,   -6,     /* 32 tap stereo FIR filter. */
    4,  -11,     /* One side filters as if the */
   -1,   -5,     /* signal was from 30 degrees */
    3,    3,     /* from the ear, the other as */
   -2,    5,     /* if 330 degrees. */
   -5,    0,
    9,    1,
    6,    3,     /*                         Input                         */
   -4,   -1,     /*                   Left         Right                  */
   -5,   -3,     /*                __________   __________                */
   -2,   -5,     /*               |          | |          |               */
   -7,    1,     /*           .---|  Hh,0(f) | |  Hh,0(f) |---.           */
    6,   -7,     /*          /    |__________| |__________|    \          */
   30,  -29,     /*         /                \ /                \         */
   12,   -3,     /*        /                  X                  \        */
  -11,    4,     /*       /                  / \                  \       */
   -3,    7,     /*  ____V_____   __________V   V__________   _____V____  */
  -20,   23,     /* |          | |          |   |          | |          | */
    2,    0,     /* | Hh,30(f) | | Hh,330(f)|   | Hh,330(f)| | Hh,30(f) | */
    1,   -6,     /* |__________| |__________|   |__________| |__________| */
  -14,   -5,     /*      \     ___      /           \      ___     /      */
   15,  -18,     /*       \   /   \    /    _____    \    /   \   /       */
    6,    7,     /*        `->| + |<--'    /     \    `-->| + |<-'        */
   15,  -10,     /*           \___/      _/       \_      \___/           */
  -14,   22,     /*               \     / \       / \     /               */
   -7,   -2,     /*                `--->| |       | |<---'                */
   -4,    9,     /*                     \_/       \_/                     */
    6,  -12,     /*                                                       */
    6,   -6,     /*                       Headphones                      */
    0,  -11,
    0,   -5,
    4,    0};

typedef struct
{
    int16_t taps[NUMTAPS * 2];
}earwaxContext;

geffect* geffect_create();

gecreator(earwax)

int32_t gearwaxeffectcreator::is_support(int32_t efx,int32_t mask)
{
    if(efx == AUDIO_EFFECT_STEREO_WIDENING &&
       AUDIO_CHANNEL_CUR == 2)
       return TRUE;
    return FALSE;
}

geffect* gearwaxeffectcreator::create_by_id(int32_t efx,int32_t mask)
{
    if(efx == AUDIO_EFFECT_STEREO_WIDENING && AUDIO_CHANNEL_CUR == 2)
       return geffect_create();
    return NULL;
}

void gearwaxeffectcreator::destroy()
{
    delete this;
}

struct geffectimpl
{
public:
    geffectimpl();
    ~geffectimpl();
public:
    void  update();
    earwaxContext filter[AUDIO_CHANNEL_CUR];
};

geffectimpl::geffectimpl()
{
}

geffectimpl::~geffectimpl()
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

#define SOX_INT_MIN(bits) (1 <<((bits)-1))
#define SOX_INT_MAX(bits) (((unsigned)-1)>>(33-(bits)))

static inline int16_t* scalarproduct(const int16_t *in, const int16_t *endin, int16_t *out)
{
    int32_t sample;
    int16_t j;

    while (in < endin) {
        sample = 0;
        for (j = 0; j < NUMTAPS; j++)
            sample += in[j] * filt[j];
        *out = av_clip_int16(sample >> 6);
        out++;
        in++;
    }

    return out;
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






