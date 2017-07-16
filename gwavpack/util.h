#ifndef UTIL_H
#define UTIL_H
#include "../gaudio/streamBuffer.h"

typedef char*(*stream_file_ptr)(streamBuffer* stream);
typedef long (*stream_read_ptr)(streamBuffer* stream,void* ptr,int32_t size,long n);
typedef long (*stream_tell_ptr)(streamBuffer* stream);
typedef long (*stream_seek_ptr)(streamBuffer* stream,long offset,int32_t flag);

extern stream_tell_ptr stream_tell_;
extern stream_read_ptr stream_read_;
extern stream_seek_ptr stream_seek_;

#endif
