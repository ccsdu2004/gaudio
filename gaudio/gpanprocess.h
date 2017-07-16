#ifndef G_PAN_PROCESS_H
#define G_PAN_PROCESS_H
#include <algorithm>

struct gpan
{
    gpan(float g):gain(g)
    {
    }
    float gain;
    void operator()(float& val)
    {
        val *= gain;
    }
};

#define GAIN_PROCESS(flag,pan,g)\
    float gain[2] = {scontext.device.gain,scontext.device.gain};\
    if(flag)\
        gain[pan < 0 ? 0 : 1] = g*scontext.device.gain;\
    gpan _1(gain[0]);\
    gpan _2(gain[1]);\
    std::for_each(scontext.device.frontbuffer.data[0],scontext.device.frontbuffer.data[0]+scontext.device.frontbuffer.use,_1);\
    std::for_each(scontext.device.frontbuffer.data[1],scontext.device.frontbuffer.data[1]+scontext.device.frontbuffer.use,_2);

#endif
//! by duwenhua
