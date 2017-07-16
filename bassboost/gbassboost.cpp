#include <stdio.h>
#include <stdlib.h>
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmain.h"
#include "../gaudio/gmath.h"

geffect* geffect_create();

gecreator(bassboost)

int32_t gbassboosteffectcreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_BASSBOOST)
       return TRUE;
    return FALSE;
}

geffect* gbassboosteffectcreator::create_by_id(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_BASSBOOST)
       return geffect_create();
    return NULL;
}

void gbassboosteffectcreator::destroy()
{
    delete this;
}

struct bassboost
{
    float frequency;
    float boost;
    float xn1,xn2,yn1,yn2;
    float omega,sn,cs,a,shape,beta,b0,b1,b2,a0,a1,a2;
};

struct geffectimpl
{
    bassboost left;
    bassboost right;
};

inline void effect_init(geffectimpl* boost)
{
    boost->left.frequency = 200.0f;
    boost->right.frequency = 200.0f;
    boost->left.boost = 12.0f;
    boost->right.boost = 12.0f;
}

inline void effect_reset(bassboost* boost)
{
    boost->xn1 = 0;
    boost->xn2 = 0;
    boost->yn1 = 0;
    boost->yn2 = 0;

    boost->omega = 2 * 3.141592653589 * boost->frequency/AUDIO_STANDARD_SAMPLERATE;
    boost->sn = sin(boost->omega);
    boost->cs = cos(boost->omega);
    boost->a = exp(log(10.0f)*boost->boost/40.0f);
    boost->shape = 1.0f;
    boost->beta = sqrt((boost->a*boost->a + 1)/boost->shape-(pow((boost->a-1),2)));

    boost->b0 = boost->a*((boost->a+1)-(boost->a-1)*boost->cs+boost->beta*boost->sn);
    boost->b1 = 2 * boost->a * ((boost->a - 1) - (boost->a + 1) * boost->cs);
    boost->b2 = boost->a * ((boost->a + 1) - (boost->a - 1) * boost->cs - boost->beta * boost->sn);
    boost->a0 = ((boost->a + 1) + (boost->a - 1) * boost->cs + boost->beta * boost->sn);
    boost->a1 = -2 * ((boost->a - 1) + (boost->a + 1) * boost->cs);
    boost->a2 = (boost->a + 1) + (boost->a - 1) * boost->cs - boost->beta * boost->sn;
}

char* effect_maker(geffect*)
{
    return "duwenhua2013";
}

void effect_destroy(geffect* effect)
{
    delete effect->impl;
    effect->impl = 0;
    effect->destroy(effect);
}

inline float effect_process_impl(bassboost* boost,float v)
{
    float out = (boost->b0 * v + boost->b1 * boost->xn1 +
        boost->b2 * boost->xn2 - boost->a1 * boost->yn1 - boost->a2 * boost->yn2) / boost->a0;
    boost->xn2 = boost->xn1;
    boost->xn1 = v;
    boost->yn2 = boost->yn1;
    boost->yn1 = out;
    if(out < -1.0f)
        out = -1.0f;
    else if(out > 1.0f)
        out = 1.0f;
    return out;
}

void GAPIENTRY effect_process(geffect* effect,gdatabuffer<float>& buffer)
{
    geffectimpl* boost = effect->impl;
    if(effect->update)
    {
        effect->update = FALSE;
        effect_reset(&boost->left);
        effect_reset(&boost->right);
    }
    for(int32_t i=0;i<buffer.use;i++)
    {
        buffer.data[0][i] = effect_process_impl(&boost->left,buffer.data[0][i]);
        buffer.data[1][i] = effect_process_impl(&boost->right,buffer.data[1][i]);
    }
}

void GAPIENTRY effect_set_float(geffect* effect,int32_t flag,float value)
{
    if(flag != AUDIO_EFFECT_BASSBOOST_FREQUENCE || AUDIO_EFFECT_BASSBOOST_GAIN != flag)
        return;
    geffectimpl* boost = effect->impl;
    switch(flag)
    {
    case AUDIO_EFFECT_BASSBOOST_FREQUENCE:
         boost->left.frequency = boost->right.frequency = gclamp(value,AUDIO_EFFECT_BASSBOOST_FREQUENCE_LOW,AUDIO_EFFECT_BASSBOOST_FREQUENCE_HIGH);
         effect->update = TRUE;
         break;
    case AUDIO_EFFECT_BASSBOOST_GAIN:
         boost->left.boost = boost->right.boost = gclamp(value,AUDIO_EFFECT_BASSBOOST_GAIN_LOW,AUDIO_EFFECT_BASSBOOST_GAIN_HIGH);
         effect->update = TRUE;
         break;
    default:
         return;
    }
}

void GAPIENTRY effect_get_float(geffect* effect,int32_t flag,float* value)
{
    if(flag != AUDIO_EFFECT_BASSBOOST_FREQUENCE || AUDIO_EFFECT_BASSBOOST_GAIN != flag)
        return;
    geffectimpl* boost = effect->impl;
    switch(flag)
    {
    case AUDIO_EFFECT_BASSBOOST_FREQUENCE:
         *value = boost->left.frequency;
         break;
    case AUDIO_EFFECT_BASSBOOST_GAIN:
         *value = boost->left.boost;
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
	effect->process = effect_process;
    effect->order = -1;
    effect->source = 0;
    effect->destroy = effect_destroy;
    effect->setfloat = effect_set_float;
    effect->getfloat = effect_get_float;
    effect->setint = 0;
    effect->getint = 0;
    effect->state = TRUE;

    geffectimpl* boost = new geffectimpl();
    effect_init(boost);
    effect_reset(&boost->left);
    effect_reset(&boost->right);
    effect->impl = boost;
    return effect;
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new gbassboosteffectcreator();
    return &addon;
}






