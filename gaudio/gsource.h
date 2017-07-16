#ifndef GSOURCE_H
#define GSOURCE_H
#include <list>
#include "gaudio.h"
#include "gmain.h"
#include "geffect.h"
#include "gdecoder.h"
#include "gthread.h"

#define STATE_IDLE    0
#define STATE_PLAY    1
#define STATE_PAUSE   2
#define STATE_STOP    3
#define STATE_DESTROY 4

struct gsource_
{
    gsource_();
    ~gsource_();

    void init_state(int32_t);

    bool is3d;

    volatile int32_t state;
    volatile int32_t state_old;

    gdecoder* decoder;
    gfinish   finish;
    gerror    error;
    gposition position;
    int32_t   loop;

    float pan;
    volatile float gain;
    volatile long  curpos;
    volatile int32_t mute;

    void* user;

    std::list<geffect*> effect;
    gdatabuffer<float> frontbuffer;

    void applyeffect();
    void applystate(int32_t state);

    /*volatile*/ float pos[3];
    /*volatile*/ float velocity[3];
    /*volatile*/ float orientation[3];
	/*volatile*/ bool  update;

	/*volatile*/ float picth;
	/*volatile*/ float outgain;
	/*volatile*/ float mingain;
	/*volatile*/ float maxgain;
    /*volatile*/ float inangle;
    /*volatile*/ float outangle;
    /*volatile*/ float distance;
    /*volatile*/ float maxdistance;
                 float refdistance;
    /*volatile*/ float rolloff;
};

class gSourceSort
{
public:
    bool operator()(gsource* source1,gsource* source2)const
    {
        return source1->frontbuffer.use > source2->frontbuffer.use;
    };
};

void gCalcSourceParams(gsource* source,bool mono,float* dry,float* wet,float* pitch);

void g3dEffectCalc(gsource* source);

#endif
//! by duwenhua2013









