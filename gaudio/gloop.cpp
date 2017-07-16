#include "gcontext.h"
#include "gpanprocess.h"
#include "gmain.h"
#include "gmixer.h"
#include "bs2b.h"

void gprocess();

uint32_t gloop(void* ptr)
{
    while(scontext.killed)
    {
        gprocess();
#if(defined(WIN32) || defined(WIN64))
        //Sleep(50);
#endif

    }
    return 0;
}

void gprocess()
{
    gdecprocess();

    gmixer();

    scontext.device.applyeffect();

    //gscoperlocker locker(&scontext.locker);

    bool p = panprocess();
    if(!p)
    {
        GAIN_PROCESS(false,-1,1.0f)
    }

    uint32_t length = scontext.device.frontbuffer.use;
    int16_t* buffer = scontext.device.backbuffer.buffer;

    if(length != 0)
    {
        /*if(AUDIO_CHANNEL_CUR == 2 && scontext.bs2bptr != 0)
        {
            for(int32_t i=0;i<scontext.device.frontbuffer.use;i++)
            {
                float units[] = {scontext.device.frontbuffer.data[0][i],scontext.device.frontbuffer.data[1][i]};
                bs2b_cross_feed(scontext.bs2bptr,units);
                scontext.device.frontbuffer.data[0][i] = units[0];
                scontext.device.frontbuffer.data[1][i] = units[1];
            }
        }*/

        gjoin(scontext.device.frontbuffer,scontext.device.backbuffer.buffer,length);
        if(scontext.device.mode == AUDIO_PLAY_MODE && scontext.driver && !scontext.device.mute)
        {
            scontext.driver->play(scontext.driver,(char*)buffer,length*4);
        }
        else if(scontext.device.mode == AUDIO_RECORD_MODE &&
            scontext.recorder != NULL &&
            scontext.recorder->state == RECORDER_STATE_WRITE)
        {
            scontext.recorder->write(scontext.recorder,buffer,length*2);
        }
        else if(scontext.device.mode == AUDIO_RECORD_MODE &&
                scontext.recorder != NULL &&
                scontext.recorder->state != RECORDER_STATE_PAUSE)
        {
        }
        else
        {
            gerror_set(AUDIO_INNER_ERROR);
        }
    }

#if(defined(WIN32) || defined(_WIN64))
    Sleep(50);
#else
#endif
}

