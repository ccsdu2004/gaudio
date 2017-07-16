#ifndef G_RESAMPLE_H
#define G_RESAMPLE_H

void* gresample_create(int quality,float lowfactor,float highfactor);
int gresample_process(void* handle,float factor,float* in,int inLen,int lastflag,int* used,float* out,int outlen);
void gresample_close(void* handle);

#endif
