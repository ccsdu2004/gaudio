#include <algorithm>
#include "gmain.h"
#include "gmixer.h"
#include "gcontext.h"
#include "gdecoder.h"
#include "gmeta.h"
#include "gresample.h"
#include "gpanprocess.h"

void gtobuffer(gsource* source);

void gdecprocess()
{
    //gscoperlocker locker(&scontext.locker);

    std::list<gsource*>::iterator itr = scontext.device.source.begin();
    while(itr != scontext.device.source.end())
    {
        gsource* source = *itr;
        if(source != NULL && source->state == STATE_PLAY)
        {
            gtobuffer(source);
            if(source->is3d)
                g3dEffectCalc(source);
            source->applyeffect();

            if(!gisequal(source->gain,1.0f))
            {
                gpan _(source->gain);
                std::for_each(source->frontbuffer.data[0],source->frontbuffer.data[0]+source->frontbuffer.use,_);
                std::for_each(source->frontbuffer.data[1],source->frontbuffer.data[1]+source->frontbuffer.use,_);
            }

            //apply pan
            if(!gisequal(source->pan,AUDIO_PAN_DEFAULT))
            {
                float pan = source->pan;
                float newgain = AUDIO_PAN_HIGH - fabs(pan);
                newgain /= AUDIO_PAN_HIGH;

                float gain[2] = {source->gain,source->gain};
                gain[pan < 0 ? 0 : 1] = newgain;
                gpan _1(gain[0]);
                gpan _2(gain[1]);
                std::for_each(source->frontbuffer.data[0],source->frontbuffer.data[0]+source->frontbuffer.use,_1);
                std::for_each(source->frontbuffer.data[1],source->frontbuffer.data[1]+source->frontbuffer.use,_2);
            }
            itr++;
        }
        else if(source->state == STATE_DESTROY)
        {
            //gmeta_deinit(&source->decoder->metainfo);
            itr = scontext.device.source.erase(itr);

            if(source->decoder->resample[0] != NULL)
            {
                 gresample_close(source->decoder->resample[0]);
                 gresample_close(source->decoder->resample[1]);
                 source->decoder->resample[0] = 0;
                 source->decoder->resample[1] = 0;
            }

            stream_close((streamBuffer*)source->decoder->stream);
            source->decoder->stream = 0;

            source->decoder->destroy(source->decoder);
            source->decoder = NULL;
            gFreeData(source);
        }
    }
}

void gdemuxer(int32_t channel,const float* input,gdatabuffer<float>& output,uint32_t count)
{
    count /= channel;
    if(channel == 2)
    {
        uint32_t m = 0;
        uint32_t n = 0;
        for(uint32_t i=0;i<count;i++)
        {
            output.data[0][m++] = *(input++);
            output.data[1][n++] = *(input++);
        }
    }
    else if(channel == 1)
    {
        std::copy(input,input+count,output.data[0]);
        std::copy(input,input+count,output.data[1]);
    }
}

void gtobuffer(gsource* source)
{
    gdecoder* decoder = source->decoder;
    gdecoder_int32 getint = decoder->getint;
    uint32_t channel = getint(decoder,AUDIO_ENUM_CHANNEL);
    uint32_t samplerate = getint(decoder,AUDIO_ENUM_SAMPLERATE);
    int32_t length = get_suitable_buffer_size(channel,samplerate);
    float buffer[BUFFER_SIZE];

    int32_t size = source->decoder->read(decoder,buffer,length);
    gdemuxer(channel,buffer,source->frontbuffer,size);
    source->frontbuffer.use = size/channel;

    if(size != length && !source->loop)
    {
        source->applystate(STATE_IDLE);
        if(source->finish)
            source->finish(source);
    }
    else if(size != length)
    {
        source->curpos = 0;
        source->decoder->seek(source->decoder,0);
    }
//--
    /*if(size == 0 && !source->loop)
    {
        source->applystate(STATE_IDLE);
        source->finish(source);
    }
    else if(size == 0)
    {
        source->curpos = 0;
        source->decoder->seek(source->decoder,0);
    }*/
// --/*
    if(source->decoder->resample[0])
    {
        float left[BUFFER_SIZE];
        float right[BUFFER_SIZE];
        int32_t used[2];

        float ratio = source->decoder->ratio;
        int32_t use = source->frontbuffer.use;
        void** handle = source->decoder->resample;

        float* in1 = source->frontbuffer.data[0];
        int32_t ret1 = gresample_process(handle[0],ratio,in1,use,size != length,&used[0],left,BUFFER_SIZE);

        float* in2 = source->frontbuffer.data[1];
        int32_t ret2 = gresample_process(handle[1],ratio,in2,use,size != length,&used[1],right,BUFFER_SIZE);
        std::copy(left,left+ret1,source->frontbuffer.data[0]);
        std::copy(right,right+ret2,source->frontbuffer.data[1]);
        source->frontbuffer.use = ret1;
    }

    source->curpos += size;

    if(source->position)
    {
        uint32_t current = source->decoder->getint(source->decoder,AUDIO_ENUM_POSITION);
        (source->position)(source,current);
    }
}









