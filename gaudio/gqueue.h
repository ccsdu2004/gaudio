#ifndef QQUEUE_H
#define QQUEUE_H
#include "gaudio.h"

void* gqueue_create(int32_t);
void  gqueue_destroy(void*);
void  gqueue_resize(void*,int32_t);
int32_t gqueue_process(void*,float*in,int32_t len,float* out);

#endif
