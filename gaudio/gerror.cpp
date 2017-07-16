#include "gmain.h"
#include "gcontext.h"

void gerror_set(int32_t error)
{
    if(scontext.lasterror == AUDIO_NO_ERROR)
		scontext.lasterror = error;
}

int32_t gaudio_error_get()
{
    int32_t error;

    if(&scontext)
    {
        error = scontext.lasterror;
        scontext.lasterror = AUDIO_NO_ERROR;
    }
    else
        error = AUDIO_INVALID_OPERATION;
    return error;
}
