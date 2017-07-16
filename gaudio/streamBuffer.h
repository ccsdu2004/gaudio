#ifndef STREAM_BUFFER_H
#define STREAM_BUFFER_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "gaudio.h"

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

typedef struct _streamBuffer streamBuffer;

streamBuffer* stream_open_file(const char* file);
streamBuffer* stream_open_mem(void* mem,uint32_t size,int32_t close);
streamBuffer* stream_open_callbacks(gread read,gseek seek,gtell tell,gclose close,void* context);
int32_t       stream_read(streamBuffer* stream,void* ptr,int32_t size,int32_t nmemb);
int32_t       stream_seek(streamBuffer* stream,int32_t offset,int32_t flag);
int32_t       stream_tell(streamBuffer* stream);
int32_t       stream_close(streamBuffer* stream);
char*         stream_file(streamBuffer* stream);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
//! by ccsdu2004
