#ifndef GFILTER_H
#define GFILTER_H
#include "gaudio.h"
#include "gmain.h"
#include "gmath.h"

#define LOWPASSFREQREF (5000)

struct gfilter
{
    float coeff;
#ifndef _MSC_VER
    float history[0];
#else
    float history[1];
#endif
};

static inline float gfilter2p(gfilter* iir,uint32_t offset,float input)
{
    float* history = &iir->history[offset*2];
    float a = iir->coeff;
    float output = input;
    output = output + (history[0]-output)*a;
    history[0] = output;
    output = output + (history[1]-output)*a;
    history[1] = output;
    return output;
}

static inline float gfilter2pc(gfilter* iir,uint32_t offset,float input)
{
    const float* history = &iir->history[offset*2];
    float a = iir->coeff;
    float output = input;
    output = output + (history[0]-output)*a;
    output = output + (history[1]-output)*a;
    return output;
}

#define GFILTERCOS cosf(M_PI*2.0f * LOWPASSFREQREF / AUDIO_STANDARD_SAMPLERATE)

/* Calculates the low-pass filter coefficient given the pre-scaled gain and
 * cos(w) value. Note that g should be pre-scaled (sqr(gain) for one-pole,
 * sqrt(gain) for four-pole, etc) */
static inline float gcoeffcalc(float g,float cw)
{
    float a = 0.0f;
    if(g < 0.9999f)
    {
        /* Be careful with gains < 0.001, as that causes the coefficient head
         * towards 1, which will flatten the signal */
        g = gmax(g,0.001f);
        a = (1.0f-g*cw-sqrtf(2*g*(1.0f-cw)-g*g*(1.0f-cw*cw)))/(1.0f-g);
    }
    return a;
}

#endif
