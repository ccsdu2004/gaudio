#include <stdio.h>
#include <stdlib.h>
#include "gaudio.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmath.h"

#define GAIN_SILENCE_THRESHOLD 0.0001f

geffect* geffect_create();

gecreator(compressor)

int32_t gcompressoreffectcreator::is_support(int32_t id,int32_t mask)
{
    if(id == AUDIO_EFFECT_COMPRESSER)
       return TRUE;
    return FALSE;
}

struct geffectimpl
{
    geffectimpl()
    {
        const float attacktime  = AUDIO_STANDARD_SAMPLERATE * 0.2f; /* 200ms Attack */
        const float releasetime = AUDIO_STANDARD_SAMPLERATE * 0.4f; /* 400ms Release */
        attackrate = 1.0f / attacktime;
        releaserate = 1.0f / releasetime;
        for(int i=0;i<AUDIO_CHANNEL_MAX;i++)
            gain[i] = 1.0f;
        gainctrl = 1.0f;
    }

    float gain[AUDIO_CHANNEL_MAX];
    float attackrate;
    float releaserate;
    float gainctrl;
};

geffect* gcompressoreffectcreator::create_by_id(int32_t id,int32_t mask)
{
    if(id == AUDIO_EFFECT_COMPRESSER)
    {
        geffect* efx = geffect_create();
        efx->curid = id;
        return efx;
    }
    return NULL;
}

void gcompressoreffectcreator::destroy()
{
    delete this;
}

const char* effect_maker(geffect*)
{
    return "poplar";
}

void effect_destroy(geffect* effect)
{
    delete effect->impl;
    effect->destroy(effect);
}

void GAPIENTRY effect_process(geffect* effect,gdatabuffer<float>& buffer)
{
    gsource* source = effect->source;
    if(!source)
        return;

    int it,kt;
    uint32_t base;

    for(base = 0;base < buffer.use;)
    {
        float temps[64];
        int td = gmin<float>(buffer.use-base,64);

        if(effect->state)
        {
            float output,smp,amplitude;
            float gain = effect->impl->gainctrl;

            for(it = 0;it < td;it++)
            {
                smp = buffer.data[0][it+base];
                amplitude = fabsf(smp);
                if(amplitude > gain)
                    gain = gmin<float>(gain+effect->impl->attackrate,amplitude);
                else if(amplitude < gain)
                    gain = gmax<float>(gain-effect->impl->releaserate,amplitude);
                output = 1.0f / gclamp<float>(gain, 0.5f, 2.0f);
                temps[it] = smp * output;
            }

            effect->impl->gainctrl = gain;
        }
        else
        {
            float output, smp, amplitude;
            float gain = effect->impl->gainctrl;

            for(it = 0;it < td;it++)
            {
                smp = 0.0f;
                for(int c=0;c<AUDIO_CHANNEL_CUR;c++)
                    smp = buffer.data[c][it+base];
                smp/= AUDIO_CHANNEL_CUR;

                amplitude = 1.0f;
                if(amplitude > gain)
                    gain = gmin<float>(gain+effect->impl->attackrate, amplitude);
                else if(amplitude < gain)
                    gain = gmax<float>(gain-effect->impl->releaserate, amplitude);
                output = 1.0f / gclamp<float>(gain, 0.5f, 2.0f);
                temps[it] = smp * output;
            }

            effect->impl->gainctrl = gain;
        }


        for(kt = 0;kt < AUDIO_CHANNEL_CUR;kt++)
        {
            float gain = effect->impl->gain[kt];
            if(!(gain > GAIN_SILENCE_THRESHOLD))
                continue;

            for(it = 0;it < td;it++)
                buffer.data[kt][base+it] += gain * temps[it];
        }
        base += td;
    }
}

void GAPIENTRY effect_set_float(geffect* effect,int32_t flag,float value)
{
}

void GAPIENTRY effect_get_float(geffect* effect,int32_t flag,float* value)
{
}

geffect* geffect_create()
{
    geffect* effect = new geffect();
    effect->mask = EFFECT_SUPPORT_BIND_TO_SOURCE | EFFECT_SUPPORT_BIND_TO_GLOBAL;
    effect->binded = FALSE;
    effect->fftcb = 0;
    effect->pcmcb = 0;
	effect->process = effect_process;
    effect->order = -1;
    effect->source = 0;
    effect->destroy  = effect_destroy;
    effect->setfloat = effect_set_float;
    effect->getfloat = effect_get_float;
    effect->setint = 0;
    effect->getint = 0;
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
    addon.addon = new gcompressoreffectcreator();
    return &addon;
}






