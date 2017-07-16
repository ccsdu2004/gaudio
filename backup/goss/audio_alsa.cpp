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
#include "header.h"
#include <stdlib.h>
#include <ctype.h>
#include <sys/asoundlib.h>

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

int audio_open(struct audio_info_struct *ai)
{
	int err;
	int card=0,device=0;
	char scard[128], sdevice[128];

	if(!ai)
		return -1;
	if(ai->device) {	/* parse ALSA device name */
		if(strchr(ai->device,':')) {	/* card with device */
			strncpy(scard, ai->device, sizeof(scard)-1);
			scard[sizeof(scard)-1] = '\0';
			if (strchr(scard,':')) *strchr(scard,':') = '\0';
			card = snd_card_name(scard);
			if (card < 0) {
				fprintf(stderr, "wrong soundcard number: %s\n", scard);
				exit(1);
			}
			strncpy(sdevice, strchr(ai->device, ':') + 1, sizeof(sdevice)-1);
		} else {
			strncpy(sdevice, ai->device, sizeof(sdevice)-1);
		}
		sdevice[sizeof(sdevice)-1] = '\0';
		device = atoi(sdevice);
		if (!isdigit(sdevice[0]) || device < 0 || device > 31) {
			fprintf(stderr, "wrong device number: %s\n", sdevice);
			exit(1);
		}
	}

	if((err=snd_pcm_open(&ai->handle, card, device, SND_PCM_OPEN_PLAYBACK)) < 0 )
	{
		fprintf(stderr, "open failed: %s\n", snd_strerror(err));
		exit(1);
	}

	if(audio_reset_parameters(ai) < 0)
	{
		audio_close(ai);
		return -1;
	}

	return 0;
}

static void audio_set_playback_params(struct audio_info_struct *ai)
{
	int err;
	snd_pcm_playback_info_t pi;
	snd_pcm_playback_params_t pp;

	if((err=snd_pcm_playback_info(ai->handle, &pi)) < 0 )
	{
		fprintf(stderr, "playback info failed: %s\n", snd_strerror(err));
		return;	/* not fatal error */
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
		return; /* not fatal error */
	}
}

int audio_reset_parameters(struct audio_info_struct *ai)
{
	audio_set_format(ai);
	audio_set_channels(ai);
	audio_set_rate(ai);

	return 0;
}

int audio_rate_best_match(struct audio_info_struct *ai)
{
	return 0;
}

int audio_set_rate(struct audio_info_struct *ai)
{
	int ret;

	if(!ai || ai->rate < 0)
		return -1;

	ai->alsa_format.rate=ai->rate;

	if((ret=snd_pcm_playback_format(ai->handle, &ai->alsa_format)) < 0 )
		return -1;

	audio_set_playback_params(ai);

	return 0;
}

int audio_set_channels(struct audio_info_struct *ai)
{
	int ret;

	if(ai->alsa_format.channels < 0)
		return 0;

	ai->alsa_format.channels = ai->channels;

	if((ret=snd_pcm_playback_format(ai->handle, &ai->alsa_format)) < 0 )
		return -1;

	audio_set_playback_params(ai);

	return 0;
}

int audio_set_format(struct audio_info_struct *ai)
{
	int ret;

	if(ai->format == -1)
		return 0;

	switch(ai->format)
	{
		case AUDIO_FORMAT_SIGNED_16:
		default:
			ai->alsa_format.format=SND_PCM_SFMT_S16_NE;
			break;
		case AUDIO_FORMAT_UNSIGNED_8:
			ai->alsa_format.format=SND_PCM_SFMT_U8;
			break;
		case AUDIO_FORMAT_SIGNED_8:
			ai->alsa_format.format=SND_PCM_SFMT_S8;
			break;
		case AUDIO_FORMAT_ULAW_8:
			ai->alsa_format.format=SND_PCM_SFMT_MU_LAW;
			break;
		case AUDIO_FORMAT_ALAW_8:
			ai->alsa_format.format=SND_PCM_SFMT_A_LAW;
			break;
		case AUDIO_FORMAT_UNSIGNED_16:
			ai->alsa_format.format=SND_PCM_SFMT_U16_NE;
			break;
	}

	if((ret=snd_pcm_playback_format(ai->handle, &ai->alsa_format)) < 0 )
		return -1;

	audio_set_playback_params(ai);

	return 0;
}

int audio_get_formats(struct audio_info_struct *ai)
{
	int i, err;
	int fmt = -1;
	snd_pcm_playback_info_t pi;

	static int fmts[] = {
		AUDIO_FORMAT_SIGNED_16, AUDIO_FORMAT_UNSIGNED_16,
		AUDIO_FORMAT_UNSIGNED_8, AUDIO_FORMAT_SIGNED_8,
		AUDIO_FORMAT_ULAW_8, AUDIO_FORMAT_ALAW_8
	};
	static int afmts[] = {
		SND_PCM_FMT_S16_NE, SND_PCM_FMT_U16_NE,
		SND_PCM_FMT_U8, SND_PCM_FMT_S8,
		SND_PCM_FMT_MU_LAW, SND_PCM_FMT_A_LAW
	};

	if((err=snd_pcm_playback_info(ai->handle, &pi)) < 0 )
	{
		fprintf(stderr, "playback info failed: %s\n", snd_strerror(err));
		return -1;
	}

	for (i = 0; i < 6; i++) {
		if (pi.formats & afmts[i]) {
			if (fmt == -1)
				fmt = 0;
			fmt |= fmts[i];
		}
	}

	return fmt;
}

int audio_play_samples(struct audio_info_struct *ai,unsigned char *buf,int len)
{
	ssize_t ret;

	ret=snd_pcm_write(ai->handle, buf, len);

	return ret;
}

int audio_close(struct audio_info_struct *ai)
{
	int ret;
	ret = snd_pcm_close(ai->handle);
	return ret;
}

int init_portaudio(audio_output_t* ao)
{
	int err = paNoError;

	if(ao == NULL)
        return -1;

	ao->open = audio_open;
	ao->flush = 0;
	ao->write = audio_play_samples;
	ao->get_formats = audio_get_formats;
	ao->close = audio_close;
	ao->deinit = 0;

	ao->userptr = malloc(sizeof(buffer_portaudio_t));
	if(ao->userptr==NULL)
		return -1;
	memset(ao->userptr,0,sizeof(buffer_portaudio_t));

	int ret = ao->open(ao);
	if(ret != 0)
	    return -1;
	ao->format = AUDIO_FORMAT_SIGNED_16;
	audio_reset_parameters(ao);
	return 0;
}
