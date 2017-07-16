#include "gaddons.h"
#include "geffect.h"

geffectfactory::~geffectfactory()
{
    printf("geffect.\n");
    std::vector<std::pair<void*,geffectcreator*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        geffectcreator* creator = itr->second;
        printf("123:%i\n");
        creator->destroy();
        printf("124\n");
        void* handle = itr->first;
        gaddonclose(handle);
        printf("125\n");
        itr++;
    }
    printf("geffect1.\n");
}

int32_t geffectfactory::is_support(int32_t efx,int32_t mask)
{
    std::vector<std::pair<void*,geffectcreator*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        geffectcreator* creator = itr->second;
        int32_t ret = creator->is_support(efx,mask);
        if(ret == TRUE)
            return TRUE;
        itr++;
    }
    return FALSE;
}

void geffectfactory::addcreator(void* handle,geffectcreator* creator)
{
    list.push_back(std::make_pair(handle,creator));
}

geffect* geffectfactory::create_by_id(int32_t id,int32_t mask)
{
    std::vector<std::pair<void*,geffectcreator*> >::iterator itr = list.begin();
    while(itr != list.end())
    {
        geffectcreator* creator = itr->second;
        geffect* efx = creator->create_by_id(id,mask);
        if(efx != NULL)
            return efx;
        itr++;
    }
    return NULL;
}
