#include <windows.h>
#include "audio.h"
 
#define BUFFER_SIZE 0x10000
#define NUM_BUFFERS 8  
 
struct queue_state
{
	WAVEHDR buffer_headers[NUM_BUFFERS];
	int next_buffer;
	HANDLE play_done_event;
	HWAVEOUT waveout;
};

static int open_win32(struct audio_output_struct* ao)
{
	struct queue_state* state;
	int i;
	MMRESULT res;
	WAVEFORMATEX out_fmt;
	UINT dev_id;

	if(!ao) 
        return -1;
 
	state = calloc(1,sizeof(struct queue_state));
	if(!state) 
        return -1;

	ao->userptr = state;
	for(i = 0; i < NUM_BUFFERS; i++)
	if(!(state->buffer_headers[i].lpData = malloc(BUFFER_SIZE)))
	    return -1;

	state->play_done_event = CreateEvent(0,FALSE,FALSE,0);
	if(state->play_done_event == INVALID_HANDLE_VALUE) 
        return -1;
 
	dev_id = WAVE_MAPPER;
	strcpy(ao->device,"winmm");
 
	out_fmt.wFormatTag = WAVE_FORMAT_PCM;
	out_fmt.wBitsPerSample = 16;
	out_fmt.nChannels = ao->channels;
	out_fmt.nSamplesPerSec = ao->rate;
	out_fmt.nBlockAlign = out_fmt.nChannels*out_fmt.wBitsPerSample/8;
	out_fmt.nAvgBytesPerSec = out_fmt.nBlockAlign*out_fmt.nSamplesPerSec;
	out_fmt.cbSize = 0;

	res = waveOutOpen(&state->waveout,dev_id,&out_fmt,
        (DWORD_PTR)state->play_done_event,0,CALLBACK_EVENT);

	switch(res)
	{
		case MMSYSERR_NOERROR:
			break;
		case MMSYSERR_ALLOCATED:
        case MMSYSERR_NODRIVER:
        case MMSYSERR_NOMEM:
        case WAVERR_BADFORMAT:  
        default:        
			return -1;
	}
 
	ResetEvent(state->play_done_event);
	return 0;
}
 
static int write_win32(struct audio_output_struct* ao,unsigned char* buf,int len)
{
	struct queue_state* state;
	MMRESULT res;
	WAVEHDR* hdr;

	int rest_len;  
	int bufill;   

	if(!ao || !ao->userptr) 
        return -1;
	if(!buf || len <= 0) 
        return 0;

	state = (struct queue_state*)ao->userptr;
	hdr = &state->buffer_headers[state->next_buffer];
 
	while(hdr->dwBufferLength == BUFFER_SIZE && !(hdr->dwFlags & WHDR_DONE))
	{
		WaitForSingleObject(state->play_done_event,INFINITE);
	}

	if(hdr->dwFlags & WHDR_DONE)
	{
		waveOutUnprepareHeader(state->waveout, hdr, sizeof(WAVEHDR));
		hdr->dwFlags = 0;
		hdr->dwBufferLength = 0;
	}

	bufill = BUFFER_SIZE - hdr->dwBufferLength;
	if(len < bufill) 
        bufill = len;

	rest_len = len - bufill;
	memcpy(hdr->lpData + hdr->dwBufferLength, buf, bufill);
	hdr->dwBufferLength += bufill;
	if(hdr->dwBufferLength == BUFFER_SIZE)
	{  
		res = waveOutPrepareHeader(state->waveout, hdr, sizeof(WAVEHDR));
		if(res != MMSYSERR_NOERROR) 
            return -1;

		res = waveOutWrite(state->waveout, hdr, sizeof(WAVEHDR));
		if(res != MMSYSERR_NOERROR) 
            return -1;
 
		state->next_buffer = (state->next_buffer + 1) % NUM_BUFFERS;
	}
 
	if(rest_len && write_win32(ao, buf + bufill,rest_len) < 0)  
	    return -1;
	else    
	    return len;
}

static void flush_win32(struct audio_output_struct *ao)
{
	int i, z;
	struct queue_state* state;

	if(!ao || !ao->userptr) 
        return;
	state = (struct queue_state*)ao->userptr;

    i = 0;
	z = state->next_buffer - 1;
	for(i = 0; i < NUM_BUFFERS; i++)
	{
		if(!state->buffer_headers[i].dwFlags & WHDR_DONE)
			WaitForSingleObject(state->play_done_event,INFINITE);

		waveOutUnprepareHeader(state->waveout, &state->buffer_headers[i],sizeof(WAVEHDR));
		state->buffer_headers[i].dwFlags = 0;
		state->buffer_headers[i].dwBufferLength = 0;
		z = (z + 1) % NUM_BUFFERS;
	}
}

static int close_win32(struct audio_output_struct *ao)
{
	int i;
	struct queue_state* state;

	if(!ao || !ao->userptr) return -1;
	state = (struct queue_state*)ao->userptr;

	flush_win32(ao);
	waveOutClose(state->waveout);
	CloseHandle(state->play_done_event);

	for(i = 0; i < NUM_BUFFERS; i++) 
        free(state->buffer_headers[i].lpData);

	free(ao->userptr);
	ao->userptr = 0;
	return 0;
}

int init_portaudio(audio_output_t* ao)
{
	if(!ao) 
        return -1;
 
	ao->open = open_win32;
	ao->flush = flush_win32;
	ao->write = write_win32;
	ao->close = close_win32;
	return 0;
}
 
int deinit_portaudio(audio_output_t* ao)
{
	return 0;
} 

