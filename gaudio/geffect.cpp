#include "gcontext.h"
#include "geffect.h"

geffect* gaudio_effect_create(int32_t id,int32_t mask)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return NULL;
    }
    return scontext.factory.effectfactory.create_by_id(id,mask);
}

void gaudio_effect_destroy(geffect* effect)
{
    if(scontext.killed!=1)
        gerror_set(AUDIO_NOT_INIT);
    else if(!effect)
        gerror_set(AUDIO_BAD_VALUE);
    else
        effect->destroy(effect);
}

void gaudio_effect_bind(geffect* effect,int32_t order)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return;
    }

    if(effect == NULL)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }

    if(effect->source != NULL || effect_support_global(effect) == FALSE || effect->binded)
    {
        gerror_set(AUDIO_INVALID_OPERATION);
        return;
    }

    if(scontext.device.effect.size() > AUDIO_MAX_EFFECT)
    {
        gerror_set(AUDIO_NOT_SUPPORT);
        return;
    }

    //gscoperlocker locker(&scontext.locker);

    scontext.device.effect.push_back(effect);

    effect->binded = TRUE;
    effect->order = order;
    effect->source = 0;
}

void gaudio_effect_bind_to_source(geffect* effect,gsource* source)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return;
    }

    if(effect == NULL || source == NULL)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }

    if(effect_support_source(effect) == FALSE || effect->binded)
    {
        gerror_set(AUDIO_INVALID_OPERATION);
        return;
    }

    if(source->effect.size()>MAX_EFFECT_BIND_TO_SOURCE)
    {
        gerror_set(AUDIO_NOT_SUPPORT);
        return;
    }

    //gscoperlocker locker(&scontext.locker);

    effect->binded = TRUE;
    effect->source = source;
    source->effect.push_back(effect);
}

void gaudio_effect_unbind(geffect* effect)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return;
    }

    if(effect == NULL)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }

    if(effect->binded != TRUE || effect->source)
    {
        gerror_set(AUDIO_INVALID_OPERATION);
        return;
    }

    //gscoperlocker locker(&scontext.locker);

    scontext.device.effect.remove(effect);
    effect->binded = FALSE;
}

void gaudio_effect_unbind_from_source(geffect* effect,gsource* source)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return;
    }

    if(effect == NULL)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }

    if(effect->binded == FALSE || effect->source != source)
    {
        gerror_set(AUDIO_INVALID_OPERATION);
        return;
    }

    //gscoperlocker locker(&scontext.locker);

    source->effect.remove(effect);
    effect->source = 0;
}

void gaudio_effect_set_int32(geffect* effect,int32_t flag,int32_t value)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return;
    }

    if(effect == NULL)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }

    //gscoperlocker locker(&scontext.locker);

    if(flag == AUDIO_EFFECT_ENABLE)
        effect->state = value > 0 ? TRUE : FALSE;
    else if(effect->setint)
    {
        effect->setint(effect,flag,value);
    }
    else
        gerror_set(AUDIO_BAD_TAG);
}

void gaudio_effect_get_int32(geffect* effect,int32_t flag,int32_t* value)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return;
    }

    if(effect == NULL)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }

    //gscoperlocker locker(&scontext.locker);

    if(flag == AUDIO_EFFECT_ENABLE)
        *value = effect->state;
    else if(effect->getint)
    {
        effect->getint(effect,flag,value);
    }
    else
        gerror_set(AUDIO_BAD_TAG);
}

void gaudio_effect_set_fft_callback(geffect* effect,gefxcb cb)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return;
    }

    if(effect == NULL || cb == NULL)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }

    //gscoperlocker locker(&scontext.locker);

    effect->fftcb = cb;
}

void gaudio_effect_set_float(geffect* effect,int32_t flag,float value)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return;
    }

    if(effect == NULL)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }

    //gscoperlocker locker(&scontext.locker);

    if(effect->setfloat)
        effect->setfloat(effect,flag,value);
}

void gaudio_effect_get_float(geffect* effect,int32_t flag,float* value)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return;
    }

    if(effect == NULL)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }

    if(effect->getfloat)
        effect->getfloat(effect,flag,value);
}
