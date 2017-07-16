#ifndef GADDONS_H
#define GADDONS_H
#include "gaudio.h"
#include "gcontext.h"
#include "gmain.h"

//! addon symbol ginstance.
#define EXPORT_SYMBOL "ginstance"

typedef struct gaddons gaddons;

typedef void (GAPIENTRY* gaddon_deinit)(gaddons*);

class gaddons
{
public:
    int32_t flag;
    void* handle;
    void* addon;
    gaddon_deinit deinit;
};

typedef gaddons* (GAPIENTRY* ginstance_ptr)(void*);

void gloadaddons(const char*,int32_t);
void gaddonclose(void*);
void gsetaddon(const char*,gaddons* addon,void* handle);

#if defined(__cplusplus)
extern "C"
{
#endif

#if(defined(WIN32) || defined(WIN64))
    __declspec(dllexport) gaddons* GAPIENTRY ginstance(void*);
#else
    gaddons* ginstance(void*);
#endif

    gaddons* gwave_instance(void* ctx);
    gaddons* gau_instance(void* ctx);
    gaddons* gaiff_instance(void* ctx);
    gaddons* gfade_effect_instance(void* ctx);
    gaddons* gspeedchanger_effect_instance(void* ctx);
    gaddons* g168_effect_instance(void* ctx);

#if defined(__cplusplus)
}
#endif

#endif
