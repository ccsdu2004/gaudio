#ifndef _PORTAUDIO_H
#define _PORTAUDIO_H

#define AUDIO_OUT_HEADPHONES       0x01
#define AUDIO_OUT_INTERNAL_SPEAKER 0x02
#define AUDIO_OUT_LINE_OUT         0x04
#define AUDIO_RATE_TOLERANCE	  3

struct audio_output_t
{
	int (*open)(struct audio_output_t*);
	int (*get_formats)(struct audio_output_t*);
	int (*write)(struct audio_output_t*,unsigned char*,int);
	void(*flush)(struct audio_output_t*);
	int (*close)(struct audio_output_t*);
	int (*deinit)(struct audio_output_t*);

	long  rate;
	int   channels;
	int   format;
	int   is_open;
	void* userptr;
};

int init_portaudio(audio_output_t* ao);

#endif

