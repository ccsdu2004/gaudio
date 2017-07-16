#ifndef G_LISTENER_H
#define G_LISTENER_H
#include "gaudio.h"

class glistener
{
public:
    glistener();
    //volatile
    float pos[3];
    //volatile
    float vel[3];
    //volatile
    float forward[3];
    //volatile
    float up[3];
    //volatile
    float gain;
    //volatile
    float mpu;
    int32_t update;
};

#endif
//! by duwenhua2013
