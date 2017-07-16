#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "gmemory.h"
#include "gcontext.h"
#include "gstring.h"
#include "gsource.h"
#include "gdecoder.h"
#include "streamBuffer.h"
#include "gresample.h"
#include "gmain.h"
#include "gmath.h"

#if HAVE_STAT
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#define structStat struct stat
#elif HAVE__STAT
#define stat(p,b) _stat((p),(b))
#define structStat struct _stat
#else
#error "No stat-like function on this platform"
#endif

int32_t gaudio_source_get_string(gsource* source,int32_t flag,char* string)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return FALSE;
    }
    else
    {
        if(!source)
            gerror_set(AUDIO_BAD_VALUE);
        else
        {
            if(source->decoder && source->decoder->meta)
            {
                return source->decoder->meta(source->decoder,flag,string);
            }
        }
    }
    return FALSE;
}

void gaudio_source_set_float(gsource* source,int32_t flag,float value)
{
    if(scontext.killed!=1)
        gerror_set(AUDIO_NOT_INIT);
    else
    {
        if(!source)
            gerror_set(AUDIO_BAD_VALUE);
        else
        {
            //gscoperlocker locker(&sContext.locker);
            if(flag == AUDIO_ENUM_GAIN)
                source->gain = gclamp(value,.0f,1.0f);
            else if(flag == AUDIO_ENUM_PAN)
                source->pan = gclamp(value,AUDIO_PAN_LOW,AUDIO_PAN_HIGH);
        }
    }
}

void gaudio_source_get_float(gsource* source,int32_t flag,float* value)
{
    if(scontext.killed!=1)
        gerror_set(AUDIO_NOT_INIT);
    else
    {
        if(!source)
            gerror_set(AUDIO_BAD_VALUE);
        else if(flag == AUDIO_ENUM_GAIN)
            *value = source->gain;
        else
            gerror_set(AUDIO_BAD_TAG);
    }
}

void gaudio_source_set_finish_callback(gsource* source,gfinish finish)
{
    if(scontext.killed!=1)
        gerror_set(AUDIO_NOT_INIT);
    else
    {
        //gscoperlocker locker(&sContext.locker);
        if(!source)
            gerror_set(AUDIO_BAD_VALUE);
        else
            source->finish = finish;
    }
}

void gaudio_source_set_error_callback(gsource* source,gerror error)
{
    if(scontext.killed!=1)
        gerror_set(AUDIO_NOT_INIT);
    else
    {
        //gscoperlocker locker(&sContext.locker);
        if(!source)
            gerror_set(AUDIO_BAD_VALUE);
        else
            source->error = error;
    }
}

void gaudio_source_set_position_callback(gsource* source,gposition position)
{
    if(scontext.killed!=1)
        gerror_set(AUDIO_NOT_INIT);
    else
    {
        //gscoperlocker locker(&sContext.locker);
        if(!source)
            gerror_set(AUDIO_BAD_VALUE);
        else
            source->position = position;
    }
}

void gaudio_source_set_float3(gsource* source,int32_t flag,float x,float y,float z)
{
    if(scontext.killed!=1)
        gerror_set(AUDIO_NOT_INIT);
    else
    {
        if(!source)
            gerror_set(AUDIO_BAD_VALUE);
        else
        {
            //gscoperlocker locker(&sContext.locker);

            switch(flag)
            {
            case AUDIO_ENUM_POSITION:
                 source->pos[0] = x;
                 source->pos[1] = y;
                 source->pos[2] = z;
                 break;
            case AUDIO_ENUM_VELOCITY:
                 source->velocity[0] = x;
                 source->velocity[1] = y;
                 source->velocity[2] = z;
                 break;
            default:
                 gerror_set(AUDIO_BAD_TAG);
                 break;
            }
        }
    }
}

void gaudio_source_get_float3(gsource* source,int32_t flag,float* x,float* y,float* z)
{
    if(scontext.killed!=1)
        gerror_set(AUDIO_NOT_INIT);
    else
    {
        if(!source)
            gerror_set(AUDIO_BAD_VALUE);
        else
        {
            //gscoperlocker locker(&sContext.locker);

            switch(flag)
            {
            case AUDIO_ENUM_POSITION:
                 *x = source->pos[0];
                 *y = source->pos[1];
                 *z = source->pos[2];
                 break;
            case AUDIO_ENUM_VELOCITY:
                 *x = source->velocity[0];
                 *y = source->velocity[1];
                 *z = source->velocity[2];
                 break;
            default:
                 gerror_set(AUDIO_BAD_TAG);
                 break;
            }
        }
    }
}

