#include "gcontext.h"
#include "gmain.h"
#include "gdecoder.h"
#include "gresample.h"
#include "gdecreator.h"
#include "gaddons.h"

gdecoderfactory::~gdecoderfactory()
{
    std::vector<std::pair<void*,gdecreator*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        gdecreator* creator = itr->second;
        creator->destroy();
        void* handle = itr->first;
        gaddonclose(handle);;
        itr++;
    }
}

int32_t gdecoderfactory::is_support(int32_t dec,int32_t mask)
{
    std::vector<std::pair<void*,gdecreator*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        gdecreator* creator = itr->second;
        int32_t ret = creator->is_support(dec,0);
        if(ret == TRUE)
            return TRUE;
        itr++;
    }
    return FALSE;
}

void gdecoderfactory::addcreator(void* handle,gdecreator* creator)//,gaddons* addon)
{
    list.push_back(std::make_pair(handle,creator));
}

gdecoder* gdecoderfactory::create_by_decid(int32_t dec,int32_t mask)
{
    std::vector<std::pair<void*,gdecreator*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        gdecreator* creator = itr->second;
        gdecoder* decoder = creator->create_by_decid(dec,mask);
        if(decoder != NULL)
            return decoder;
        itr++;
    }
    return NULL;
}

gdecoder* gdecoderfactory::create_by_extension(const char* extension)
{
    std::vector<std::pair<void*,gdecreator*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        gdecreator* creator = itr->second;
        gdecoder* decoder = creator->create_by_extension(extension);
        if(decoder != NULL)
            return decoder;
        itr++;
    }
    return NULL;
}

gdecoder* gdecoderfactory::create_by_buffer(int8_t* buffer,uint32_t length)
{
    std::vector<std::pair<void*,gdecreator*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        gdecreator* creator = itr->second;
        gdecoder* decoder = creator->create_by_buffer(buffer,length);
        if(decoder != NULL)
            return decoder;
        itr++;
    }
    return NULL;
}
