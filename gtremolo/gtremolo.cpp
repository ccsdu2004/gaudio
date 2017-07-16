#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gaudio.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmain.h"
#include "../gaudio/gmath.h"

#define SEC_TIME_C 8*(int32_t)AUDIO_STANDARD_SAMPLERATE
#define SEC_TIME SEC_TIME_C
#define P2 6.2831853f
 
geffect* geffect_create(); 

gecreator(tremolo)

int32_t gtremoloeffectcreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_TREMOLO)
       return TRUE;
    return FALSE;            
} 
 
geffect* gtremoloeffectcreator::create_by_id(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_TREMOLO)
       return geffect_create();
    return NULL;              
}
 
void gtremoloeffectcreator::destroy()
{
    delete this; 
}
 
struct geffectimpl
{
    uint32_t sn;       
    float depth;
    float period;
    float phaser;
};

inline void effect_init(geffectimpl* impl)
{   
    impl->sn     = 0;
    impl->depth  = AUDIO_EFFECT_TREMOLO_DEPTH_DEFAULT;
    impl->period = AUDIO_EFFECT_TREMOLO_PERIOD_DEFAULT;
    impl->phaser = AUDIO_EFFECT_TREMOLO_PHASE_DEFAULT;
}

const char* effect_maker(geffect*)
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

void GAPIENTRY effect_process(geffect* effect,gdatabuffer<float>& buffer)
{     
    geffectimpl* impl = effect->impl; 
    if(effect->update)
    {
        effect->update = FALSE;                     
    }
    
    float l,r;
    for(int32_t i=0;i<buffer.use;i++) 
    {   
        effect->impl->sn = (effect->impl->sn++)%(SEC_TIME);
        l = buffer.data[0][i];
        r = buffer.data[1][i];
 
        l *= (1-effect->impl->depth*sin(P2*effect->impl->sn/(AUDIO_STANDARD_SAMPLERATE*effect->impl->period)));
        r *= (1-effect->impl->depth*sin(P2*effect->impl->sn/(AUDIO_STANDARD_SAMPLERATE*effect->impl->period)+effect->impl->phaser));  
        buffer.data[0][i] = l;
        buffer.data[1][i] = r;
    }   
}

void GAPIENTRY effect_set_float(geffect* effect,int32_t flag,float value)
{
    if(flag != AUDIO_EFFECT_TREMOLO_DEPTH || 
       flag != AUDIO_EFFECT_TREMOLO_PERIOD ||
       flag != AUDIO_EFFECT_TREMOLO_PHASER)
        return; 
        
    geffectimpl* impl = effect->impl;     
    switch(flag)
    {
    case AUDIO_EFFECT_TREMOLO_DEPTH:
         impl->depth = gclamp(value,AUDIO_EFFECT_TREMOLO_DEPTH_LOW,AUDIO_EFFECT_TREMOLO_DEPTH_HIGH);
         effect->update = TRUE;
         break;
    case AUDIO_EFFECT_TREMOLO_PERIOD:
         impl->period = gclamp(value,AUDIO_EFFECT_TREMOLO_PERIOD_LOW,AUDIO_EFFECT_TREMOLO_PERIOD_HIGH);
         effect->update = TRUE;
         break;
    case AUDIO_EFFECT_TREMOLO_PHASER:
         impl->phaser = gclamp(value,AUDIO_EFFECT_TREMOLO_PHASE_LOW,AUDIO_EFFECT_TREMOLO_PHASE_HIGH);
         impl->phaser /= 360.0f*P2;
         effect->update = TRUE;
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
    case AUDIO_EFFECT_TREMOLO_DEPTH:
         *value = impl->depth;
         break;
    case AUDIO_EFFECT_TREMOLO_PERIOD:
         *value = impl->period;
         break;
    case AUDIO_EFFECT_TREMOLO_PHASER:
         *value = impl->phaser * P2/360.0f;   
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
    
    geffectimpl* impl = new geffectimpl();
    effect_init(impl);
    effect->impl = impl;
    return effect;          
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new gtremoloeffectcreator();
    return &addon;
}






