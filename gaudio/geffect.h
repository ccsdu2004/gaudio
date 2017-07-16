#ifndef G_EFFECT_H
#define G_EFFECT_H
#include <vector>
#include <utility>
#include "gaudio.h"
#include "gaddons.h"
#include "gsource.h"
#include "geffect.h"
#include "gthread.h"

typedef void (*geffect_destroy)(geffect*);
typedef void (GAPIENTRY* gefxprocess)(geffect*,gdatabuffer<float>&);
typedef void (GAPIENTRY* geffect_set_float)(geffect*,int32_t,float);
typedef void (GAPIENTRY* geffect_get_float)(geffect*,int32_t,float*);
typedef void (GAPIENTRY* geffect_set_int32)(geffect*,int32_t,int32_t);
typedef void (GAPIENTRY* geffect_get_int32)(geffect*,int32_t,int32_t*);

#define MAX_EFFECT_BIND_TO_SOURCE 6

enum
{
    EFFECT_SUPPORT_BIND_TO_GLOBAL = 1,
    EFFECT_SUPPORT_BIND_TO_SOURCE = 2
};

struct geffectimpl;

struct geffect_
{
    int32_t mask;
    int32_t order;
    int32_t state;
    int32_t curid;
    int32_t binded;
    int32_t update;
    gefxcb  fftcb;
    gsource* source;
    gefxprocess process;
    geffect_destroy destroy;
    geffect_set_float setfloat;
    geffect_get_float getfloat;
    geffect_set_int32 setint;
    geffect_get_int32 getint;
    geffectimpl* impl;
};

inline int32_t effect_support_global(geffect* effect)
{
    if(!effect)return FALSE;
    return effect->mask == 1 || effect->mask == 3;
}

inline int32_t effect_support_source(geffect* effect)
{
    if(!effect)return FALSE;
    return effect->mask == 2 || effect->mask == 3;
}

class geffectcreator
{
public:
    virtual ~geffectcreator(){}
    virtual int32_t is_support(int32_t efx,int32_t mask) = 0;
    virtual geffect* create_by_id(int32_t id,int32_t mask) = 0;
    virtual void destroy() = 0;
};

#define gecreator(x)\
class g##x##effectcreator : public geffectcreator\
{\
public:\
    int32_t is_support(int32_t efx,int32_t mask);\
    geffect* create_by_id(int32_t id,int32_t mask);\
    void destroy();\
};

class geffectfactory : public geffectcreator
{
public:
    ~geffectfactory();
    int32_t is_support(int32_t efx,int32_t mask);
    void addcreator(void* handle,geffectcreator* creator);
    geffect* create_by_id(int32_t id,int32_t mask);
    void destroy(){}
private:
    std::vector<std::pair<void*,geffectcreator*> > list;
};

#endif
//! by duwenhua2013