void gaudio_source_set_int32(gsource* source,int32_t flag,int32_t value)
{
    if(scontext.killed!=1)
        gerror_set(AUDIO_NOT_INIT);
    else
    {
        //gscoperlocker locker(&sContext.locker);

        if(!source)
            gerror_set(AUDIO_BAD_VALUE);
        else if(!source->decoder)
            gerror_set(AUDIO_INNER_ERROR);
        else
        {
            switch(flag)
            {
            case AUDIO_ENUM_LOOP:
                source->loop = value;
                break;
            case AUDIO_ENUM_MUTE:
                source->mute = value;
            default:
                gerror_set(AUDIO_BAD_TAG);
                break;
            }
        }
    }
}

void gaudio_source_get_int32(gsource* source,int32_t flag,int32_t* value)
{
    if(scontext.killed!=1)
        gerror_set(AUDIO_NOT_INIT);
    else
    {
        if(!source)
            gerror_set(AUDIO_BAD_VALUE);
        else if(!source->decoder)
            gerror_set(AUDIO_INNER_ERROR);
        else
        {
            //gscoperlocker locker(&sContext.locker);

            switch(flag)
            {
            case AUDIO_ENUM_SAMPLERATE:
            case AUDIO_ENUM_CHANNEL:
            case AUDIO_ENUM_FORMAT:
            case AUDIO_ENUM_POSITION:
            case AUDIO_ENUM_DURATION:
            case AUDIO_ENUM_BITERATE:
                *value = source->decoder->getint(source->decoder,flag);
                break;
            case AUDIO_ENUM_LOOP:
                *value = source->loop;
                break;
            case AUDIO_ENUM_MUTE:
                *value = source->mute;
                break;
            case AUDIO_ENUM_AUDIO:
                *value = source->decoder->decid;
                break;
            case AUDIO_ENUM_PLAYING:
                *value = source->state == STATE_PLAY;
                break;
            default:
                gerror_set(AUDIO_BAD_TAG);
                break;
            }
        }
    }
}

gsource* gaudio_source_post_call(gsource* source)
{
    gdecoder* decoder = source->decoder;
    //gmeta_init(&decoder->metainfo);
    stream_seek((streamBuffer*)decoder->stream,-128,SEEK_END);
    /*stream_read((streamBuffer*)decoder->stream,decoder->metainfo.id3v1,128,1);
    if(!strncmp("TAG",decoder->metainfo.id3v1,3))
        decoder->metainfo.id3v1length = 128;
    else
        decoder->metainfo.id3v1length = 0;*/
    stream_seek((streamBuffer*)decoder->stream,0,SEEK_SET);
    //decoder->metainfo.id3v2length = 0;
    /*decoder->metainfo.id3v2length = id3_header_size_get((streamBuffer*)decoder->stream);
    if(decoder->metainfo.id3v2length != 0)
    {
        stream_seek((streamBuffer*)decoder->stream,decoder->metainfo.id3v2length,SEEK_CUR);
    }*/

    //!000
    int32_t ret1 = decoder->init(decoder);
    uint32_t samplerate = decoder->getint(decoder,AUDIO_ENUM_SAMPLERATE);
    uint32_t channel = decoder->getint(decoder,AUDIO_ENUM_CHANNEL);
    int32_t ret2 = gvalid_check(channel,samplerate);
    if(ret1 == FALSE || ret2 == FALSE)
    {
        stream_seek((streamBuffer*)decoder->stream,0,SEEK_SET);
        stream_close((streamBuffer*)decoder->stream);
        decoder->destroy(decoder);
        gerror_set(AUDIO_NOT_SUPPORT);
        gFreeBuffer(source);
        return NULL;
    }
    if(!gisequal(samplerate,AUDIO_STANDARD_SAMPLERATE))
    {
        decoder->ratio = AUDIO_STANDARD_SAMPLERATE/samplerate;
        decoder->resample[0] = gresample_create(1,decoder->ratio-AUDIO_MIN_DIFF,decoder->ratio+AUDIO_MIN_DIFF);
        decoder->resample[1] = gresample_create(1,decoder->ratio-AUDIO_MIN_DIFF,decoder->ratio+AUDIO_MIN_DIFF);
    }

    scontext.device.source.push_back(source);
    source->init_state(STATE_IDLE);
    source->update = false;
    return source;
}

