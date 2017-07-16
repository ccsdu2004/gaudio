#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gaddons.h"
#include "gdecreator.h"
#include "streamBuffer.h"
#include "gmain.h"
#include "timidity.h"

#ifdef HAVE_MIDI

gdecoder* gdecoder_midi_create();

gxcreator(midi)

int32_t gmididecreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_MIDI)
       return TRUE;
    return FALSE;
}

gdecoder* gmididecreator::create_by_extension(const char* extension)
{
    if(!strncmp(extension,"mid",3) || !strncmp(extension,"midi",4))
        return gdecoder_midi_create();
    return NULL;
}

gdecoder* gmididecreator::create_by_decid(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_MIDI)
       return gdecoder_midi_create();
    return NULL;
}

gdecoder* gmididecreator::create_by_buffer(int8_t* buffer,uint32_t length)
{
    if(buffer[0] == 'M' && buffer[1] == 'T' && buffer[2] == 'h' && buffer[3] == 'd')
        return gdecoder_midi_create();
    return NULL;
}

void gmididecreator::destroy()
{
    delete this;
}

const char* GAPIENTRY decoder_midi_maker(gdecoder*)
{
    return "duwenhua2013";
}

struct gdecoderimpl
{
    uint32_t format;
    uint32_t channel;
    uint32_t samplerate;
    uint32_t duration;
    MidSong* song;
    gdecoderimpl():
    format(2),
    channel(2),
    samplerate(44100),
    duration(0),
    song(NULL)
    {
    }
};

void GAPIENTRY decoder_midi_destroy(gdecoder* decoder)
{
    mid_exit();
    gdecoderimpl* impl = decoder->impl;
    delete impl;
    impl = 0;
    delete decoder;
}

uint32_t GAPIENTRY decoder_midi_get_int32(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
        return decoder->impl->channel;
    else if(flag == AUDIO_ENUM_SAMPLERATE)
        return decoder->impl->samplerate;
    else if(flag == AUDIO_ENUM_FORMAT)
        return AUDIO_FORMAT_TYPE_INT16;
    else if(flag == AUDIO_ENUM_DURATION)
        return decoder->impl->duration;
    else if(flag == AUDIO_ENUM_POSITION)
        return mid_song_get_time(decoder->impl->song);
    else if(flag == AUDIO_ENUM_BITERATE)
        return decoder->impl->channel*decoder->impl->samplerate*decoder->impl->format;
    return FALSE;
}

int32_t GAPIENTRY decoder_midi_init(gdecoder* decoder)
{
    decoder->impl->format = 16;
    decoder->impl->channel = 2;
    decoder->impl->samplerate = 44100;
    MidSongOptions options;
    options.rate = decoder->impl->samplerate;
    options.format = MID_AUDIO_S16LSB;
    //options.format = (bits == 16) ? MID_AUDIO_S16LSB : MID_AUDIO_S8;
    options.channels = decoder->impl->channel;
    options.buffer_size = 4096 / (16 * decoder->impl->channel / 8);
    decoder->impl->song = mid_song_load((streamBuffer*)decoder->stream,&options);
    if(decoder->impl->song == NULL)
        return FALSE;
    mid_song_set_volume(decoder->impl->song,100);
    mid_song_start(decoder->impl->song);

    decoder->impl->duration = mid_song_get_total_time(decoder->impl->song);
    return TRUE;
}

int32_t GAPIENTRY decoder_midi_seek(gdecoder* decoder,uint32_t position)
{
    mid_song_seek(decoder->impl->song,position);
    if(position == 0)
        mid_song_start(decoder->impl->song);
    return TRUE;
}

uint32_t GAPIENTRY decoder_midi_read(gdecoder* decoder,float* pcm,int32_t length)
{
    short* buffer = new short[length];
    int32_t read = mid_song_read_wave(decoder->impl->song,buffer,sizeof(short)*length);
    read /= sizeof(short);
    for(int i=0;i<read;i++)
        pcm[i] = gtofloat(buffer[i]);
    delete[] buffer;
    return read;
}

gdecoder* gdecoder_midi_create()
{
    gdecoder* decoder = new gdecoder();
    decoder->meta = 0;
    decoder->destroy = decoder_midi_destroy;
    decoder->init = decoder_midi_init;
    decoder->seek = decoder_midi_seek;
    decoder->read = decoder_midi_read;
    decoder->resample[0] = decoder->resample[1] = 0;
    decoder->decid = AUDIO_FORMAT_MIDI;
    decoder->version = 1012;
    decoder->maker = decoder_midi_maker;

    decoder->stream = 0;
    decoder->ratio = 1.0f;
    decoder->impl = new gdecoderimpl();
    decoder->getint = decoder_midi_get_int32;
    return decoder;
}

static gaddons midi_addon;

gaddons* GAPIENTRY gmidi_instance(void* ctx)
{
    mid_init(sContext.soundfont);
    midi_addon.handle = ctx;
    midi_addon.flag = AUDIO_PLUGIN_AUDIO;
    midi_addon.addon = new gmididecreator();
    return &midi_addon;
}

#endif
//!by duwenhua2013




