#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "bs2b.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmain.h"
#include "../gaudio/gmath.h"
 
geffect* geffect_create(); 

gecreator(cf)

int32_t gcfeffectcreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_CROSSFEED)
       return TRUE;
    return FALSE;            
} 
 
geffect* gcfeffectcreator::create_by_id(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_CROSSFEED)
       return geffect_create();
    return NULL;              
}
 
void gcfeffectcreator::destroy()
{
    delete this; 
}

struct geffectimpl
{   
    bs2b handle;
    int32_t type;
    
    geffectimpl()
    {
        bs2b_set_level(&handle,type = BS2B_HIGH_CLEVEL);
    }
};

void effect_init(geffectimpl*& impl)
{   
    bs2b_clear(&impl->handle);
    bs2b_set_level(&impl->handle,impl->type);
    bs2b_set_srate(&impl->handle,AUDIO_STANDARD_SAMPLERATE);
}

const char* effect_maker(geffect*)
{
    return "D.Eric";        
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
        effect_init(impl);                  
    }
    
    float buf[2] = {0};
    for(int32_t i=0;i<buffer.use;i++)
    {
        buf[0] = buffer.data[0][i];
        buf[1] = buffer.data[1][i];
        bs2b_cross_feed(&effect->impl->handle,buf);
        buffer.data[0][i] = buf[0];
        buffer.data[1][i] = buf[1];         
    }
}

void GAPIENTRY effect_set_int(geffect* effect,int32_t flag,int32_t value)
{
    geffectimpl* impl = effect->impl;     
    switch(flag)
    {
    case AUDIO_EFFECT_CROSSFFED_DEEP:
         impl->type = gclamp(value,AUDIO_EFFECT_CROSSFFED_LOW,AUDIO_EFFECT_CROSSFFED_HIGH);
         effect->update = TRUE;
         break;                       
    default:
         return;             
    }
}

void GAPIENTRY effect_get_int(geffect* effect,int32_t flag,int32_t* value)
{
    geffectimpl* impl = effect->impl;   
    switch(flag)
    {
    case AUDIO_EFFECT_CROSSFFED_DEEP:
         *value = impl->type;
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
    effect->setfloat = 0;
    effect->getfloat = 0;
    effect->setint = effect_set_int;
    effect->getint = effect_get_int;
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
    addon.addon = new gcfeffectcreator();
    return &addon;
}