#define CHECK_STATE_AND_STACK\
    if(scontext.killed!=1)\
    {\
        gerror_set(AUDIO_NOT_INIT);\
        return NULL;\
    }\
    else if(scontext.device.source.size()> AUDIO_MAX_SOURCE)\
    {\
        gerror_set(AUDIO_NOT_SUPPORT);\
        return NULL;\
    }

gsource* gaudio_source_create_from_file(const char* filename,int32_t flag)
{
    CHECK_STATE_AND_STACK

    structStat stat_;
    if(stat(filename,&stat_) || strlen(filename) >= MAX_PATH)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return NULL;
    }

    char* extension = getlastof(filename,'.');
    const char* ext = strlwr(extension);
    gdecoder* decoder = scontext.factory.decoderfactory.create_by_extension(ext);
    if(decoder == NULL)
    {
        gerror_set(AUDIO_NOT_SUPPORT);
        return NULL;
    }

    gsource* source = gcreate_buffer<gsource>(1);

    if(flag == TRUE)
    {
        uint8_t* buffer = gcreate_buffer<uint8_t>(stat_.st_size);
        if(!buffer)
        {
            gFreeBuffer(decoder);
            gFreeBuffer(source);
            gerror_set(AUDIO_OUT_OF_MEMORY);
            return NULL;
        }
        FILE* file = fopen(filename,"rb");
        int32_t ret = fread(buffer,sizeof(uint8_t),stat_.st_size,file);
        if(ret != stat_.st_size)
        {
            gFreeBuffer(buffer);
            gFreeBuffer(decoder);
            gFreeBuffer(source);
            fclose(file);
            gerror_set(AUDIO_INNER_ERROR);
            return NULL;
        }
        fclose(file);
        decoder->stream = stream_open_mem(buffer,ret,TRUE);
    }
    else
        decoder->stream = stream_open_file(filename);

    source->decoder = decoder;
    return gaudio_source_post_call(source);
}

gsource* gaudio_source_create_from_buffer(int8_t* buffer,uint32_t length)
{
    CHECK_STATE_AND_STACK

    gdecoder* decoder = scontext.factory.decoderfactory.create_by_buffer(buffer,length);
    if(decoder == NULL)
    {
        gerror_set(AUDIO_NOT_SUPPORT);
        return NULL;
    }

    decoder->stream = stream_open_mem(buffer,length,FALSE);
    gsource* source = gcreate_buffer<gsource>(1);

    source->decoder = decoder;
    return gaudio_source_post_call(source);
}

gsource* gaudio_source_create_from_buffer2(int32_t samplerate,int32_t channel,int32_t format,int8_t* buffer,int32_t length)
{
    CHECK_STATE_AND_STACK

    if(samplerate < 8000 || samplerate > 48000)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return NULL;
    }

    if(channel < 1 || channel > 2)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return NULL;
    }

    if(format != AUDIO_FORMAT_TYPE_INT8 ||
       format != AUDIO_FORMAT_TYPE_INT16 ||
       format != AUDIO_FORMAT_TYPE_INT24 ||
       format != AUDIO_FORMAT_TYPE_INT32)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return NULL;
    }

    gdecoder* decoder = gdecoder_create_raw(channel,format,samplerate);
    if(decoder == NULL)
    {
        gerror_set(AUDIO_OUT_OF_MEMORY);
        return NULL;
    }

    decoder->stream = stream_open_mem(buffer,length,FALSE);
    gsource* source = gcreate_buffer<gsource>(1);

    source->decoder = decoder;
    int32_t ret1 = decoder->init(decoder);
    int32_t ret2 = gvalid_check(channel,samplerate);
    if(ret1 == FALSE || ret2 == FALSE)
    {
        stream_seek((streamBuffer*)decoder->stream,0,SEEK_SET);
        stream_close((streamBuffer*)decoder->stream);
        decoder->destroy(decoder);
        gerror_set(AUDIO_NOT_SUPPORT);
        gFreeBuffer(source);
        return NULL;
    }
    if(!gisequal(samplerate,AUDIO_STANDARD_SAMPLERATE))
    {
        decoder->ratio = AUDIO_STANDARD_SAMPLERATE/samplerate;
        decoder->resample[0] = gresample_create(1,decoder->ratio-AUDIO_MIN_DIFF,decoder->ratio+AUDIO_MIN_DIFF);
        decoder->resample[1] = gresample_create(1,decoder->ratio-AUDIO_MIN_DIFF,decoder->ratio+AUDIO_MIN_DIFF);
    }
    scontext.device.source.push_back(source);
    source->init_state(STATE_IDLE);
    source->update = false;
    return source;
}

