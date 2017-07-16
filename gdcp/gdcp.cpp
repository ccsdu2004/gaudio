#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "gaudio.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmain.h"
#include "../gaudio/gmath.h"

#define ATT_VALUE (0.0001f)
#define REL_VALUE (0.003f)

#define SEC_TIME_C 8*(int32_t)AUDIO_STANDARD_SAMPLERATE
#define SEC_TIME SEC_TIME_C
#define P2 6.2831853f

int BufferLen_Comp = 303;
float Comp_Buffer[606];
float GateThresh_Comp =0.05;// [0-1] % must be < LimiterThresh_Comp
float GateFactor_Comp=0.60; //[0-1] %
float LimiterThresh_Comp=0.50; //[0-1] % must be > GateThresh_Comp
float LimiterFactor_Comp=0.80; //[0-1] %
float Compression_Comp = 0.30; //  [0-1] %
float outGain_Comp = 0.2; //[1-10]
 
geffect* geffect_create(); 

gecreator(compressor)

int32_t gcompressoreffectcreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_COMPRESSER)
       return TRUE;
    return FALSE;            
} 
 
geffect* gcompressoreffectcreator::create_by_id(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_COMPRESSER)
       return geffect_create();
    return NULL;              
}
 
void gcompressoreffectcreator::destroy()
{
    delete this; 
}
 
struct geffectimpl
{
    uint32_t sn;
    uint32_t first;
    uint32_t count;
    float att;
    float rel;
    float env;
    float x_previous_Comp; 
    float sum_previous_Comp;      
    float depth;
    float period;
    float phaser;
};

inline void effect_init(geffectimpl* impl)
{   
    impl->sn     = 0;
    impl->first  = 1;
    impl->count  = 0;
    impl->env    = 0.0f;
    impl->x_previous_Comp = 0.0f;
    impl->sum_previous_Comp = 0.0f;
    
    impl->att = (ATT_VALUE == 0.0) ? (0.0) : exp (-1.0 / (AUDIO_STANDARD_SAMPLERATE * ATT_VALUE));
    impl->rel = (REL_VALUE == 0.0) ? (0.0) : exp (-1.0 / (AUDIO_STANDARD_SAMPLERATE * REL_VALUE));
    
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

void effect_process_impl()
{    
}

void Sample_Comp(float* l,float* r,int32_t CPoints,geffect* effect)
{ 
    //implementation of the compressor with moving average ... lets see if it works :)
    float sum = 0;
    float rms = 0;
    float s = 0;

    Comp_Buffer[(effect->impl->sn%CPoints)*2] =* l;
    Comp_Buffer[(effect->impl->sn%CPoints)*2+1] =* r;

    if(effect->impl->first)
    {
        if(effect->impl->count<CPoints+1)
        {
            effect->impl->count ++;
            return;
            // do nothing - wait until we reach the FPoint-th sample to calculate the average
        }

        effect->impl->first = 0;
        
        for(int i=CPoints-1;i>=0;i--)
        {
            s=0.5*Comp_Buffer[((effect->impl->sn-i)%CPoints)*2]+0.5*Comp_Buffer[((effect->impl->sn-i)%CPoints)*2+1];
            sum+= s*s;
        }
        effect->impl->sum_previous_Comp=sum;
        s=0.5*Comp_Buffer[((effect->impl->sn-CPoints+1)%CPoints)*2]+0.5*Comp_Buffer[((effect->impl->sn-CPoints+1)%CPoints)*2+1];
        effect->impl->x_previous_Comp=s*s;
        return;
    }

    //calculate the average for the y[n] - the output signal
    s=0.5*(*l)+0.5*(*r);
    sum = effect->impl->sum_previous_Comp + (s*s) - effect->impl->x_previous_Comp;
    rms=sqrt(sum/CPoints);
    effect->impl->sum_previous_Comp=sum;
    s=0.5*Comp_Buffer[((effect->impl->sn-CPoints+1)%CPoints)*2]+0.5*Comp_Buffer[((effect->impl->sn-CPoints+1)%CPoints)*2+1];
    effect->impl->x_previous_Comp=s*s;

    // dynamic selection: attack or release?
    bool attack = (rms > effect->impl->env)? true : false;
    double  theta = (rms > effect->impl->env)? effect->impl->att : effect->impl->rel;
    // smoothing with capacitor, envelope extraction...
    // here be aware of pIV denormal numbers glitch
    effect->impl->env = (1.0 - theta) * rms + theta * effect->impl->env;

    float coef=2000;
    //if (useJack) coef = 0.1;

    //The Gate
    double  Ggain = 1.0;
    if(effect->impl->env < GateThresh_Comp*coef)
    {
        Ggain = Ggain - GateFactor_Comp*(GateThresh_Comp*coef - effect->impl->env) / (GateThresh_Comp*coef);
        // result - two hard kneed gated channels...
        *l= (*l) * Ggain;
        *r= (*r) * Ggain;
        return;
    }

    //The Compressor (maybe it is called expander...)
    double  Cgain = 1.0;
    if(effect->impl->env < LimiterThresh_Comp*coef)
    {
        Cgain = Cgain + Compression_Comp*(LimiterThresh_Comp*coef- effect->impl->env) /(LimiterThresh_Comp*coef);
        // result - two hard kneed compressed channels...
        *l= (*l) * Cgain;
        *r= (*r) * Cgain;
        return;
    }

    //The limiter
    float  Lgain = 1.0f;
    if(effect->impl->env > LimiterThresh_Comp*coef)
    {
        Lgain = Lgain - LimiterFactor_Comp*(effect->impl->env - LimiterThresh_Comp*coef) / (effect->impl->env);
        // result - two hard kneed limited channels...
        *l= (*l) * Lgain;
        *r= (*r) * Lgain;
        return;
    }
    return;
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
    addon.addon = new gcompressoreffectcreator();
    return &addon;
}






