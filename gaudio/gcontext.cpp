#include <memory.h>
#include "gaudio.h"
#include "bs2b.h"
#include "gmain.h"
#include "gcontext.h"
#include "gmemory.h"

gcontext scontext;

gcontext::gcontext()
{
    killed = 0;
    lasterror = AUDIO_NO_ERROR;

    bs2bptr = (bs2b*)malloc(sizeof(bs2b));
    if(bs2bptr)
    {
        bs2b_set_srate(bs2bptr,AUDIO_STANDARD_SAMPLERATE);
        bs2b_set_level(bs2bptr,BS2B_DEFAULT_CLEVEL);
    }
    driver = NULL;
    driver_create = 0;
    thread = NULL;
}

gcontext::~gcontext()
{
    if(bs2bptr)
        free(bs2bptr);
    bs2bptr = 0;
}
