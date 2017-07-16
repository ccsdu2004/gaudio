#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "gaudio.h"
#include "modulator.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmain.h"
#include "../gaudio/gfilter.h"

#define WAVEFORM_FRACBITS  16
#define WAVEFORM_FRACONE   (1<<WAVEFORM_FRACBITS)
#define WAVEFORM_FRACMASK  (WAVEFORM_FRACONE-1)

static inline float this_sin_(uint32_t index)
{
    return sin(index/(double)(1<<WAVEFORM_FRACBITS)*M_PI*2.0f);
}

static inline float this_saw_(uint32_t index)
{
    return index*2.0f/(1<<WAVEFORM_FRACBITS)-1.0f;
}

static inline float this_square_(uint32_t index)
{
    return ((index>>(WAVEFORM_FRACBITS-1))&1)?-1.0f:1.0f;
}

static inline float this_filter_1p(gfilter* iir,uint32_t offset,float input)
{
    float* history = &iir->history[offset];
    float a = iir->coeff;
    float output = input;
    output = output + (history[0]-output)*a;
    history[0] = output;
    return input - output;
}

geffect* geffect_create();

gecreator(modulator)

int32_t gmodulatoreffectcreator::is_support(int32_t id,int32_t mask)
{
    if(id == AUDIO_EFFECT_MODULATOR)
       return TRUE;
    return FALSE;            
} 
 
geffect* gmodulatoreffectcreator::create_by_id(int32_t id,int32_t mask)
{   
    if(id == AUDIO_EFFECT_MODULATOR)
    {   
        return geffect_create();    
    }
    return NULL;              
}
 
void gmodulatoreffectcreator::destroy()
{
    delete this; 
}

char* effect_maker(geffect*)
{
    return "duwenhua2013";        
}
 
typedef float(*process_ptr)(uint32_t);

struct geffectimpl
{  
    int32_t   waveform;
    float     samplerate;
    float     cutoff;        
    uint32_t  index;
    uint32_t  step;
    gfilter   iir;
    float     history[1];
           
    process_ptr process;   
    
    geffectimpl()
    {   
        waveform = AUDIO_EFFECT_MODULATOR_SINUSOID;
        samplerate = AUDIO_EFFECT_MODULATOR_FREQUENCY_DEFAULT;
        cutoff = AUDIO_EFFECT_MODULATOR_HIGHPASS_CUTOFF_DEFAULT;
        
        index = 0;
        step = AUDIO_EFFECT_MODULATOR_FREQUENCY_DEFAULT*(1<<WAVEFORM_FRACBITS) / 44100.0f;
        float cw = cos(2.0*M_PI * AUDIO_EFFECT_MODULATOR_HIGHPASS_CUTOFF_DEFAULT / 44100.0f);
        float a = (2.0f-cw) - sqrt(pow(2.0f-cw, 2.0f) - 1.0f);
        iir.coeff = a; 
        process = this_sin_;
    }
};

void effect_destroy(geffect* effect)
{   
    geffectimpl* modulator = effect->impl;
    delete modulator;
    modulator = 0;
    effect->destroy(effect);  
}

void GAPIENTRY effect_process(geffect* effect,gdatabuffer<float>& buffer)
{     
    geffectimpl* modulator = effect->impl;
    uint32_t index = modulator->index;
    float gain = 0.0f;
    for(uint32_t i=0;i<buffer.use;i++)
    {
        index += modulator->step;
        index &= WAVEFORM_FRACMASK;
        gain = modulator->process(index);              
        buffer.data[0][i] *= gain;
        buffer.data[0][i] = this_filter_1p(&modulator->iir,0,buffer.data[0][i]);
        buffer.data[1][i] *= gain;
        buffer.data[1][i] = this_filter_1p(&modulator->iir,0,buffer.data[1][i]);
    } 
    modulator->index = index;
}

void GAPIENTRY effect_set_float(geffect* effect,int32_t flag,float value)
{   
    geffectimpl* modulator = effect->impl;
    if(!modulator)
        return;
    switch(flag)
    {
    case AUDIO_EFFECT_MODULATOR_FREQUENCY:
        modulator->step = value*(1<<WAVEFORM_FRACBITS) / AUDIO_STANDARD_SAMPLERATE;
        return;
    case AUDIO_EFFECT_MODULATOR_HIGHPASS_CUTOFF:
    {
        float cw = cos(2.0*M_PI * value / AUDIO_STANDARD_SAMPLERATE);
        float a = (2.0f-cw) - sqrt(pow(2.0f-cw, 2.0f) - 1.0f);
        modulator->iir.coeff = a; 
        return;
    }
    case AUDIO_EFFECT_MODULATOR_WAVEFORM:
    {
        if(value == AUDIO_EFFECT_MODULATOR_SINUSOID)
        {    
            modulator->waveform = AUDIO_EFFECT_MODULATOR_SINUSOID;
            modulator->process = this_sin_;
        }   
        else if(value == AUDIO_EFFECT_MODULATOR_SAWTOOTH)
        {  
            modulator->waveform = AUDIO_EFFECT_MODULATOR_SAWTOOTH;
            modulator->process = this_saw_;
        }
        else if(value == AUDIO_EFFECT_MODULATOR_SQUARE)
        {   
            modulator->waveform = AUDIO_EFFECT_MODULATOR_SQUARE;  
            modulator->process = this_square_;      
        }
        return;                       
    }
    }      
}

void GAPIENTRY effect_get_float(geffect* effect,int32_t flag,float* value)
{  
    geffectimpl* modulator = effect->impl;
    if(!modulator)
        return;
    switch(flag)
    {
    case AUDIO_EFFECT_MODULATOR_FREQUENCY:
        *value = modulator->samplerate; 
        return;
    case AUDIO_EFFECT_MODULATOR_HIGHPASS_CUTOFF:
        *value = modulator->cutoff; 
        return;
    case AUDIO_EFFECT_MODULATOR_WAVEFORM:
        *value = modulator->waveform; 
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
    effect->impl = new geffectimpl();
    return effect;          
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{             
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new gmodulatoreffectcreator();
    return &addon;
}






