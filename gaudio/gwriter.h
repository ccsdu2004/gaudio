#ifndef GEEK_WRITER_H
#define GEEK_WRITER_H
#include <cstdio>
#include "gaudio.h"

//struct gwriter;

typedef const char* (GAPIENTRY *gwriter_maker)(gwriter*);
typedef gwriter* (GAPIENTRY *gwriter_create)(int32_t flag,int32_t,int32_t,int32_t);
typedef int32_t (GAPIENTRY *gwriter_open)(gwriter*,const char*);
typedef int32_t(GAPIENTRY *gwriter_write)(gwriter*,uint8_t* data,int32_t length);
typedef void    (GAPIENTRY *gwriter_close)(gwriter*);
typedef uint32_t(GAPIENTRY *gwriter_destroy)(gwriter*);

struct gwriter_
{
    gwriter_maker maker;
    gwriter_create create;
    gwriter_open open;
    gwriter_write write;
    gwriter_close close;
    gwriter_destroy destroy;

    int32_t id;
    int32_t state;
    int32_t version;
};

class gwriterfactory
{
public:
    virtual ~gwriterfactory(){}
    virtual int32_t is_support(int32_t flag) = 0;
    virtual gwriter* create_by_id(int32_t flag,int32_t channel,int32_t samplerate,int32_t format) = 0;
    virtual void destroy() = 0;
};

#define gwcreator(x)\
class g##x##writer : public gwriterfactory\
{\
public:\
    int32_t is_support(int32_t flag,int32_t channel,int32_t samplerate,int32_t format);\
    gwriter* create_by_id(int32_t flag,int32_t channel,int32_t samplerate,int32_t format);\
    void destroy();\
};

class gwritercontext : public gwriterfactory
{
public:
    ~gwritercontext();
    void addwriter(void* handle,gwriterfactory* factory);
    int32_t is_support(int32_t flag);
    gwriter* create_by_id(int32_t flag,int32_t channel,int32_t samplerate,int32_t format);
    void destroy(){}
private:
    std::vector<std::pair<void*,gwriterfactory*> > list;
};

#endif
