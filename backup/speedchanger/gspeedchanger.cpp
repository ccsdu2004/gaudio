#include <stdio.h>
#include <stdlib.h>
#include "speedchanger.h"
#include "../gaudio/gfilter.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmain.h"
#include "../gaudio/gmath.h"

typedef void*(*gresample_create__ptr)(int quality,float lowfactor,float highfactor);
typedef long (*gresample_process__ptr)(void* handle,float factor,float* in,int inLen,int lastflag,int* used,float* out,int outlen);
typedef void (*gresample_close__ptr)(void*);

static gresample_create__ptr  gresample_create_ptr = 0;
static gresample_process__ptr gresample_process_ptr = 0;
static gresample_close__ptr   gresample_close_ptr = 0;
 
geffect* geffect_create(); 

gecreator(speedchanger)

int32_t gspeedchangereffectcreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_SPEED_CHANGER)
        return TRUE;
    return FALSE;            
} 
 
geffect* gspeedchangereffectcreator::create_by_id(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_SPEED_CHANGER)
       return geffect_create();
    return NULL;              
}
 
void gspeedchangereffectcreator::destroy()
{
    delete this; 
}

struct geffectimpl
{   
    geffectimpl():ratio(1.0f),update(FALSE)
    {
        left = gresample_create_ptr(1,AUDIO_EFFECT_SPEED_CHANGER_RATIO_LOW,AUDIO_EFFECT_SPEED_CHANGER_RATIO_HIGH);
        right = gresample_create_ptr(1,AUDIO_EFFECT_SPEED_CHANGER_RATIO_LOW,AUDIO_EFFECT_SPEED_CHANGER_RATIO_HIGH);
        buffer[0] = new float[const_size];
        buffer[1] = new float[const_size];
        usefilter = TRUE;
        
        float cw = GFILTERCOS;
        float g = 1.0f;
        filter[0].coeff = filter[1].coeff = gcoeffcalc(g,cw);
        filter[0].history[0] = filter[0].history[1] = filter[1].history[0] = filter[1].history[1] = 0.0f;
    }
    
    ~geffectimpl()
    {   
        gresample_close_ptr(left);
        gresample_close_ptr(right);
        delete []buffer[0];
        delete []buffer[1];           
    }
    
    float   ratio;  
    int32_t update;  
    void*   left;
    void*   right;
    float*  buffer[2];
    
    int32_t usefilter;
    gfilter filter[2];
    
    static int32_t const_size;
};

int32_t geffectimpl::const_size = STD_BUFFER_SIZE*AUDIO_EFFECT_SPEED_CHANGER_RATIO_HIGH+1;

inline void effect_init(geffectimpl* changer)
{
    changer->ratio = AUDIO_EFFECT_SPEED_CHANGER_RATIO_DEFAULT;
    changer->update = FALSE;
}

static char* effect_maker(geffect*)
{
    return "http://www.geek-audio.org/";        
}

void effect_destroy(geffect* effect)
{   
    geffectimpl* changer = effect->impl;
    delete changer;
    changer = 0;
    delete effect;     
}

void GAPIENTRY effect_process(geffect* effect,gdatabuffer<float>& buffer)
{    
    geffectimpl* changer = effect->impl;
    if(changer->update)
        changer->update = FALSE;      
    if(gisequal(changer->ratio,AUDIO_EFFECT_SPEED_CHANGER_RATIO_DEFAULT))
        return;                     
    int used = 0;
    int32_t ret1 = gresample_process_ptr(changer->left,changer->ratio,buffer.data[0],buffer.use,1,&used,changer->buffer[0],changer->const_size);
    int32_t ret2 = gresample_process_ptr(changer->right,changer->ratio,buffer.data[1],buffer.use,1,&used,changer->buffer[1],changer->const_size);            
    if(ret1 > buffer.length)
        return;  
    
    float v;
    for(int i=0;i<ret1;i++)
    {
        v = gclamp(changer->buffer[0][i],-1.0f,1.0f);
        changer->buffer[0][i] = v;//changer->update ? gfilter2p(&changer->filter[0],0,v) : v;
    }
    
    for(int i=0;i<ret2;i++)
    {   
        v = gclamp(changer->buffer[1][i],-1.0f,1.0f);
        changer->buffer[1][i] = v;//changer->update ? gfilter2p(&changer->filter[1],0,v) : v;            
    }
    
    std::copy(changer->buffer[0],changer->buffer[0]+ret1,buffer.data[0]);
    std::copy(changer->buffer[1],changer->buffer[1]+ret2,buffer.data[1]); 
    buffer.use = gmin(ret1,ret2);     
}

void GAPIENTRY effect_set_float(geffect* effect,int32_t flag,float value)
{
    if(flag != AUDIO_EFFECT_SPEED_CHANGER_RATIO)
        return; 
    geffectimpl* changer = effect->impl;  
    switch(flag)
    {
    case AUDIO_EFFECT_SPEED_CHANGER_RATIO:
         changer->ratio = gclamp(value,AUDIO_EFFECT_SPEED_CHANGER_RATIO_LOW,AUDIO_EFFECT_SPEED_CHANGER_RATIO_HIGH);
         effect->update = TRUE;
         break;
    default:
         return;             
    }
}

void GAPIENTRY effect_get_float(geffect* effect,int32_t flag,float* value)
{
    if(flag != AUDIO_EFFECT_SPEED_CHANGER_RATIO)
        return; 
    geffectimpl* changer = effect->impl;     
    switch(flag)
    {
    case AUDIO_EFFECT_SPEED_CHANGER_RATIO:
         *value = changer->ratio;
         break;
    default:
         return;             
    }
}

//! effect_set_int32 and effect_get_int32 for testing!
void GAPIENTRY effect_set_int32(geffect* effect,int32_t flag,int32_t value)
{
    if(flag != AUDIO_EFFECT_SPEED_CHANGER_RATIO + 1)
        return; 
    flag --;    
    geffectimpl* changer = effect->impl;  
    switch(flag)
    {
    case AUDIO_EFFECT_SPEED_CHANGER_RATIO:
         effect->impl->usefilter = value;
         break;
    default:
         return;             
    }
}

void GAPIENTRY effect_get_int32(geffect* effect,int32_t flag,int32_t* value)
{
    if(flag != AUDIO_EFFECT_SPEED_CHANGER_RATIO + 1)
        return; 
    flag --;    
    geffectimpl* changer = effect->impl;     
    switch(flag)
    {
    case AUDIO_EFFECT_SPEED_CHANGER_RATIO:
         *value = changer->usefilter;
         break;
    default:
         return;             
    }
}
 
geffect* geffect_create()
{
    geffect* effect = new geffect();
    effect->mask = EFFECT_SUPPORT_BIND_TO_GLOBAL;
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
    
    geffectimpl* changer = new geffectimpl();
    effect_init(changer);
    effect->impl = changer;
    return effect;          
}

static gaddons addon;

gaddons* ginstance(void* ctx)
{
    gresample_create_ptr = (gresample_create__ptr)gaudio_address_get("gresample_create");
    gresample_process_ptr = (gresample_process__ptr)gaudio_address_get("gresample_process"); 
    gresample_close_ptr  = (gresample_close__ptr)gaudio_address_get("gresample_close"); 
             
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new gspeedchangereffectcreator();
    return &addon;
}






