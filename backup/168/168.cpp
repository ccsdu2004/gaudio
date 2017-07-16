#include <stdio.h>
#include <stdlib.h>
#include "gaudio.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmain.h"

geffect* geffect_create();

gecreator(168)

int32_t g168effectcreator::is_support(int32_t id,int32_t mask)
{
    if(id == AUDIO_EFFECT_INVERT || 
       id == AUDIO_EFFECT_SWAP_CHANNEL ||
       id == AUDIO_EFFECT_MERGE_CHANNEL)
       return TRUE;
    return FALSE;            
} 

struct geffectimpl
{
    geffectimpl():fade(0.0f),length(0){}        
    float fade;
    uint32_t length;        
};
 
geffect* g168effectcreator::create_by_id(int32_t id,int32_t mask)
{   
    if(id == AUDIO_EFFECT_INVERT || 
       id == AUDIO_EFFECT_SWAP_CHANNEL ||
       id == AUDIO_EFFECT_MERGE_CHANNEL)
    {   
        geffect* efx = geffect_create(); 
        efx->curid = id;   
        return efx;    
    }
    return NULL;              
}
 
void g168effectcreator::destroy()
{
    delete this; 
}

const char* effect_maker(geffect*)
{
    return "duwenhua2013";        
}

void effect_destroy(geffect* effect)
{  
    effect->destroy(effect);  
}

void GAPIENTRY effect_process(geffect* effect,gdatabuffer<float>& buffer)
{     
    if(effect->curid == AUDIO_EFFECT_MERGE_CHANNEL)
    {   
        for(uint32_t i=0;i<buffer.use;i++)
            buffer.data[0][i] = buffer.data[1][i] = 0.5f*(buffer.data[0][i] + buffer.data[1][i]);
    }
    else if(effect->curid == AUDIO_EFFECT_INVERT)
    {
        for(uint32_t i=0;i<buffer.use;i++)
        {
            buffer.data[0][i] = -buffer.data[0][i];
            buffer.data[1][i] = -buffer.data[1][i];
        }
    }
    else if(effect->curid == AUDIO_EFFECT_SWAP_CHANNEL)
    {
        for(uint32_t i=0;i<buffer.use;i++)
            std::swap(buffer.data[0][i],buffer.data[1][i]);
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
    effect->curid = -1;
    effect->impl = NULL;
    return effect;          
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{             
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new g168effectcreator();
    return &addon;
}






