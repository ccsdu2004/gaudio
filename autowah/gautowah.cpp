#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmain.h"
#include "../gaudio/gmath.h"

#ifndef PI
#define PI 3.14159265f
#endif
 
#define LFOSKIP 30
 
geffect* geffect_create(); 

gecreator(autowah)

int32_t gautowaheffectcreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_AUTOWAH)
       return TRUE;
    return FALSE;            
} 
 
geffect* gautowaheffectcreator::create_by_id(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_AUTOWAH)
       return geffect_create();
    return NULL;              
}
 
void gautowaheffectcreator::destroy()
{
    delete this; 
}

struct autowah
{
    float phase;
    float lfoskip;
    uint32_t skipcount;
    float xn1,xn2,yn1,yn2;
    float b0,b1,b2,a0,a1,a2;
};

struct geffectimpl
{   
    float freq;
    float startphase;
    float depth;
    float freqofs;
    float res;
    autowah data[AUDIO_CHANNEL_MAX];
};

inline void effect_init_impl(geffectimpl* impl,autowah& wah,int32_t ch)
{        
    wah.lfoskip = impl->freq * 2 * PI / 44100.0f;
    wah.skipcount = 0;
    wah.xn1 = 0;
    wah.xn2 = 0;
    wah.yn1 = 0;
    wah.yn2 = 0;
    wah.b0 = 0;
    wah.b1 = 0;
    wah.b2 = 0;
    wah.a0 = 0;
    wah.a1 = 0;
    wah.a2 = 0;
    wah.phase = impl->startphase;
    if(ch == 1)
        wah.phase += (float)PI;         
}

inline void effect_reset(geffectimpl*& impl)
{
    for(int32_t i=0;i<AUDIO_CHANNEL_MAX;i++)
        effect_init_impl(impl,impl->data[i],i);   
}

inline void effect_init(geffectimpl*& wah)
{
    wah->freq = AUDIO_EFFECT_AUTOWAH_FREQ_DEFAULT;
    wah->startphase = AUDIO_EFFECT_AUTOWAH_PHASE_DEFAULT/180.0f;
    wah->depth = AUDIO_EFFECT_AUTOWAH_DEPTH_DEFAULT/100.0f;
    wah->freqofs = AUDIO_EFFECT_AUTOWAH_FREQOFF_DEFAULT/100.0f;
    wah->res = AUDIO_EFFECT_AUTOWAH_RESOURCE_DEFAULT/100.0f;
    effect_reset(wah);
}

char* effect_maker(geffect*)
{
    return "http://www.geek-audio.org/";        
}

void effect_destroy(geffect* effect)
{   
    delete effect->impl;
    effect->impl = 0;
    delete effect;
    effect = 0;     
}

void effect_process_impl(geffectimpl*& impl,int channel,float*& buffer,int32_t length)
{   
    autowah* wah = &impl->data[channel];
    float frequency,omega,sn,cs,alpha;
    float in,out;     
    for(int32_t i = 0;i < length;i++) 
    {
        in = buffer[i];
        if((wah->skipcount++) % LFOSKIP == 0) 
        {
            frequency = (1 + cos(wah->skipcount*wah->lfoskip + wah->phase)) / 2;
            frequency = frequency * impl->depth * (1 - impl->freqofs) + impl->freqofs;
            frequency = exp((frequency - 1) * 6);
            omega = PI * frequency;
            sn = sin(omega);
            cs = cos(omega);
            alpha = sn / (2 * impl->res);
            wah->b0 = (1 - cs) / 2;
            wah->b1 = 1 - cs;
            wah->b2 = (1 - cs) / 2;
            wah->a0 = 1 + alpha;
            wah->a1 = -2 * cs;
            wah->a2 = 1 - alpha;
        };
        out = (wah->b0 * in + wah->b1 * wah->xn1 + wah->b2 * wah->xn2 - wah->a1 * wah->yn1 - wah->a2 * wah->yn2) / wah->a0;
        wah->xn2 = wah->xn1;
        wah->xn1 = in;
        wah->yn2 = wah->yn1;
        wah->yn1 = out;
      
        // Prevents clipping
        if(out < -1.0f)
            out = -1.0f;
        else if(out > 1.0f)
            out = 1.0f;
      
        buffer[i] = (float)out;
    }
}

void GAPIENTRY effect_process(geffect* effect,gdatabuffer<float>& buffer)
{     
    geffectimpl* impl = effect->impl; 
    if(effect->update)
    {
        effect->update = FALSE;
        effect_reset(impl);                  
    }
    
    effect_process_impl(impl,0,buffer.data[0],buffer.use);
    effect_process_impl(impl,1,buffer.data[1],buffer.use);
}

void GAPIENTRY effect_set_float(geffect* effect,int32_t flag,float value)
{
    geffectimpl* wah = effect->impl;     
    switch(flag)
    {
    case AUDIO_EFFECT_AUTOWAH_FREQ:
         wah->freq = gclamp(value,AUDIO_EFFECT_AUTOWAH_FREQ_LOW,AUDIO_EFFECT_AUTOWAH_FREQ_HIGH);
         effect->update = TRUE;
         break;
    case AUDIO_EFFECT_AUTOWAH_FREQOFF:
         wah->freqofs = gclamp(value,AUDIO_EFFECT_AUTOWAH_FREQOFF_LOW,AUDIO_EFFECT_AUTOWAH_FREQOFF_HIGH);
         wah->freqofs /= 100.0f;
         effect->update = TRUE;
         break;
    case AUDIO_EFFECT_AUTOWAH_PHASE:
         wah->startphase = gclamp(value,AUDIO_EFFECT_AUTOWAH_PHASE_LOW,AUDIO_EFFECT_AUTOWAH_PHASE_HIGH);
         wah->startphase /= 180.0f;
         effect->update = TRUE;
         break;
    case AUDIO_EFFECT_AUTOWAH_DEPTH:
         wah->depth = gclamp(value,AUDIO_EFFECT_AUTOWAH_DEPTH_LOW,AUDIO_EFFECT_AUTOWAH_DEPTH_HIGH);
         wah->depth /= 100.0f; 
         effect->update = TRUE;
         break;
    case AUDIO_EFFECT_AUTOWAH_RESOURCE:
         wah->res = gclamp(value,AUDIO_EFFECT_AUTOWAH_RESOURCE_LOW,AUDIO_EFFECT_AUTOWAH_RESOURCE_HIGH);
         wah->res /= 100.0f;
         effect->update = TRUE;
         break;                           
    default:
         return;             
    }
}

void GAPIENTRY effect_get_float(geffect* effect,int32_t flag,float* value)
{
    geffectimpl* wah = effect->impl;   
    switch(flag)
    {
    case AUDIO_EFFECT_AUTOWAH_FREQ:
         *value = wah->freq;
         break;
    case AUDIO_EFFECT_AUTOWAH_FREQOFF:
         *value = wah->freqofs*100.0f;
         break;
    case AUDIO_EFFECT_AUTOWAH_PHASE:
         *value = wah->startphase*180.0f;
         break;
    case AUDIO_EFFECT_AUTOWAH_DEPTH:
         *value = wah->depth*100.0f;
         break;
    case AUDIO_EFFECT_AUTOWAH_RESOURCE:
         *value = wah->res*100.0f;
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
    
    geffectimpl* wah = new geffectimpl();
    effect_init(wah);
    effect->impl = wah;
    return effect;          
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new gautowaheffectcreator();
    return &addon;
}






