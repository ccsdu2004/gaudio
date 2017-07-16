#ifndef G_RECORDER_H
#define G_RECORDER_H
#include <vector>
#include <utility>
#include "gaudio.h"

typedef gcatcher* (GAPIENTRY *grecorder_create_callback)(gcatcher*,const char*,uint32_t,uint32_t,uint32_t);
typedef uint32_t   (GAPIENTRY *grecorder_write_callback)(gcatcher*,int16_t*,uint32_t);
typedef uint32_t   (GAPIENTRY *grecorder_close_callback)(gcatcher*);
typedef void       (GAPIENTRY *grecorder_destroy_callback)(gcatcher*);

#define RECORDER_STATE_CREATE 0
#define RECORDER_STATE_START  1
#define RECORDER_STATE_WRITE  2
#define RECORDER_STATE_CLOSE  3
#define RECORDER_STATE_PAUSE  4

struct gcatcher_
{
    grecorder_create_callback  create;
    grecorder_write_callback   write;
    grecorder_close_callback   close;
    grecorder_destroy_callback destroy;
    int32_t state;
};

class gcatchercreator
{
public:
    virtual ~gcatchercreator(){}
    virtual int32_t is_support(int32_t id,int32_t mask) = 0;
    virtual gcatcher* create_by_id(int32_t id,int32_t mask) = 0;
    virtual void destroy() = 0;
};

#define grecreator(x)\
class g##x##grecordercreator : public gcatchercreator\
{\
public:\
    int32_t is_support(int32_t re,int32_t mask);\
    gcatcher* create_by_id(int32_t id,int32_t mask);\
    void destroy();\
};

class gcatcherfactory : public gcatchercreator
{
public:
    ~gcatcherfactory();
    int32_t is_support(int32_t efx,int32_t mask);
    void addcreator(void* handle,gcatchercreator* creator);
    gcatcher* create_by_id(int32_t id,int32_t mask);
    void destroy(){}
private:
    std::vector<std::pair<void*,gcatchercreator*> > list;
};


#endif
