#include <cstdio>
#include <cstdlib>
#include "gaudio.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/grecorder.h"
#include "../gaudio/gmain.h"
#include "encode/lib_Encoder.h"
#include "encode/lib_Sndfile.h"
#include "libaudio.h"

grecreator(rec)

grecorder* recoder_creator(int32_t id);

int32_t grecgrecordercreator::is_support(int32_t id,int32_t mask)
{
    int32_t c = getSndfileType(id);
    return c != -1;
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
    return recorder->impl->lib->write((unsigned char*)buffer,length*sizeof(short));
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
    if(getSndfileType(enc) != -1)
        return new lib_Sndfile(enc);
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




