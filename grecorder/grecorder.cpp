#include <cstdio>
#include <cstdlib>
#include "gaudio.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/grecorder.h"
#include "../gaudio/gmain.h"
#include "encode/lib_Encoder.h"
#include "encode/lib_Sndfile.h"
#include "encode/lib_Twolame.h"
#include "encode/lib_Lame.h"
#include "encode/lib_Ape.h"
#include "encode/lib_Aac.h"
#include "encode/lib_Flac.h"
#include "encode/wavpack_Encoder.h"
#include "libaudio.h"

grecreator(rec)

grecorder* recoder_creator(int32_t id);

int32_t grecgrecordercreator::is_support(int32_t id,int32_t mask)
{
    if(id == AUDIO_FORMAT_AIFF ||
       id == AUDIO_FORMAT_AU   ||
       id == AUDIO_FORMAT_WAV  ||
       id == AUDIO_FORMAT_OGG  ||
       id == AUDIO_FORMAT_VOC  ||
       id == AUDIO_FORMAT_FLAC ||
       id == AUDIO_FORMAT_CAF  ||
       id == AUDIO_FORMAT_MP2  ||
       id == AUDIO_FORMAT_MP3  ||
       id == AUDIO_FORMAT_APE ||
       id == AUDIO_FORMAT_AAC ||
       id == AUDIO_FORMAT_WV)
        return TRUE;
    return FALSE;
}

grecorder* grecgrecordercreator::create_by_id(int32_t id,int32_t mask)
{
    return recoder_creator(id);
}

void grecgrecordercreator::destroy()
{
    delete this;
}

struct grecorderimpl
{
    lib_Encoder* lib;
};

grecorder* GAPIENTRY recorder_create_callback(grecorder* recorder,
const char* output,uint32_t channel,uint32_t samplerate,uint32_t format)
{
    recorder->impl->lib->open(output,channel,samplerate,format);
    return recorder;
}

uint32_t GAPIENTRY recorder_write_callback(grecorder* recorder,int16_t* buffer,uint32_t length)
{
    return recorder->impl->lib->write((unsigned char*)buffer,length*2);
}

uint32_t GAPIENTRY recorder_close_callback(grecorder* recorder)
{
     recorder->impl->lib->close();
     return TRUE;
}

void GAPIENTRY recorder_destroy_callback(grecorder* recorder)
{
     delete recorder->impl->lib;
}

lib_Encoder* get_encoder_impl(int32_t enc)
{
    if(enc == AUDIO_FORMAT_AIFF ||
       enc == AUDIO_FORMAT_AU   ||
       enc == AUDIO_FORMAT_WAV  ||
       enc == AUDIO_FORMAT_OGG  ||
       enc == AUDIO_FORMAT_VOC  ||
       enc == AUDIO_FORMAT_FLAC ||
       enc == AUDIO_FORMAT_CAF)
        return new lib_Sndfile(enc);
    else if(enc == AUDIO_FORMAT_MP2)
        return new lib_Twolame();
    else if(enc == AUDIO_FORMAT_MP3)
        return new lib_Lame();
    //else if(enc == AUDIO_FORMAT_APE)
    //    return new lib_Ape();
    else if(enc == AUDIO_FORMAT_AAC)
        return new lib_Aac();
    else if(enc == AUDIO_FORMAT_WV)
        return new wavpack_Encoder();
    return NULL;
}

grecorder* recoder_creator(int32_t enc)
{
    lib_Encoder* lib = get_encoder_impl(enc);
    if(!lib)
        return NULL;
    grecorder* recorder = new grecorder();
    recorder->impl = new grecorderimpl();
    recorder->impl->lib = lib;
    recorder->create = recorder_create_callback;
    recorder->write = recorder_write_callback;
    recorder->close = recorder_close_callback;
    recorder->destroy = recorder_destroy_callback;
    return recorder;
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_WRITER;
    addon.addon = new grecgrecordercreator();
    return &addon;
}




