#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gmemory.h"
#include "gcontext.h"
#include "gstring.h"
#include "gsource.h"
#include "gdecoder.h"
#include "streamBuffer.h"
#include "gresample.h"
#include "gmain.h"
#include "gmath.h"

gsource_::gsource_():
is3d(false),
state(STATE_IDLE),
state_old(STATE_IDLE),
decoder(NULL),
finish(NULL),
error(NULL),
position(NULL),
loop(FALSE),
pan(AUDIO_PAN_DEFAULT),
gain(0.8f),
curpos(0),
mute(FALSE),
user(0)
{
    pos[0] = pos[1] = pos[2] = .0f;
    velocity[0] = velocity[1] = velocity[2] = .0f;
    orientation[0] = orientation[1] = orientation[2] = .0f;

    update = false;

    picth = 1.0f;
	outgain = .0f;
	mingain = .0f;
	maxgain = 1.0f;

    inangle = outangle = 360.0f;
    distance = 1.0f;
    refdistance = 1.0f;
    maxdistance = FLT_MAX;
    rolloff = 1.0f;

    int32_t i=0;
    for(;i<AUDIO_CHANNEL_CUR;i++)
       frontbuffer.data[i] = gcreate_buffer<float>(BUFFER_SIZE/2);
    frontbuffer.length = BUFFER_SIZE/2;
}

gsource_::~gsource_()
{
    int32_t i=0;
    for(;i<AUDIO_CHANNEL_CUR;i++)
    {
        gFreeBuffer(frontbuffer.data[i]);
    }
}

void gsource_::init_state(int32_t state_)
{
    state = state_old = state_;
}

void gsource_::applyeffect()
{
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

void gsource_::applystate(int32_t state)
{
    //gscoperlocker locker(&sContext.locker);
    state_old = this->state;
    this->state = state;
}
