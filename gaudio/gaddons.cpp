#if(defined(WIN32) || defined(WIN64))
    #include <windows.h>
#else
    #include <dlfcn.h>
    #include <dirent.h>
#endif
#include <string.h>
#include "gstring.h"
#include "gaddons.h"
#include "gmemory.h"
#include "gmain.h"

void gaddonclose(void* handle)
{
	if(handle)
	{
#if(defined(WIN32) || defined(WIN64))
		::FreeLibrary(static_cast<HMODULE>(handle));
#else
		::dlclose(handle);
#endif
	}
}

void gsetaddon(const char* file,gaddons* addon,void* handle)
{
    if(addon->flag == AUDIO_PLUGIN_AUDIO)
    {
        addon->handle = handle;
        gdecreator* creator = (gdecreator*)addon->addon;
        if(!creator)
            gaddonclose(handle);
        else
            scontext.factory.decoderfactory.addcreator(handle,creator);
    }
    else if(addon->flag == AUDIO_PLUGIN_EFFECT)
    {
        addon->handle = handle;
        geffectcreator* creator = (geffectcreator*)addon->addon;
        if(creator)
            scontext.factory.effectfactory.addcreator(handle,creator);
        else
            gaddonclose(handle);
    }
    else if(addon->flag == AUDIO_PLUGIN_WRITER)
    {
        addon->handle = handle;
        gwriterfactory* creator = (gwriterfactory*)addon->addon;
        if(creator)
        {
            scontext.factory.writerfactory.addwriter(handle,creator);
        }
        else
            gaddonclose(handle);
    }
    else if(addon->flag == AUDIO_PLUGIN_DRIVER && !scontext.driver_create)
    {
        addon->handle = handle;
        scontext.driver_create = (gdriver_create)addon->addon;
        if(scontext.driver_create)
            scontext.driver = scontext.driver_create(16,DEVICE_CHANNEL_STEREO,AUDIO_STANDARD_SAMPLERATE);
        else
            scontext.driver = NULL;
    }
    else
        gaddonclose(handle);
}

void gloadplugin(const char* file)
{
#if(defined(WIN32) || defined(WIN64))
	void* handle = (void*)LoadLibrary(file);
#else
    // RTLD_NOW
	void* handle = ::dlopen(file,RTLD_LAZY | RTLD_GLOBAL);
#endif

    if(!handle)
    {
        //int error = GetLastError();
        return;
    }

#if(defined(WIN32) || defined(WIN64))
    HMODULE module = static_cast<HMODULE>(handle);
	void* fn = (void*)GetProcAddress(module,EXPORT_SYMBOL);
#else
	void* fn = ::dlsym(handle,EXPORT_SYMBOL);
#endif
    ginstance_ptr instance = (ginstance_ptr)fn;
    if(!instance)
    {
        gaddonclose(handle);
        return;
    }

    gaddons* addon = instance(handle);
    if(!addon)
    {
        gaddonclose(handle);
        return;
    }

    gsetaddon(file,addon,handle);
}

void gloadaddons(const char* addons,int32_t flag)
{
#if(defined(WIN32) || defined(WIN64))
    char dir[MAX_PATH] = {0};
    GetModuleFileName(NULL,dir,MAX_PATH);
    char* p = strrchr(dir,'\\');
    *p = 0;
    strcat(dir,"//");
    strcat(dir,addons);
    strcat(dir,"//");
    char target[MAX_PATH] = {0};
    strcpy(target,dir);
    strcat(target,"*.dll");

    WIN32_FIND_DATA data;
    HANDLE finder = FindFirstFile(target,&data);
    if(INVALID_HANDLE_VALUE == finder)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }

    while(TRUE)
    {
        if(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
        }
        else
        {
            char file[MAX_PATH] = {0};
            strcpy(file,dir);
            strcat(file,data.cFileName);
            gloadplugin(file);
        }
        if(!FindNextFile(finder,&data))
            break;
    }
    FindClose(finder);
#else
    DIR* dir;
    struct dirent* ent;

    dir = opendir(addons);
    if(dir == 0)
        return;

    char fullpath[255] = {0};
    while((ent=readdir(dir))!=NULL)
    {
        if(ent->d_type & DT_DIR)
        {
        }
        else
        {
            strcpy(fullpath,addons);
            strcat(fullpath,"//");
            strcat(fullpath,ent->d_name);
            gloadplugin(fullpath);
        }
    }
    closedir(dir);
#endif
#ifdef HAVE_MIDI
    {
        gaddons* addon = gmidi_instance(0);
        if(addon)
            gsetaddon(NULL,addon,0);
    }
#endif

#define LOAD_ADDON(fn)\
    {\
        gaddons* addon = fn(0);\
        if(addon)\
            gsetaddon(NULL,addon,0);\
    }

    LOAD_ADDON(gwave_instance)
    LOAD_ADDON(gau_instance)
    //LOAD_ADDON(gfade_effect_instance)
    //LOAD_ADDON(gspeedchanger_effect_instance)
    //LOAD_ADDON(g168_effect_instance)
}

