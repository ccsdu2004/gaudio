#ifndef G_DRIVER_H
#define G_DRIVER_H
#include "gaudio.h"

struct gdriver;

typedef gdriver* (GAPIENTRY* gdriver_create)(int32_t format,int32_t channel,int32_t samplerate);
typedef void (GAPIENTRY* gdriver_play)(gdriver*,char* buffer,uint32_t);
typedef void (GAPIENTRY* gdriver_close)(gdriver*);
typedef uint32_t (GAPIENTRY* gdriver_buffer)(short* buffer);
typedef void (GAPIENTRY* gdriver_capture_start)(int32_t format,int32_t channel,int32_t samplerate,gdriver_buffer);
typedef void (GAPIENTRY* gdriver_capture_stop)();
typedef void (GAPIENTRY* gdriver_destroy)(gdriver*);

struct gdriverimpl;

struct gdriver
{
    uint32_t id;
    gdriver_play   play;
    gdriver_close  close;
    gdriver_capture_start capture_start;
    gdriver_capture_stop  capture_stop;
    gdriver_destroy destroy;
    gdriverimpl* impl;
};

#endif
//! by duwenhua2013
