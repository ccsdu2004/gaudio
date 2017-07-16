#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmain.h"
 
geffect* geffect_create(); 

gecreator(zipper)

int32_t gzippereffectcreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_ZIPPER)
       return TRUE;
    return FALSE;            
} 
 
geffect* gzippereffectcreator::create_by_id(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_ZIPPER)
       return geffect_create();
    return NULL;              
}
 
void gzippereffectcreator::destroy()
{
    delete this; 
}

struct zipper
{
    float sum;
    float v;
};

struct geffectimpl
{   
    int32_t type;
    int32_t skip;
    int32_t cur;
    zipper  data[AUDIO_CHANNEL_MAX];
};

inline void effect_init_impl(geffectimpl* impl,zipper& zip,int32_t ch)
{        
    zip.sum = zip.v = .0f;      
}

inline void effect_reset(geffectimpl*& impl)
{
    for(int32_t i=0;i<AUDIO_CHANNEL_MAX;i++)
        effect_init_impl(impl,impl->data[i],i); 
    impl->cur = 0;      
}

inline void effect_init(geffectimpl*& zip)
{
    zip->type = AUDIO_EFFECT_ZIPPER_TYPE_DEFAULT;
    zip->skip = AUDIO_EFFECT_ZIPPER_TIME_DEFAULT + 2000; 
    effect_reset(zip);
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

void effect_process_impl(geffectimpl*& impl,int channel,float*& buffer,int32_t length)
{   
    zipper* zip = &impl->data[channel];
    float in,out;     
    for(int32_t i = 0;i < length;i++) 
    {
        in = buffer[i];
        if(impl->cur ++ % impl->skip)
        {
            impl->cur == 0;
            zip->sum = .0f;
            zip->v = in;             
        }
        else
        {   
            zip->sum += in;   
        }
        
        if(impl->type == AUDIO_EFFECT_ZIPPER_TYPE_LOW)
            buffer[i] = zip->v;
        else
        {
            buffer[i] = zip->sum/(float)impl->skip;   
        }
    }
    
    printf("zip:%d\n",impl->skip);
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
    geffectimpl* zip = effect->impl;     
    switch(flag)
    {
    case AUDIO_EFFECT_ZIPPER_TYPE:
         zip->type = gclamp(value,
             AUDIO_EFFECT_ZIPPER_TYPE_LOW,AUDIO_EFFECT_ZIPPER_TYPE_HIGH);
         effect->update = TRUE;
         break;
    case AUDIO_EFFECT_ZIPPER_TIME:
         value = gclamp(value,
             AUDIO_EFFECT_ZIPPER_TIME_LOW,
             AUDIO_EFFECT_ZIPPER_TIME_HIGH);
         value *= 44.1f;
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
    case AUDIO_EFFECT_ZIPPER_TYPE:
         *value = wah->type;
         break;
    case AUDIO_EFFECT_ZIPPER_TIME:
         *value = wah->skip/44.1f;
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
    addon.addon = new gzippereffectcreator();
    return &addon;
}






