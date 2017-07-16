#include <portaudio/portaudio.h>
#ifdef WIN32
    #include <windows.h>
#else

#endif
#include "portaudio.h"

#define SFIFO_STATIC
#include "sfifo.c"
#define SAMPLE_SIZE			(2)
#define FRAMES_PER_BUFFER	(256)
#define FIFO_DURATION		(0.5f)

typedef struct {
	PaStream *stream;
	sfifo_t fifo;
} buffer_portaudio_t;

#ifdef PORTAUDIO18
#define PaTime PaTimestamp
#define Pa_IsStreamActive Pa_StreamActive
#endif

#ifdef PORTAUDIO18
static int paCallback( void *inputBuffer, void *outputBuffer,
			 unsigned long framesPerBuffer,
			 PaTime outTime, void *userData )
#else
static int paCallback(
    const void *inputBuffer, void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData )
#endif
{
	audio_output_t* ao = (audio_output_t*)userData;
	buffer_portaudio_t *pa = (buffer_portaudio_t*)ao->userptr;
	unsigned long bytes = framesPerBuffer * SAMPLE_SIZE * ao->channels;

	if (sfifo_used(&pa->fifo)<bytes) {
		return 1;
	} else {
		sfifo_read( &pa->fifo, outputBuffer, bytes );
		return 0;
	}
}

int open_portaudio(audio_output_t *ao)
{
	buffer_portaudio_t *pa = (buffer_portaudio_t*)ao->userptr;
	PaError err;

	if(ao->rate > 0 && ao->channels >0)
    {
		err = Pa_OpenDefaultStream(
					&pa->stream,
					0,
					ao->channels,
					paInt16,
					ao->rate,
					FRAMES_PER_BUFFER,
#ifdef PORTAUDIO18
					0,
#endif
					paCallback,
					ao );

		if(err != paNoError)
			return -1;
		sfifo_init(&pa->fifo,ao->rate*FIFO_DURATION*SAMPLE_SIZE*ao->channels);
	}

	return(0);
}

static int get_formats_portaudio(audio_output_t *ao)
{
	return 16;
}

static int write_portaudio(audio_output_t *ao,unsigned char *buf,int len)
{
	buffer_portaudio_t *pa = (buffer_portaudio_t*)ao->userptr;
	PaError err;
	int written;

	while(sfifo_space(&pa->fifo) < len)
    {
#ifdef WIN32
		Sleep((FIFO_DURATION/2) * 1000);
#else
		usleep((FIFO_DURATION/2) * 1000000);
#endif
	}

	written = sfifo_write(&pa->fifo,buf,len);

	err = Pa_IsStreamActive(pa->stream);
	if(err == 0)
    {
		err = Pa_StartStream(pa->stream);
		if(err != paNoError)
			return -1;
	}
    else if(err < 0)
		return -1;
	return written;
}

static int close_portaudio(audio_output_t *ao)
{
	buffer_portaudio_t *pa = (buffer_portaudio_t*)ao->userptr;
	PaError err;

	if(pa->stream)
    {
		if(Pa_IsStreamActive(pa->stream) == 1)
        {
			err = Pa_StopStream(pa->stream);
			if(err != paNoError)
				return -1;
		}

		err = Pa_CloseStream(pa->stream);
		if(err != paNoError)
            return -1;
		pa->stream = NULL;
	}

	sfifo_close(&pa->fifo);
	return 0;
}

static void flush_portaudio(audio_output_t *ao)
{
	buffer_portaudio_t* pa = (buffer_portaudio_t*)ao->userptr;
	PaError err;

	sfifo_flush(&pa->fifo);
	err = Pa_AbortStream(pa->stream);
}

int deinit_portaudio(audio_output_t* ao)
{
	if(ao->userptr)
    {
		free(ao->userptr);
		ao->userptr = NULL;
	}
	Pa_Terminate();
	return 0;
}

int init_portaudio(audio_output_t* ao)
{
	int err = paNoError;

	if(ao == NULL)
        return -1;

	ao->open = open_portaudio;
	ao->flush = flush_portaudio;
	ao->write = write_portaudio;
	ao->get_formats = get_formats_portaudio;
	ao->close = close_portaudio;
	ao->deinit = deinit_portaudio;

	ao->userptr = malloc(sizeof(buffer_portaudio_t));
	if(ao->userptr==NULL)
		return -1;
	memset(ao->userptr,0,sizeof(buffer_portaudio_t));

	err = Pa_Initialize();
	if(err != paNoError)
		return -1;

	return ao->open(ao);
}
