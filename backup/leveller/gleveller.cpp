#include <stdio.h>
#include <stdlib.h>
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmain.h"
 
geffect* geffect_create(); 

gecreator(leveller)

int32_t glevellereffectcreator::is_support(int32_t dec,int32_t mask)
{   
    if(dec == AUDIO_EFFECT_LEVELLER)
       return TRUE;
    return FALSE;            
} 
 
geffect* glevellereffectcreator::create_by_id(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_LEVELLER)
       return geffect_create();
    return NULL;              
}
 
void glevellereffectcreator::destroy()
{
    delete this; 
}

#define LEVELER_FACTORS 6
static double gLimit[LEVELER_FACTORS] = { 0.0001, 0.0, 0.1, 0.3, 0.5, 1.0};
static double gAdjFactor[LEVELER_FACTORS] = { 0.80, 1.00, 1.20, 1.20, 1.00, 0.80};

struct gleveller
{
    int    mLevellerDbChoiceIndex;
    int    mLevellerNumPasses;
    double mLevellerDbSilenceThreshold;  
    
    //
    double gFrameSum;  
    
    double gAdjLimit[LEVELER_FACTORS];
    double gAddOnValue[LEVELER_FACTORS]; 
};

struct geffectimpl
{   
    gleveller leveller[2];
};

void GAPIENTRY effect_set_float(geffect* effect,int32_t flag,float value)
{
}

void GAPIENTRY effect_get_float(geffect* effect,int32_t flag,float* value)
{
}
 
inline void effect_init(geffectimpl* impl)
{

}

inline void effect_reset(geffectimpl* impl)
{
    effect_init(impl); 

}

char* effect_maker(geffect*)
{
    return "duwenhua2013";        
}

void effect_destroy(geffect* effect)
{   
    delete effect->impl;
    effect->impl = 0;
    delete effect;
    effect = 0;     
}

void effect_process_impl(geffectimpl* impl,int channel,float*& buffer,int32_t length)
{
    float m,tmp,in,out;
    int i,j;
   
    for(i = 0; i < length; i++) 
    {
        in = buffer[i];
        //buffer[i] = gclamp(out,-1.0f,1.0f);
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

geffect* geffect_create()
{
    geffect* effect = new geffect();
    effect->mask = EFFECT_SUPPORT_BIND_TO_GLOBAL | EFFECT_SUPPORT_BIND_TO_SOURCE;
    effect->binded = FALSE;
    effect->efxcb = 0;
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
    effect_reset(impl);
    effect->impl = impl;
    return effect;          
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new glevellereffectcreator();
    return &addon;
}






