#ifndef G_MIXER_H
#define G_MIXER_H
#include "gaudio.h"
#include "gmemory.h"

template<bool B>
inline float g4tofloat(uint8_t val)
{
    return (val>>4)/7.0f;
}

template<>
inline float g4tofloat<false>(uint8_t val)
{
    return (val-((val>>4) << 4))/7.0f;
}

void gjoin(const gdatabuffer<float>& input,int16_t* output,uint32_t count);
void gdemuxer24(int32_t channel,const uint8_t* input,float* left,float* right,uint32_t count);
void gmixer();

#endif
//! by duwenhua
