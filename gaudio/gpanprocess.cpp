#include "gpanprocess.h"
#include "gcontext.h"
#include "gmain.h"

bool panprocess()
{
    if(gisequal(scontext.device.pan,AUDIO_PAN_DEFAULT))
    {
        return false;
    }

    float pan = scontext.device.pan;
    float newgain = AUDIO_PAN_HIGH - fabs(pan);
    newgain /= AUDIO_PAN_HIGH;
    GAIN_PROCESS(true,pan,newgain)
    return true;
}
