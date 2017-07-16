#include <stdio.h>
#include <stdlib.h>
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmain.h"

/*
#define PHASERLFOSHAPE 4.0f
#define LFOSKIPSAMPLES 20
#ifndef PI
#define PI 3.14159265f
#endif*/ 
 
geffect* geffect_create(); 

gecreator(stereoEnhancer)

int32_t gstereoEnhancereffectcreator::is_support(int32_t dec,int32_t mask)
{   
    if(dec == AUDIO_EFFECT_STEREO_ENHANCE)
       return TRUE;
    return FALSE;            
} 
 
geffect* gstereoEnhancereffectcreator::create_by_id(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_STEREO_ENHANCE)
       return geffect_create();
    return NULL;              
}
 
void gstereoEnhancereffectcreator::destroy()
{
    delete this; 
}

class stereoEnhancer
{
public:
    stereoEnhancer(float _wide_coeff):wideCoeff(_wide_coeff){}
		
    // Lou's Hack
    void setWideCoeff(float _wideCoeff){wideCoeff = _wideCoeff;}
    float getWideCoeff(){return wideCoeff;}
    void nextSample(float& _in_left,float& _in_right)
    {
        // Lou's Hack
		// I really can't tell you why this math works, but it sounds good
		float toRad = 3.141592 / 180;
		_in_left += _in_right * sinf(wideCoeff * .5 * toRad);
		_in_right -= _in_left * sinf(wideCoeff * .5 * toRad);
    }
private:
    // Lou's Hack
    float wideCoeff;
};

/*
struct gphaser
{
    unsigned long skipcount;
    float old[24];
    float gain;
    float fbout;
    float lfoskip;
    float phase;       
};

struct geffectimpl
{   
    bool  update;
    float freq;
    float depth;
    float phase;
    float stage;
    float dry;
    float fb;
    gphaser phaser[2];
};*/

void GAPIENTRY effect_set_float(geffect* effect,int32_t flag,float value)
{
    geffectimpl* impl = effect->impl;     
    /*switch(flag)
    {
    case AUDIO_EFFECT_PHASER_SAMPLERATE:
         impl->freq =  
             gclamp(value,AUDIO_EFFECT_PHASER_FREQUENCY_LOW,
                          AUDIO_EFFECT_PHASER_FREQUENCY_HIGH);
         impl->update = TRUE;
         break;
    case AUDIO_EFFECT_PHASER_PHASE:
         impl->phase = 
             gclamp(value,AUDIO_EFFECT_PHASER_PHASE_LOW,
                          AUDIO_EFFECT_PHASER_PHASE_HIGH);
         impl->update = TRUE;
         break;
    case AUDIO_EFFECT_PHASER_DEPTH:
         impl->depth = 
             gclamp(value,AUDIO_EFFECT_PHASER_DEPTH_LOW,
                          AUDIO_EFFECT_PHASER_DEPTH_HIGH);
         impl->update = TRUE;
         break; 
    case AUDIO_EFFECT_PHASER_STAGE:
         impl->stage = 
             gclamp(value,AUDIO_EFFECT_PHASER_STAGE_LOW,
                          AUDIO_EFFECT_PHASER_STAGE_HIGH);
         impl->update = TRUE;
         break;       
    case AUDIO_EFFECT_PHASER_FB:
         impl->fb = 
             gclamp(value,AUDIO_EFFECT_PHASER_FB_LOW,
                          AUDIO_EFFECT_PHASER_FB_HIGH);
         impl->update = TRUE;
         break; 
    case AUDIO_EFFECT_PHASER_DRY:
         impl->dry = 
             gclamp(value,AUDIO_EFFECT_DRY_LOW,
                          AUDIO_EFFECT_DRY_HIGH);
         impl->update = TRUE;
         break;                             
    default:
         return;             
    }*/
}

void GAPIENTRY effect_get_float(geffect* effect,int32_t flag,float* value)
{
    geffectimpl* impl = effect->impl;   
    /*switch(flag)
    {
    case AUDIO_EFFECT_PHASER_SAMPLERATE:
         *value = impl->freq;
         break;
    case AUDIO_EFFECT_PHASER_PHASE:
         *value = impl->phase;
         break;
    case AUDIO_EFFECT_PHASER_DEPTH:
         *value = impl->depth;
         break;
    case AUDIO_EFFECT_PHASER_STAGE:
         *value = impl->stage;
         break;  
    case AUDIO_EFFECT_PHASER_FB:
         *value = impl->fb;
         break;
    case AUDIO_EFFECT_PHASER_DRY:
         *value = impl->dry;
         break;                      
    default:
         return;             
    }*/
}
 
inline void effect_init(geffectimpl* impl)
{
    /*impl->freq = 0.4f;
    impl->depth = 100.0f;
    impl->phase = 0.0f;
    impl->stage = 2.0f;
    impl->dry = 128.0f;
    impl->fb = 0.0f;*/
}

inline void effect_reset(geffectimpl* impl)
{
    /*effect_init(impl); 
    for(int j = 0; j < impl->stage; j++)
    {
        impl->phaser[0].old[j] = 0.0f;
        impl->phaser[1].old[j] = 0.0f;    
    }
    
    impl->phaser[0].skipcount = impl->phaser[1].skipcount = 0;
    impl->phaser[0].gain = impl->phaser[1].gain = 0;
    impl->phaser[0].fbout = impl->phaser[1].fbout = 0;
    impl->phaser[0].lfoskip = 
        impl->phaser[1].lfoskip = impl->freq * 2 * PI / AUDIO_STANDARD_SAMPLERATE;
    impl->phaser[0].phase = impl->phaser[1].phase = impl->phase;
    impl->phaser[1].phase += PI;*/
}

char* effect_maker(geffect*)
{
    return "duwenhua2013";        
}

void effect_destroy(geffect* effect)
{   
    //delete effect->impl;
    effect->impl = 0;
    //delete effect;
    effect = 0;     
}

void effect_process_impl(geffectimpl* impl,int channel,float*& buffer,int32_t length)
{
    /*float m,tmp,in,out;
    int i,j;
   
    for(i = 0; i < length; i++) 
    {
        in = buffer[i];
        m = in + impl->phaser[channel].fbout * impl->fb/100.0f;
        if(((impl->phaser[channel].skipcount++) % LFOSKIPSAMPLES) == 0) 
        {
            impl->phaser[channel].gain = 
                (1 + cos(impl->phaser[channel].skipcount * 
                   impl->phaser[channel].lfoskip + 
                       impl->phaser[channel].phase)) / 2.0f;
         
            impl->phaser[channel].gain =
                (exp(impl->phaser[channel].gain * 
                    PHASERLFOSHAPE) - 1) / 
                        (exp(PHASERLFOSHAPE)-1);
         
            impl->phaser[channel].gain = 
                 1 - impl->phaser[channel].gain / 255.0f * impl->depth;    
        }
 
        for(j = 0; j < impl->stage; j++) 
        {
            tmp = impl->phaser[channel].old[j];
            impl->phaser[channel].old[j] = impl->phaser[channel].gain * tmp + m;
            m = tmp - impl->phaser[channel].gain * impl->phaser[channel].old[j];
        }
    
        impl->phaser[channel].fbout = m;
        out = (m * impl->dry + in * (255.0f - impl->dry)) / 255.0f;
        buffer[i] = gclamp(out,-1.0f,1.0f);
    }*/
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
    
    /*geffectimpl* impl = new geffectimpl();
    effect_init(impl);
    effect_reset(impl);
    effect->impl = impl;*/
    return effect;          
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new gstereoEnhancereffectcreator();
    return &addon;
}






