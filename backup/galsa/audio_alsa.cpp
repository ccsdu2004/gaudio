/*
 *  Driver for Advanced Linux Sound Architecture, http://alsa.jcu.cz
 *
 *  Code by Anders Semb Hermansen <ahermans@vf.telia.no>
 *  Cleanups by Jaroslav Kysela <perex@jcu.cz>
 *              Ville Syrjala <syrjala@sci.fi>
 *
 *  You can use -a <soundcard #>:<device #>...
 *  For example: mpg123 -a 1:0 aaa.mpg
 *               mpg123 -a guspnp:1 aaa.mpg
 *
 * This file comes under GPL license.
 */
#include <stdlib.h>
#include <ctype.h>
extern "C"
{
#include <alsa/asoundlib.h>
}

#include "header.h"

#ifdef SND_LITTLE_ENDIAN
#define SND_PCM_SFMT_S16_NE SND_PCM_SFMT_S16_LE
#define SND_PCM_SFMT_U16_NE SND_PCM_SFMT_U16_LE
#define SND_PCM_FMT_S16_NE SND_PCM_FMT_S16_LE
#define SND_PCM_FMT_U16_NE SND_PCM_FMT_U16_LE
#else
#define SND_PCM_SFMT_S16_NE SND_PCM_SFMT_S16_BE
#define SND_PCM_SFMT_U16_NE SND_PCM_SFMT_U16_BE
#define SND_PCM_FMT_S16_NE SND_PCM_FMT_S16_BE
#define SND_PCM_FMT_U16_NE SND_PCM_FMT_U16_BE
#endif

#define SFIFO_STATIC
#include "sfifo.c"
#define SAMPLE_SIZE			(2)
#define FRAMES_PER_BUFFER	(256)
#define FIFO_DURATION		(0.5f)

int audio_close(struct audio_output_t *ai)
{
	int ret;
	snd_pcm_drain(ai->handle);
	ret = snd_pcm_close(ai->handle);
	return ret;
}

int audio_play(struct audio_output_t* ai,unsigned char* buffer,int length)
{
    ssize_t ret;
	ret = snd_pcm_writei(ai->handle,buffer,length);
	return ret;
}

static void audio_set_playback_params(struct audio_output_t *ai)
{
	int err;
	snd_pcm_playback_info_t pi;
	snd_pcm_playback_params_t pp;

	if((err=snd_pcm_playback_info(ai->handle, &pi)) < 0 )
	{
		fprintf(stderr, "playback info failed: %s\n", snd_strerror(err));
		return;
	}

	bzero(&pp, sizeof(pp));
	pp.fragment_size = pi.buffer_size/4;
	if (pp.fragment_size > pi.max_fragment_size) pp.fragment_size = pi.max_fragment_size;
	if (pp.fragment_size < pi.min_fragment_size) pp.fragment_size = pi.min_fragment_size;
	pp.fragments_max = -1;
	pp.fragments_room = 1;

	if((err=snd_pcm_playback_params(ai->handle, &pp)) < 0 )
	{
		fprintf(stderr, "playback params failed: %s\n", snd_strerror(err));
		return;
	}
}



