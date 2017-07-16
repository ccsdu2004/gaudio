#ifndef GMEMORY_H
#define GMEMORY_H
#include "gaudio.h"
#include "gmain.h"
#include "gcontext.h"

template<class T>
T*  gcreate_buffer(int32_t size)
{
    if(size == 0)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return NULL;
    }
    T* buffer = new T[size];
    if(buffer == NULL)
        gerror_set(AUDIO_OUT_OF_MEMORY);
    return buffer;
}

#endif
//! by duwenhua2013
