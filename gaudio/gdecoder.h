#ifndef GDECODER_H
#define GDECODER_H
#include <cstdio>
#include "gaudio.h"
#include "gmeta.h"

struct gdecoder;
typedef const char* (GAPIENTRY *gdecoder_maker_callback)(gdecoder*);
typedef int32_t (GAPIENTRY *gdecoder_init_callback)(gdecoder*);
typedef int32_t (GAPIENTRY *gdecoder_meta_callback)(gdecoder*,int32_t,char*);
typedef int32_t (GAPIENTRY *gdecoder_seek_callback)(gdecoder*,uint32_t);
typedef uint32_t(GAPIENTRY *gdecoder_read_callback)(gdecoder*,float*,int32_t);
typedef void    (GAPIENTRY *gdecoder_destroy)(gdecoder*);
typedef uint32_t(GAPIENTRY *gdecoder_int32)(gdecoder*,uint32_t flag);

struct gdecoderimpl;

struct gdecoder
{
    gdecoder_maker_callback maker;
    gdecoder_init_callback  init;
    gdecoder_meta_callback  meta;
    gdecoder_seek_callback  seek;
    gdecoder_read_callback  read;
    gdecoder_destroy        destroy;
    gdecoder_int32          getint;

    int32_t decid;
    int32_t state;
    int32_t enc;
    int32_t version;

    void* stream;
    void* resample[2];
    float ratio;

    gdecoderimpl* impl;
};

#endif





















