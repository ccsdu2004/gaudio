#include <stdlib.h>
#include "gaudio.h"
#include "gmain.h"

#define THREAD_STACK_SIZE (1*1024*1024)

#if defined(WIN32) || defined(WIN64)
#include <windows.h>
typedef struct
{
    uint32_t(*func)(void*);
    void *ptr;
    HANDLE thread;
}ThreadInfo;

static DWORD APIENTRY gstarterfunc(void* ptr)
{
    ThreadInfo *inf = (ThreadInfo*)ptr;
    int32_t ret = inf->func(inf->ptr);
    ExitThread((DWORD)ret);
    return ret;
}

void* gstartthread(uint32_t(*func)(void*),void *ptr)
{
    ThreadInfo* inf = (ThreadInfo*)malloc(sizeof(ThreadInfo));
    if(!inf)return 0;

    inf->func = func;
    inf->ptr = ptr;

    inf->thread = CreateThread(NULL,THREAD_STACK_SIZE,gstarterfunc,inf,0,NULL);
    if(!inf->thread)
    {
        free(inf);
        return NULL;
    }
    return inf;
}

uint32_t gstopthread(void* thread)
{
    printf("...\n");
    ThreadInfo* inf = (ThreadInfo*)thread;
    WaitForSingleObject(inf->thread,INFINITE);
    DWORD ret = 0;
    GetExitCodeThread(inf->thread,&ret);
    CloseHandle(inf->thread);
    free(inf);
    printf(".....\n");
    return (uint32_t)ret;
}

#else

#include <pthread.h>

typedef struct
{
    uint32_t (*func)(void*);
    void*    ptr;
    uint32_t ret;
    pthread_t thread;
} ThreadInfo;

static void* StarterFunc(void *ptr)
{
    ThreadInfo *inf = (ThreadInfo*)ptr;
    inf->ret = inf->func(inf->ptr);
    return NULL;
}

void* gstartthread(uint32_t(*func)(void*),void *ptr)
{
    pthread_attr_t attr;
    ThreadInfo* inf = (ThreadInfo*)malloc(sizeof(ThreadInfo));
    if(!inf) return NULL;

    if(pthread_attr_init(&attr) != 0)
    {
        free(inf);
        return NULL;
    }
    if(pthread_attr_setstacksize(&attr, THREAD_STACK_SIZE) != 0)
    {
        pthread_attr_destroy(&attr);
        free(inf);
        return NULL;
    }

    inf->func = func;
    inf->ptr = ptr;
    if(pthread_create(&inf->thread, &attr, StarterFunc, inf) != 0)
    {
        pthread_attr_destroy(&attr);
        free(inf);
        return NULL;
    }
    pthread_attr_destroy(&attr);

    return inf;
}

uint32_t gstopthread(void *thread)
{
    ThreadInfo* inf = (ThreadInfo*)thread;
    uint32_t ret;

    pthread_join(inf->thread,NULL);
    ret = inf->ret;
    free(inf);
    return ret;
}

#endif
