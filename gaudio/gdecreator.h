#ifndef G_DECEATOR_H
#define G_DECEATOR_H
#include <vector>
#include <utility>
#include "gdecoder.h"

class gdecreator
{
public:
    virtual ~gdecreator(){}
    virtual int32_t is_support(int32_t dec,int32_t mask) = 0;
    virtual gdecoder* create_by_extension(const char* extension) = 0;
    virtual gdecoder* create_by_decid(int32_t dec,int32_t mask) = 0;
    virtual gdecoder* create_by_buffer(int8_t* buffer,uint32_t length) = 0;
    virtual void destroy() = 0;
};

#define gxcreator(x)\
class g##x##decreator : public gdecreator\
{\
public:\
    int32_t is_support(int32_t efx,int32_t mask);\
    gdecoder* create_by_extension(const char* extension);\
    gdecoder* create_by_decid(int32_t dec,int32_t mask);\
    gdecoder* create_by_buffer(int8_t* buffer,uint32_t length);\
    void destroy();\
};

class gdecoderfactory : public gdecreator
{
public:
    ~gdecoderfactory();
    int32_t is_support(int32_t dec,int32_t mask);
    void addcreator(void* handle,gdecreator* creator);
    gdecoder* create_by_extension(const char* extension);
    gdecoder* create_by_decid(int32_t dec,int32_t mask);
    gdecoder* create_by_buffer(int8_t* buffer,uint32_t length);
    void destroy(){}
private:
    std::vector<std::pair<void*,gdecreator*> > list;
};

#endif
