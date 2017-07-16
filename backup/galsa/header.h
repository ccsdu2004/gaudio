#ifndef _PORTAUDIO_H
#define _PORTAUDIO_H
#include <alsa/asoundlib.h>

#define ALSA

#define AUDIO_OUT_HEADPHONES       0x01
#define AUDIO_OUT_INTERNAL_SPEAKER 0x02
#define AUDIO_OUT_LINE_OUT         0x04
#define AUDIO_RATE_TOLERANCE	   3

struct audio_output_t
{
	int (*open)(struct audio_output_t*);
	int (*get_formats)(struct audio_output_t*);
	int (*write)(struct audio_output_t*,unsigned char*,int);
	void(*flush)(struct audio_output_t*);
	int (*close)(struct audio_output_t*);
	int (*deinit)(struct audio_output_t*);
#ifdef AUDIO_USES_FD
    int fn;
#endif
#ifdef SGI
    ALconfig config;
    ALport port;
#endif
    long rate;
    long gain;
    int output;
#ifdef ALSA
    snd_pcm_t *handle;
    snd_pcm_format_t alsa_format;
#endif
    char  device[256];
    int   channels;
    int   format;
    int   private1;
    void* private2;
};

int init_portaudio(audio_output_t* ao);

#endif

