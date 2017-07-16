#include <string.h>
#include "gaudio.h"
#include "gmath.h"
#include "gmain.h"
#include "gcontext.h"
#include "gaddons.h"

void GAPIENTRY gexitcallback()
{
    gstopthread(scontext.thread);
    printf("12\n");
    if(scontext.driver)
    {
        printf("123\n");
        scontext.driver->close(scontext.driver);
        printf("124\n");
        scontext.driver->destroy(scontext.driver);
        printf("125\n");
        scontext.driver = 0;
        printf("126\n");
    }
}

void gaudio_init(const char* addons)
{
    if(scontext.killed == 1)
    {
        gerror_set(AUDIO_INVALID_OPERATION);
        return;
    }
    scontext.killed = 1;
	scontext.lasterror = AUDIO_NO_ERROR;
	gloadaddons(addons,FALSE);
	scontext.thread = gstartthread(gloop,NULL);
}

void gaudio_deinit()
{
    scontext.killed = 0;
    gexitcallback();
}

int32_t gaudio_is_support(int32_t flag,int32_t mask)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return FALSE;
    }

    if(flag == AUDIO_PLUGIN_AUDIO)
    {
        if(mask == AUDIO_FORMAT_RAW)
            return TRUE;
        return scontext.factory.decoderfactory.is_support(mask,0);
    }
    else if(flag == AUDIO_PLUGIN_EFFECT)
    {
        return scontext.factory.effectfactory.is_support(mask,0);
    }
    else if(flag == AUDIO_PLUGIN_DRIVER)
    {
        return scontext.driver->id == mask;
    }
    else if(flag == AUDIO_PLUGIN_WRITER)
    {
        return scontext.factory.writerfactory.is_support(mask);
    }

    return FALSE;
}

void gaudio_set_float(int32_t flag,float value)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return;
    }

    //gcontext* context = getCurrentContext();
    //gscoperlocker locker(&context->locker);

    if(flag == AUDIO_ENUM_GAIN)
    {
        scontext.device.gain = gclamp(value,0.0f,1.0f);
    }
    else if(flag == AUDIO_ENUM_SOUND_SPEED)
    {
        if(value > 0.0f)
            scontext.device.soundspeed = value;
        else
            gerror_set(AUDIO_BAD_VALUE);
    }
    else if(flag == AUDIO_ENUM_PAN)
    {
        value = gclamp(value,AUDIO_PAN_LOW,AUDIO_PAN_HIGH);
        scontext.device.pan = value;
    }
    else
        gerror_set(AUDIO_BAD_TAG);
}

void gaudio_get_float(int32_t flag,float* value)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return;
    }

    if(flag == AUDIO_ENUM_GAIN)
        *value = scontext.device.gain;
    else if(flag == AUDIO_ENUM_SOUND_SPEED)
        *value = scontext.device.soundspeed;
    else
        gerror_set(AUDIO_BAD_TAG);
}

void gaudio_set_int32(int32_t flag,int32_t value)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return;
    }

    //gscoperlocker locker(&scontext.locker);

    if(flag == AUDIO_PLAY_MODE)
    {
        scontext.device.mode = value ? AUDIO_PLAY_MODE : AUDIO_RECORD_MODE;
    }
    else if(flag == AUDIO_RECORD_MODE)
    {
        if(value == TRUE)
           scontext.device.mode = AUDIO_RECORD_MODE;
        else
           gerror_set(AUDIO_INVALID_OPERATION);
    }
    else if(flag == AUDIO_ENUM_MUTE)
    {
        scontext.device.mute = value == 0 ? FALSE : TRUE;
    }
    else if(flag == AUDIO_ENUM_ATTENUATION)
    {
        scontext.device.attenuation = gclamp<int32_t>(value,AUDIO_ATTENUATION_MIN,AUDIO_ATTENUATION_MAX);
    }
    else
        gerror_set(AUDIO_BAD_TAG);
}

void gaudio_get_int32(int32_t flag,int32_t* value)
{
    if(scontext.killed!=1)
        gerror_set(AUDIO_NOT_INIT);
    if(flag == AUDIO_PLAY_MODE)
        *value = scontext.device.mode == AUDIO_PLAY_MODE ? TRUE : FALSE;
    else if(flag == AUDIO_RECORD_MODE)
        *value = scontext.device.mode == AUDIO_RECORD_MODE ? TRUE : FALSE;
    else if(flag == AUDIO_ENUM_VERSION)
        *value = GAUDIO_ADUIO_VERSION;
    else if(flag == AUDIO_ENUM_DRIVER)
    {
        if(scontext.driver_create && scontext.driver)
            *value = scontext.driver->id;
        else
        {
            *value = AUDIO_DRIVER_NULL;
            gerror_set(AUDIO_BAD_VALUE);
        }
    }
    else if(flag == AUDIO_ENUM_MUTE)
        *value = scontext.device.mute;
    else if(flag == AUDIO_ENUM_ATTENUATION)
        *value = scontext.device.attenuation;
    else
        gerror_set(AUDIO_BAD_TAG);
}

void gaudio_set_string(int32_t flag,const char* str)
{
    //gscoperlocker locker(&scontext.locker);

    /*if(flag == AUDIO_ENUM_SOUNDFONT)
    {
        if(str == 0 || strlen(str) == 0)
        {
            gerror_set(AUDIO_BAD_VALUE);
            return;
        }
        strcpy(sContext.soundfont,str);
        #ifdef HAVE_MIDI
        gaddons* addon = gmidi_instance(0);
        if(addon)
            gsetaddon(NULL,addon,0);
        #endif
    }
    else*/
    //gerror_set(AUDIO_BAD_TAG);
}

int32_t gaudio_get_string(int32_t flag,char* str)
{
    /*if(flag == AUDIO_ENUM_SOUNDFONT)
    {
        strcpy(str,sContext.soundfont);
        return TRUE;
    }
    gerror_set(AUDIO_BAD_TAG);*/
    return FALSE;
}
