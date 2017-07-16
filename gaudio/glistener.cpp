#include "gcontext.h"
#include "glistener.h"

glistener::glistener()
{
    for(int i=0;i<3;i++)
    {
        pos[i] = vel[i] = up[i] = 0.0f;
    }
    up[1] = 1.0f;
    gain = 1.0f;
    mpu = 1.0f;
    update = FALSE;
}

glistener* gaudio_listener_get_instance()
{
    if(scontext.killed == 1)
    {
        return &scontext.device.listener;
    }
    else
        return NULL;
}

void gaudio_listener_set_float3(int32_t flag,float x,float y,float z)
{
    if(scontext.killed == 0)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }

    //gscoperlocker locker(&scontext.locker);

    switch(flag)
    {
       case AUDIO_ENUM_POSITION:
          scontext.device.listener.pos[0] = x;
          scontext.device.listener.pos[1] = y;
          scontext.device.listener.pos[2] = z;
          scontext.device.listener.update = TRUE;
          break;
       case AUDIO_ENUM_VELOCITY:
          scontext.device.listener.vel[0] = x;
          scontext.device.listener.vel[1] = y;
          scontext.device.listener.vel[2] = z;
          scontext.device.listener.update = TRUE;
          break;
       case AUDIO_ENUM_FORWARD:
          scontext.device.listener.forward[0] = x;
          scontext.device.listener.forward[1] = y;
          scontext.device.listener.forward[2] = z;
          scontext.device.listener.update = TRUE;
          break;
       case AUDIO_ENUM_UP:
          scontext.device.listener.up[0] = x;
          scontext.device.listener.up[1] = y;
          scontext.device.listener.up[2] = z;
          scontext.device.listener.update = TRUE;
          break;
       default:
          scontext.lasterror = AUDIO_BAD_TAG;
    }
}

void gaudio_listener_get_float3(int32_t flag,float* x,float* y,float* z)
{
    if(scontext.killed == 0)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }

    switch(flag)
    {
       case AUDIO_ENUM_POSITION:
          *x = scontext.device.listener.pos[0];
          *y = scontext.device.listener.pos[1];
          *z = scontext.device.listener.pos[2];
          break;
       case AUDIO_ENUM_VELOCITY:
          *x = scontext.device.listener.vel[0];
          *y = scontext.device.listener.vel[1];
          *z = scontext.device.listener.vel[2];
          break;
       case AUDIO_ENUM_UP:
          *x = scontext.device.listener.up[0];
          *y = scontext.device.listener.up[1];
          *z = scontext.device.listener.up[2];
          break;
       default:
          scontext.lasterror = AUDIO_BAD_TAG;
    }
}





