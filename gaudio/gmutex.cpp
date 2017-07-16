#include "gthread.h"

gmutex::gmutex()
{
#ifdef _WIN32 || _WIN64
    InitializeCriticalSection(&section);
#else
    pthread_mutex_init(&section,0);
#endif
}

gmutex::~gmutex()
{
#ifdef _WIN32
    DeleteCriticalSection(&section);
#else
    pthread_mutex_destroy(&section);
#endif
}

void gmutex::lock()
{
#ifdef _WIN32
    EnterCriticalSection(&section);
#else
    pthread_mutex_lock(&section);
#endif
}

void gmutex::unlock()
{
#ifdef _WIN32
    LeaveCriticalSection(&section);
#else
    pthread_mutex_unlock(&section);
#endif
}




