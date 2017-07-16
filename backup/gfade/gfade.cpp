#include <stdio.h>
#include <stdlib.h>
#include "gaudio.h"
#include "fade.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmath.h"

#define MIN_FADE_NEED_TIME 6000

geffect* geffect_create();

gecreator(fade)

int32_t gfadeeffectcreator::is_support(int32_t id,int32_t mask)
{
    if(id == AUDIO_EFFECT_FADE_IN ||
       id == AUDIO_EFFECT_FADE_OUT)
       return TRUE;
    return FALSE;            
} 

struct geffectimpl
{
    geffectimpl():fade(0.0f),length(0){}        
    float fade;
    uint32_t length;        
};
 
geffect* gfadeeffectcreator::create_by_id(int32_t id,int32_t mask)
{   
    if(id == AUDIO_EFFECT_FADE_IN || 
       id == AUDIO_EFFECT_FADE_OUT)
    {   
        geffect* efx = geffect_create(); 
        efx->curid = id;
        if(id == AUDIO_EFFECT_FADE_IN)
           efx->impl->fade = AUDIO_EFFECT_FADE_IN_DEFAULT;
        else if(id == AUDIO_EFFECT_FADE_OUT)
           efx->impl->fade = AUDIO_EFFECT_FADE_OUT_DEFAULT; 
        return efx;    
    }
    return NULL;              
}
 
void gfadeeffectcreator::destroy()
{
    delete this; 
}

char* effect_maker(geffect*)
{
    return "duwenhua2013";        
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
    int32_t duration = 0; 
    gaudio_source_get_int32(source,AUDIO_ENUM_DURATION,&duration);
    if(duration < MIN_FADE_NEED_TIME)
        return;  
    uint32_t fade = effect->impl->fade*AUDIO_STANDARD_SAMPLERATE;   
    if(effect->curid == AUDIO_EFFECT_FADE_IN)
    {   
        if(effect->impl->length > fade)
            return; 
        else
        {   
            long oldlen = effect->impl->length;
            effect->impl->length += buffer.use;
            long rec = gmin(effect->impl->length,fade);
            rec = gmin<long>(buffer.use,rec);
            float inc = 1.0f/(float)fade;
            for(uint32_t i=0;i<rec;i++)
            {
                buffer.data[0][i] *= (oldlen+i)*inc;
                buffer.data[1][i] *= (oldlen+i)*inc;                
            }    
        }      
    }
    else if(effect->curid == AUDIO_EFFECT_FADE_OUT)
    {   
        int32_t position;
        gaudio_source_get_int32(source,AUDIO_ENUM_POSITION,&position);
        if(position + fade < duration)
            return;
        else
        {   
            long oldlen = effect->impl->length;
            effect->impl->length += buffer.use;
            float inc = 1.0f/(float)fade;
            float curpos = (float)effect->impl->length/AUDIO_STANDARD_SAMPLERATE; 
            long rec = duration - 1000*curpos;
            rec -= fade;
            rec -= 499;
            int i = rec < 0 ? 0 : rec;
            while(i<buffer.use)
            {   
                buffer.data[0][i] *= (fade-oldlen-i)*inc;
                buffer.data[1][i] *= (fade-oldlen-i)*inc;          
                i++;
            }  
        }          
    }
}

void GAPIENTRY effect_set_float(geffect* effect,int32_t flag,float value)
{   
    if(effect->curid == AUDIO_EFFECT_FADE_IN && flag == AUDIO_EFFECT_FADE_IN_VALUE)
        effect->impl->fade = gclamp(value,AUDIO_EFFECT_FADE_IN_MIN,AUDIO_EFFECT_FADE_IN_MAX);                 
    else if(effect->curid == AUDIO_EFFECT_FADE_OUT && flag == AUDIO_EFFECT_FADE_OUT_VALUE)
        effect->impl->fade = gclamp(value,AUDIO_EFFECT_FADE_OUT_MIN,AUDIO_EFFECT_FADE_OUT_MAX); 
}

void GAPIENTRY effect_get_float(geffect* effect,int32_t flag,float* value)
{   
    if(flag == AUDIO_EFFECT_FADE_IN_VALUE || flag == AUDIO_EFFECT_FADE_OUT_VALUE)
        *value = effect->impl->fade;
}
 
geffect* geffect_create()
{
    geffect* effect = new geffect();
    effect->mask = EFFECT_SUPPORT_BIND_TO_SOURCE;
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
    effect->impl = new geffectimpl();
    return effect;          
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{             
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new gfadeeffectcreator();
    return &addon;
}






