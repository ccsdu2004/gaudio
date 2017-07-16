#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmain.h"
 
geffect* geffect_create(); 

gecreator(pcm)

int32_t gpcmeffectcreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_PCM)
       return TRUE;
    return FALSE;            
} 
 
geffect* gpcmeffectcreator::create_by_id(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_PCM)
       return geffect_create();
    return NULL;              
}
 
void gpcmeffectcreator::destroy()
{
    delete this; 
}

struct zipper
{
    float sum;
    float v;
};

const char* effect_maker(geffect*)
{
    return "duwenhua2013";        
}

void effect_destroy(geffect* effect)
{   
    delete effect;
    effect = 0;     
}

void GAPIENTRY effect_process(geffect* effect,gdatabuffer<float>& buffer)
{     
    if(effect->pcmcb)
        effect->pcmcb(effect,buffer.data,2,44100,buffer.use); 
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
    effect->setint = 0;
    effect->getint = 0;
    effect->state = TRUE;
    effect->impl = 0;
    return effect;          
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new gpcmeffectcreator();
    return &addon;
}






