#include <algorithm>
#include "gmath.h"
#include "gmixer.h"

void gmixern(gsource** input,int32_t n,gsource* ref,int32_t maxuse);

void gjoin(const gdatabuffer<float>& input,int16_t* output,uint32_t count)
{
    for(uint32_t i=0;i<count;i++)
    {
        float left = input.data[0][i];
        float right = input.data[1][i];
        float lvalue = gclamp<float>(left,-1.0f,1.0f);
        float rvalue = gclamp<float>(right,-1.0f,1.0f);
        *(output++) = 32767.0f*lvalue;
        *(output++) = 32767.0f*rvalue;
    }
}

#define COPY_BUFFER_HYBIRD(n)\
    for(uint32_t i=0;i<n;i++)\
    {\
        scontext.device.frontbuffer.data[0][i] = input[i%2]->frontbuffer.data[0][i];\
        scontext.device.frontbuffer.data[1][i] = input[1-i%2]->frontbuffer.data[1][i];\
    }

#define COPY_BUFFER_ONE(flag,start,end)\
    for(uint32_t i=start;i<end;i++)\
    {\
        scontext.device.frontbuffer.data[0][i] = input[flag]->frontbuffer.data[0][i];\
        scontext.device.frontbuffer.data[1][i] = input[flag]->frontbuffer.data[1][i];\
    }

void gmixer_n(gsource** input,int32_t n,gsource* ref,int32_t maxuse)
{
    if(n == 0)
        scontext.device.frontbuffer.use = 0;
    else if(n == 1)
    {
        uint32_t length = input[0]->frontbuffer.use;
        std::copy(input[0]->frontbuffer.data[0],input[0]->frontbuffer.data[0]+length,scontext.device.frontbuffer.data[0]);
        std::copy(input[0]->frontbuffer.data[1],input[0]->frontbuffer.data[1]+length,scontext.device.frontbuffer.data[1]);
        scontext.device.frontbuffer.use = length;
    }
    else if(n == 2)
    {
        uint32_t size0 = input[0]->frontbuffer.use;
        uint32_t size1 = input[1]->frontbuffer.use;
        scontext.device.frontbuffer.use = gmax(size0,size1);
        if(size0 == size1)
        {
            COPY_BUFFER_HYBIRD(size0)
        }
        else
        {
            uint32_t low = gmin(size0,size1);
            COPY_BUFFER_HYBIRD(low)
            if(low == size0)
            {
                COPY_BUFFER_ONE(1,low,size1-low)
            }
            else
            {
                COPY_BUFFER_ONE(0,low,size0-low)
            }
        }
    }
    else
    {
        gmixern(input,n,ref,maxuse);
    }
}

void gmixer()
{
    //gscoperlocker locker(&scontext.locker);

    gsource* sources[AUDIO_MAX_SOURCE];
    gsource* ref = 0;
    int32_t n = 0;
    int32_t maxuse = 0;
    std::list<gsource*>::iterator itr = scontext.device.source.begin();
    while(itr != scontext.device.source.end())
    {
        gsource* sour = *itr;
        if(sour != NULL && sour->state == STATE_PLAY && !sour->mute)
        {
            sources[n++] = sour;
            if(sour->frontbuffer.use > maxuse)
            {
                maxuse = sour->frontbuffer.use;
                ref = sour;
            }
        }
        itr ++;
    }

    gmixer_n(sources,n,ref,maxuse);
}

void gmixern(gsource** input,int32_t n,gsource* ref,int32_t maxuse)
{
    gsource* cur = 0;
    int32_t curlen = 0;
    for(int32_t i=0;i<n;i++)
    {
        cur = input[i];
        curlen = cur->frontbuffer.use;
        if(cur->frontbuffer.use != maxuse)
        {
            std::copy(ref->frontbuffer.data[0]+curlen,ref->frontbuffer.data[0]+maxuse-curlen,cur->frontbuffer.data[0]+curlen);
            std::copy(ref->frontbuffer.data[1]+curlen,ref->frontbuffer.data[1]+maxuse-curlen,cur->frontbuffer.data[1]+curlen);
        }
    }

    for(uint32_t i=0;i<maxuse;i++)
    {
        scontext.device.frontbuffer.data[0][i] = input[i%maxuse  ]->frontbuffer.data[0][i];
        scontext.device.frontbuffer.data[1][i] = input[i == maxuse -1? 0 : 1%maxuse+1]->frontbuffer.data[1][i];
    }
}

