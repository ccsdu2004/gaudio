#include "gaudio.h"
#include "gcontext.h"
#include "gcatcher.h"

/*
grecorder* gaudio_recorder_create(const char* file,uint32_t audio)
{
    if(sContext.recorder != NULL)
    {
        gerror_set(AUDIO_BAD_OPERATION);
        return NULL;
    }

    sContext.recorder = sContext.factory.recorderfactory.create_by_id(audio,0);
    if(!sContext.recorder)
    {
        gerror_set(AUDIO_NOT_SUPPORT);
        return NULL;
    }

    sContext.recorder = sContext.recorder->create(sContext.recorder,file,AUDIO_CHANNEL_CUR,AUDIO_STANDARD_SAMPLERATE,16);
    sContext.recorder->state = RECORDER_STATE_CREATE;
    return sContext.recorder;
}

void gaudio_recorder_write(grecorder* recorder)
{
    if(recorder != NULL)
    {
        if(recorder != sContext.recorder)
            gerror_set(AUDIO_BAD_VALUE);
        else if(sContext.recorder->state != RECORDER_STATE_CREATE &&
                sContext.recorder->state != RECORDER_STATE_PAUSE)
        {
            gerror_set(AUDIO_BAD_OPERATION);
        }
        else
        {
            sContext.recorder->state = RECORDER_STATE_WRITE;
        }
    }
    else
        gerror_set(AUDIO_BAD_VALUE);
}

void gaudio_recorder_pause(grecorder* recorder)
{
    if(recorder != NULL)
    {
        if(recorder != sContext.recorder)
            gerror_set(AUDIO_BAD_VALUE);
        else if(sContext.recorder->state != RECORDER_STATE_WRITE)
            gerror_set(AUDIO_BAD_OPERATION);
        else
        {
            sContext.recorder->state = RECORDER_STATE_PAUSE;
        }
    }
    else
        gerror_set(AUDIO_BAD_VALUE);
}

void gaudio_recorder_stop(grecorder* recorder)
{
    if(recorder != NULL)
    {
        if(recorder != sContext.recorder)
            gerror_set(AUDIO_BAD_VALUE);
        else if(sContext.recorder->state != RECORDER_STATE_WRITE)
            gerror_set(AUDIO_BAD_OPERATION);
        else
        {
            sContext.recorder->close(sContext.recorder);
            sContext.recorder->state = RECORDER_STATE_CLOSE;
        }
    }
    else
        gerror_set(AUDIO_BAD_VALUE);
}

void gaudio_recorder_destroy(grecorder* recorder)
{
    if(recorder != NULL)
    {
        if(recorder != sContext.recorder)
            gerror_set(AUDIO_BAD_VALUE);
        else
        {
            sContext.recorder->destroy(sContext.recorder);
            sContext.recorder = NULL;
        }
    }
    else
        gerror_set(AUDIO_BAD_VALUE);
}
*/
