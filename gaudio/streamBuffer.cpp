#include <stdio.h>
#include <string.h>
#include "streamBuffer.h"
#include "gmemory.h"

#ifndef HAVE_MIDI

struct _streamBuffer
{
    char   filepath[MAX_PATH];
    gread  read;
    gseek  seek;
    gclose close;
    gtell  tell;
    void*  ctx;
};

typedef struct _stdIOContext
{
    char filepath[MAX_PATH];
    FILE *fp;
}stdIOContext;

typedef struct memContext
{
    char     filepath[MAX_PATH];
    uint8_t* base;
    uint8_t* current;
    uint8_t* end;
    int32_t  close;
}memContext;

int32_t GAPICONV stdio_stream_read(void* ptr,int32_t size,int32_t nmemb,void* ctx)
{
    return fread(ptr,size,nmemb,((stdIOContext*)ctx)->fp);
}

int32_t GAPICONV stdio_stream_seek(void* ctx,int32_t offset,int32_t flag)
{
    return fseek(((stdIOContext*)ctx)->fp,offset,flag);
}

int32_t GAPICONV stdio_stream_tell(void* ctx)
{
    return ftell(((stdIOContext*)ctx)->fp);
}

int32_t GAPICONV stdio_stream_close(void* ptr)
{
    stdIOContext* ctx = (stdIOContext*)ptr;
    int32_t ret = 0;
    ret = fclose(ctx->fp);
    gFreeBuffer(ctx);
    return ret;
}

int32_t GAPICONV mem_stream_read(void* ptr,int32_t size,int32_t nmemb,void* ctx)
{
    streamBuffer* stream = (streamBuffer*)ctx;
    memContext *c;

    c = (memContext*)stream;
    int32_t count = nmemb;

    if(c->current + count * size > c->end)
        count = (c->end - c->current) / size;
    memcpy(ptr,c->current,count*size);
    c->current += count * size;
    return count;
}

int32_t GAPICONV mem_stream_seek(void* ctx,int32_t offset,int32_t flag)
{
    streamBuffer* stream = (streamBuffer*)ctx;
    memContext* c;
    c = (memContext*)stream;
    if(c == NULL)
        return -1;
    if(flag == SEEK_SET)
    {
        if(offset < 0 || offset > c->end - c->base)
            return -1;
        c->current = c->base + offset;
        return 0;
    }
    else if(flag == SEEK_CUR)
    {
        if(offset > 0)
        {
            if(offset > c->end - c->current)
                return -1;
            c->current += offset;
            return 0;
        }
        else if(offset < 0)
        {
            if(-offset > c->current - c->base)
                return -1;
            c->current += offset;
            return 0;
        }
    }
    else if(flag == SEEK_END)
    {
        if(offset > 0 || offset > c->end - c->base)
            return -1;
        c->current = c->end + offset;
        return 0;
    }
    return -1;
}

int32_t GAPICONV mem_stream_tell(void *ptr)
{
    memContext* ctx = (memContext*)ptr;
    return ctx->current - ctx->base;
}

int32_t GAPICONV mem_stream_close(void *ptr)
{
    memContext* ctx = (memContext*)ptr;
    if(ctx->close)
        gFreeBuffer(ctx->base);
    gFreeBuffer(ctx);
    return 0;
}

streamBuffer* stream_open_fp(FILE* fp,const char* file)
{
    stdIOContext* ctx;
    streamBuffer* stream;

    stream = gcreate_buffer<streamBuffer>(1);
    if(stream == NULL)
        return NULL;

    ctx = gcreate_buffer<stdIOContext>(1);
    if(ctx == NULL)
    {
        gFreeBuffer(stream);
        return NULL;
    }

    ctx->fp = fp;
    strcpy(stream->filepath,file);
    stream->ctx = ctx;
    stream->read = stdio_stream_read;
    stream->seek = stdio_stream_seek;
    stream->tell = stdio_stream_tell;
    stream->close = stdio_stream_close;
    return stream;
}

streamBuffer* stream_open_file(const char *file)
{
    FILE *fp;
    fp = fopen(file,"rb");
    if(fp == NULL)
       return NULL;
    return stream_open_fp(fp,file);
}

streamBuffer* stream_open_mem(void* mem,uint32_t size,int32_t close)
{
    memContext*   ctx;
    streamBuffer* stream;

    stream = gcreate_buffer<streamBuffer>(1);
    if(stream == NULL)
        return NULL;

    ctx = gcreate_buffer<memContext>(1);
    if(ctx == NULL)
    {
        gFreeBuffer(stream);
        return NULL;
    }

    ctx->base = (uint8_t*)mem;
    ctx->current = (uint8_t*)mem;
    ctx->end = ((uint8_t*) mem) + size;
    ctx->close = close;

    memset(stream->filepath,0,MAX_PATH*sizeof(char));
    stream->ctx = ctx;
    stream->read = mem_stream_read;
    stream->seek = mem_stream_seek;
    stream->tell = mem_stream_tell;
    stream->close = mem_stream_close;

    return stream;
}

streamBuffer* stream_open_callbacks(gread read,gseek seek,gtell tell,gclose close,void* context)
{
    streamBuffer* stream = gcreate_buffer<streamBuffer>(1);
    if(stream == NULL)
       return NULL;

    memset(stream->filepath,0,MAX_PATH*sizeof(char));
    stream->ctx = context;
    stream->read = read;
    stream->seek = seek;
    stream->tell = tell;
    stream->close = close;
    return stream;
}

int32_t stream_read(streamBuffer* stream,void* ptr,int32_t size,int32_t nmemb)
{
    return stream->read(ptr,size,nmemb,stream->ctx);
}

int32_t stream_seek(streamBuffer* stream,int32_t offset,int32_t flag)
{
    void* ptr = stream->ctx;
    return stream->seek(ptr,offset,flag);
}

int32_t stream_tell(streamBuffer* stream)
{
    void* ptr = stream->ctx;
    return stream->tell(ptr);
}

int32_t stream_close(streamBuffer* stream)
{
    int32_t ret = stream->close(stream->ctx);
    gFreeBuffer(stream);
    return ret;
}

char* stream_file(streamBuffer* stream)
{
    stdIOContext* ctx = (stdIOContext*)stream;
    return ctx->filepath;
}

#endif

