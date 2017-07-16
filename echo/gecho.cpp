#include <stdio.h>
#include <stdlib.h>
#include "../gaudio/gaddons.h"
#include "../gaudio/geffect.h"
#include "../gaudio/gmath.h"

typedef void*(*gqueue_create__ptr)(int32_t);
typedef void (*gqueue_destroy__ptr)(void*);
typedef void (*gqueue_resize__ptr)(void*,int32_t);
typedef int32_t(*gqueue_process__ptr)(void*,float*,int32_t,float*);

static gqueue_create__ptr  gqueue_create_ptr = 0;
static gqueue_destroy__ptr gqueue_destroy_ptr = 0;
static gqueue_resize__ptr  gqueue_resize_ptr = 0;
static gqueue_process__ptr gqueue_process_ptr = 0;

geffect* geffect_create();

gecreator(echo)

int32_t gechoeffectcreator::is_support(int32_t efx,int32_t mask)
{
    if(efx == AUDIO_EFFECT_ECHO)
       return TRUE;
    return FALSE;
}

geffect* gechoeffectcreator::create_by_id(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_EFFECT_ECHO)
       return geffect_create();
    return NULL;
}

void gechoeffectcreator::destroy()
{
    delete this;
}

class geffectimpl
{
public:
    geffectimpl();
    ~geffectimpl();
public:
    void  set_delay(float delay){this->delay = delay;}
    void  set_decay(float decay){this->decay = decay;}
    float get_delay(){return delay;}
    float get_decay(){return decay;}
    void  update();
    void  close();
    int32_t process(int32_t channel,float* in,int32_t len,float* out);
    float* buffer;
private:
    float delay;
    float decay;
    void* left;
    void* right;
};

geffectimpl::geffectimpl():
delay(AUDIO_EFFECT_ECHO_DELAY_DEFAULT),
decay(AUDIO_EFFECT_ECHO_DECAY_DEFAULT)
{
    int32_t length = delay*AUDIO_STANDARD_SAMPLERATE;
    left  = gqueue_create_ptr(length);
    right = gqueue_create_ptr(length);
    buffer = new float[BUFFER_SIZE];
}

geffectimpl::~geffectimpl()
{
    close();
    delete []buffer;
}

void geffectimpl::close()
{
    gqueue_destroy__ptr(left);
    left = 0;
    gqueue_destroy__ptr(right);
    right = 0;
}

void geffectimpl::update()
{
    int length = delay*AUDIO_STANDARD_SAMPLERATE;
    gqueue_resize_ptr(left,length);
    gqueue_resize_ptr(right,length);
}

int32_t geffectimpl::process(int32_t index,float* in,int32_t len,float* out)
{
    if(index == 1)
       return gqueue_process_ptr(left,in,len,out);
    else if(index == 2)
       return gqueue_process_ptr(right,in,len,out);
    return 0;
}

inline void effect_init(geffectimpl* echo)
{
    echo->set_delay(AUDIO_EFFECT_ECHO_DELAY_DEFAULT);
    echo->set_decay(AUDIO_EFFECT_ECHO_DECAY_DEFAULT);
}

char* effect_maker(geffect*)
{
    return "http://www.geek-audio.org/";
}

void effect_destroy(geffect* effect)
{
    delete effect->impl;
    effect->impl = 0;
    effect->destroy(effect);
}

void GAPIENTRY effect_process(geffect* effect,gdatabuffer<float>& buffer)
{
    geffectimpl* echo = effect->impl;
    if(effect->update)
    {
        effect->update = FALSE;
        echo->update();
    }

    memset(echo->buffer,0,BUFFER_SIZE*sizeof(float));
    int32_t read = echo->process(1,buffer.data[0],buffer.use,echo->buffer);

    float decay = echo->get_decay();
    for(int32_t i=0;i<read;i++)
    {
        if(i%2==0)
             buffer.data[0][buffer.use-read+i] = decay*echo->buffer[i];
    }

    memset(echo->buffer,0,BUFFER_SIZE*sizeof(float));
    read = echo->process(2,buffer.data[1],buffer.use,echo->buffer);
    for(int32_t i=0;i<read;i++)
    {
        if(i%2==1)
           buffer.data[1][buffer.use-read+i] = decay*echo->buffer[i];
    }
}

void GAPIENTRY effect_set_float(geffect* effect,int32_t flag,float value)
{
    geffectimpl* echo = effect->impl;
    switch(flag)
    {
    case AUDIO_EFFECT_ECHO_DELAY:
         echo->set_delay(gclamp(value,AUDIO_EFFECT_ECHO_DELAY_MIN,AUDIO_EFFECT_ECHO_DELAY_MAX));
         effect->update = TRUE;
         break;
    case AUDIO_EFFECT_ECHO_DECAY:
         echo->set_decay(gclamp(value,AUDIO_EFFECT_ECHO_DECAY_MIN,AUDIO_EFFECT_ECHO_DECAY_MAX));
         break;
    default:
         return;
    }
}

void GAPIENTRY effect_get_float(geffect* effect,int32_t flag,float* value)
{
    geffectimpl* echo = effect->impl;
    switch(flag)
    {
    case AUDIO_EFFECT_ECHO_DELAY:
         *value = echo->get_delay();
         break;
    case AUDIO_EFFECT_ECHO_DECAY:
         *value = echo->get_decay();
         break;
    default:
         return;
    }
}

geffect* geffect_create()
{
    geffect* effect = new geffect();
    effect->mask = EFFECT_SUPPORT_BIND_TO_GLOBAL | EFFECT_SUPPORT_BIND_TO_SOURCE;
    effect->binded = FALSE;
    effect->fftcb = 0;
    effect->process = effect_process;
    effect->order = -1;
    effect->source = 0;
    effect->destroy = effect_destroy;
    effect->setfloat = effect_set_float;
    effect->getfloat = effect_get_float;
    effect->setint = 0;
    effect->getint = 0;
    effect->state = TRUE;

    geffectimpl* echo = new geffectimpl();
    effect_init(echo);
    effect->impl = echo;
    return effect;
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    gqueue_create_ptr = (gqueue_create__ptr)gaudio_address_get("gqueue_create");
    gqueue_destroy_ptr = (gqueue_destroy__ptr)gaudio_address_get("gqueue_destroy");
    gqueue_resize_ptr = (gqueue_resize__ptr)gaudio_address_get("gqueue_resize");
    gqueue_process_ptr = (gqueue_process__ptr)gaudio_address_get("gqueue_process");

    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_EFFECT;
    addon.addon = new gechoeffectcreator();
    return &addon;
}






