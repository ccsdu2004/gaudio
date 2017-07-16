#include "gmain.h"
#include "gmemory.h"
#include "gcontext.h"

gdevice::gdevice()
{
    swap = 0;
    mode = AUDIO_PLAY_MODE;
    gain = 1.0f;
    pan = AUDIO_PAN_DEFAULT;
    soundspeed = AUDIO_SOUND_SPEED_DEFAULT;
    mute = FALSE;

    attenuation = AUDIO_ATTENUATION_INVERSE_DISTANCE_CLAMPED;
    dopplerfactor = 1.0f;
    dopplervelocity = 1.0f;

    uint32_t i=0;
    for(;i<AUDIO_CHANNEL_CUR;i++)
       frontbuffer.data[i] = gcreate_buffer<float>(BUFFER_SIZE);
    frontbuffer.length = BUFFER_SIZE;

    backbuffer.buffer = gcreate_buffer<int16_t>(BUFFER_SIZE);
    backbuffer.use = 0;
}

gdevice::~gdevice()
{
    printf("gdevice1.\n");
    uint32_t i=0;
    for(;i<AUDIO_CHANNEL_CUR;i++)
        gFreeBuffer(frontbuffer.data[i]);
    gFreeBuffer(backbuffer.buffer);
    printf("gdevice2.\n");
}

void gdevice::applyeffect()
{
    //gscoperlocker locker(&scontext.locker);

    std::list<geffect*>::iterator itr = effect.begin();
    while(itr != effect.end())
    {
        geffect* effect = *itr;
        if(effect && effect->process && effect->state)
        {
            effect->process(effect,frontbuffer);
        }
        itr ++;
    }
}

