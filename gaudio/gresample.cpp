#include "libresample.h"
#include "gresample.h"

void* gresample_create(int quality,float lowfactor,float highfactor)
{
    return resample_open(quality,lowfactor,highfactor);
}

int gresample_process(void* handle,float factor,float* in,int inlen,int lastflag,int* used,float* out,int outlen)
{
    return resample_process(handle,factor,in,inlen,lastflag,used,out,outlen);
}

void gresample_close(void* handle)
{
    resample_close(handle);
}
