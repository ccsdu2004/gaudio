#include "gaddons.h"
#include "gcatcher.h"

gcatcherfactory::~gcatcherfactory()
{
    std::vector<std::pair<void*,gcatchercreator*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        gcatchercreator* creator = itr->second;
        creator->destroy();
        void* handle = itr->first;
        gaddonclose(handle);
        itr++;
    }
}

int32_t gcatcherfactory::is_support(int32_t id,int32_t mask)
{
    std::vector<std::pair<void*,gcatchercreator*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        gcatchercreator* creator = itr->second;
        int32_t ret = creator->is_support(id,mask);
        if(ret == TRUE)
            return TRUE;
        itr++;
    }
    return FALSE;
}

void gcatcherfactory::addcreator(void* handle,gcatchercreator* creator)
{
    list.push_back(std::make_pair(handle,creator));
}

gcatcher* gcatcherfactory::create_by_id(int32_t id,int32_t mask)
{
    std::vector<std::pair<void*,gcatchercreator*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        gcatchercreator* creator = itr->second;
        gcatcher* recorder = creator->create_by_id(id,mask);
        if(recorder != NULL)
            return recorder;
        itr++;
    }
    return NULL;
}
