#include "gcontext.h"
#include "gmain.h"
#include "gwriter.h"
#include "gaddons.h"

gwritercontext::~gwritercontext()
{
    std::vector<std::pair<void*,gwriterfactory*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        gwriterfactory* factory = itr->second;
        factory->destroy();
        void* handle = itr->first;
        gaddonclose(handle);;
        itr++;
    }
}

int32_t gwritercontext::is_support(int32_t flag)
{
    std::vector<std::pair<void*,gwriterfactory*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        gwriterfactory* factory = itr->second;
        int32_t support = factory->is_support(flag);
        if(support != 0)
            return support;
        itr++;
    }
    return 0;
}

void gwritercontext::addwriter(void* handle,gwriterfactory* factory)
{
    list.push_back(std::make_pair(handle,factory));
}

gwriter* gwritercontext::create_by_id(int32_t flag,int32_t channel,int32_t samplerate,int32_t format)
{
    std::vector<std::pair<void*,gwriterfactory*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        gwriterfactory* creator = itr->second;
        gwriter* writer = creator->create_by_id(flag,channel,samplerate,format);
        if(writer != NULL)
            return writer;
        itr++;
    }
    return NULL;
}
