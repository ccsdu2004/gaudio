#include "gmain.h"

int32_t check_valid_format(int32_t format,int32_t channel,int32_t samplerate)
{
    if(format != 8 && format != 16 && format != 24 && format != 32)
        return FALSE;
    if(samplerate < 8000 || samplerate > 48000)
        return FALSE;
    if(channel != 1 && channel != 2)
        return FALSE;
    return TRUE;
}
