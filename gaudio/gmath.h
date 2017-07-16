#ifndef G_MATH_H
#define G_MATH_H
#include <math.h>
#include "gaudio.h"

#ifndef M_PI
#define M_PI  3.14159265358979323846
#endif

template<class T>
inline T gmin(T a,T b)
{
    return (a > b) ? b : a;
}

template<class T>
inline T gmax(T a,T b)
{
    return (a < b) ? b : a;
}

template<class T>
inline T gclamp(T a,T b,T c)
{
    return gmin(a,gmax(b,c));
}

static inline float f2f(float val)
{
    return val;
}

static inline int32_t fastf2i(float f)
{
#ifdef HAVE_LRINTF
    return lrintf(f);
#elif defined(_MSC_VER) && defined(_M_IX86)
    int i;
    __asm fld f
    __asm fistp i
    return i;
#else
    return (int32_t)f;
#endif
}

static inline int32_t f2i(float val)
{
    val = val+1.0f - fabsf(val-1.0f);
    val = (val-2.0f + fabsf(val+2.0f)) * 0.25f;
    /* Convert to a signed integer,between -2147483647 and +2147483647. */
    return fastf2i((float)(val*2147483647.0));
}

static inline uint32_t f2ui(float val)
{
    return f2i(val)+2147483648u;
}

static inline int16_t f2s(float val)
{
    return f2i(val)>>16;
}

static inline uint16_t f2us(float val)
{
    return f2s(val)+32768;
}

static inline int8_t f2char(float val)
{
    return f2i(val)>>24;
}

static inline uint8_t f2uchar(float val)
{
    return f2char(val)+128;

}

static inline float lerp(float val1,float val2,float mu)
{
    return val1 + (val2-val1)*mu;
}

static inline float cubic(float val0,float val1,float val2,float val3,float mu)
{
    float mu2 = mu*mu;
    float a0 = -0.5f*val0 +  1.5f*val1 + -1.5f*val2 +  0.5f*val3;
    float a1 =       val0 + -2.5f*val1 +  2.0f*val2 + -0.5f*val3;
    float a2 = -0.5f*val0              +  0.5f*val2;
    float a3 =                    val1;
    return a0*mu*mu2 + a1*mu2 + a2*mu + a3;
}

static inline uint32_t gnextp2(uint32_t value)
{
    if(value > 0)
    {
        value--;
        value |= value>>1;
        value |= value>>2;
        value |= value>>4;
        value |= value>>8;
        value |= value>>16;
    }
    return value+1;
}

static inline uint32_t fastf2u(float f)
{
    return fastf2i(f);
}

#endif
