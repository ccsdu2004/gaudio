#include <algorithm>
#include "gaudio.h"
#include "gmain.h"
#include "gqueue.h"

class gqueue
{
public:
    gqueue(int32_t len = AUDIO_STANDARD_SAMPLERATE)
    {
        int32_t const_len = AUDIO_STANDARD_SAMPLERATE;
        buffer = new float[const_len];
        length = len;
        used = 0;
    }
    ~gqueue(){delete []buffer;}
public:
    void update(int32_t length);
    int32_t process(float*in,int32_t len,float* out);
private:
    int32_t length;
    int32_t used;
    float* buffer;
};

void gqueue::update(int32_t len)
{
    used = 0;
    length = len > 44100 ? 44100 : len;
}

int32_t gqueue::process(float* in,int32_t len,float* out)
{
    if(len+used<=length)
    {
        std::copy(in,in+len,buffer+used);
        used += len;
        return 0;
    }
    else if(used == length)
    {
        int32_t ret = len;
        std::copy(buffer,buffer+ret,out);
        std::copy(buffer+ret,buffer+used,buffer);
        std::copy(in,in+len,buffer+used-ret);
        return len;
    }
    else
    {
        int32_t ret = len + used - length;
        std::copy(buffer,buffer+ret,out);
        std::copy(buffer+ret,buffer+used,buffer);
        std::copy(in,in+len,buffer+used-ret);
        used = length;
        return ret;
    }
}

void* gqueue_create(int32_t len)
{
    gqueue* buffer = new gqueue(len);
    if(buffer == NULL)
    {
        gerror_set(AUDIO_OUT_OF_MEMORY);
        return NULL;
    }
    return buffer;
}

void  gqueue_destroy(void* buffer)
{
    gqueue* ptr = (gqueue*)buffer;
    delete ptr;
}

void  gqueue_resize(void* buffer,int32_t len)
{
    gqueue* ptr = (gqueue*)buffer;
    ptr->update(len);
}

int32_t gqueue_process(void* q,float*in,int32_t len,float* out)
{
    gqueue* ptr = (gqueue*)q;
    return ptr->process(in,len,out);
}