gsource* gaudio_source_create_from_virtual_io(gread read,gseek seek,gtell tell,gclose close,void* ctx)
{
    CHECK_STATE_AND_STACK

    if(read == NULL || seek == NULL || tell == NULL || close == NULL)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return NULL;
    }

    streamBuffer* stream = stream_open_callbacks(read,seek,tell,close,ctx);
    int8_t buffer[4] = {0};
    long size = stream_read(stream,buffer,sizeof(int8_t),4);
    if(size != 4)
    {
        stream_seek(stream,0,SEEK_SET);
        stream_close(stream);
        return NULL;
    }
    gdecoder* decoder = scontext.factory.decoderfactory.create_by_buffer(buffer,size);
    if(decoder == NULL)
    {
        gerror_set(AUDIO_NOT_SUPPORT);
        stream_close(stream);
        return NULL;
    }

    stream_seek(stream,0,SEEK_SET);
    decoder->stream = stream;
    gsource* source = gcreate_buffer<gsource>(1);
    source->decoder = decoder;
    return gaudio_source_post_call(source);
}

#define SOURCE_CHECK(source)\
    if(scontext.killed!=1)\
    {\
        gerror_set(AUDIO_NOT_INIT);\
        return;\
    }\
    if(source == NULL)\
    {\
        gerror_set(AUDIO_BAD_VALUE);\
        return;\
    }

#define SOURCE_SET_STATE(source,flag)\
    if(scontext.driver == 0 && scontext.device.mode == AUDIO_PLAY_MODE)\
    {\
        gerror_set(AUDIO_INVALID_OPERATION);\
        return;\
    }\
    if(scontext.killed!=1)\
    {\
        gerror_set(AUDIO_NOT_INIT);\
        return;\
    }\
    if(source == NULL)\
    {\
        gerror_set(AUDIO_BAD_VALUE);\
        return;\
    }\
    if(source->state == flag)\
    {\
        gerror_set(AUDIO_INVALID_OPERATION);\
        return;\
    }\
    source->state_old = source->state;\
    source->state = flag;

void gaudio_source_play(gsource* source,int32_t flg)
{
    //gscoperlocker locker(&sContext.locker);
    SOURCE_SET_STATE(source,STATE_PLAY)
}

void gaudio_source_play3(gsource* source,int32_t flg)
{
    //gscoperlocker locker(&sContext.locker);
    SOURCE_SET_STATE(source,STATE_PLAY)
    source->is3d = true;
}

void gaudio_source_pause(gsource* source)
{
    //gscoperlocker locker(&sContext.locker);
    SOURCE_SET_STATE(source,STATE_PAUSE)
}

void gaudio_source_stop(gsource* source)
{
    //gscoperlocker locker(&sContext.locker);
    SOURCE_SET_STATE(source,STATE_STOP)
    source->frontbuffer.use = 0;
    //stream_close((streamBuffer*)source->decoder->stream);
}

void gaudio_source_destroy(gsource* source)
{
    SOURCE_SET_STATE(source,STATE_DESTROY)
    /*if(sContext.init!=1)
        gerror_set(AUDIO_NOT_INIT);
    else if(!source)
        gerror_set(AUDIO_BAD_VALUE);
    else
    {
        sContext.device.source.remove(source);
        source->decoder->destroy(source->decoder);
        source->decoder = NULL;
        gFreeBuffer(source);
    }*/
}

void gaudio_source_seek(gsource* source,int32_t newpos)
{
    if(scontext.driver == 0 && scontext.device.mode == AUDIO_PLAY_MODE)
    {
        gerror_set(AUDIO_INVALID_OPERATION);
        return;
    }

    //gscoperlocker locker(&sContext.locker);

    SOURCE_CHECK(source)
    int32_t duration = source->decoder->getint(source->decoder,AUDIO_ENUM_DURATION);
    if(newpos < 0 || newpos > duration)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }
    source->decoder->seek(source->decoder,newpos);
}

void  GAPIENTRY gaudio_source_set_user(gsource* source,void* user)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return;
    }
    if(source == NULL)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return;
    }

    //gscoperlocker locker(&sContext.locker);
    source->user = user;
}

void* GAPIENTRY gaudio_source_get_user(gsource* source)
{
    if(scontext.killed!=1)
    {
        gerror_set(AUDIO_NOT_INIT);
        return 0;
    }
    if(source == NULL)
    {
        gerror_set(AUDIO_BAD_VALUE);
        return 0;
    }
    return source->user;
}
