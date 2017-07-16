#include "lib_Flac.h"

lib_Flac::lib_Flac()
{
	encoder = FLAC__stream_encoder_new();
    ok = FLAC__stream_encoder_set_verify(encoder,true);
}

lib_Flac::~lib_Flac()
{
    FLAC__stream_encoder_delete(encoder);
}

int lib_Flac::open(const char* snd,int channels,int rate,int bite)
{
    if(!encoder)return 0;

    ok &= FLAC__stream_encoder_set_verify(encoder,true);
	//ok &= FLAC__stream_encoder_set_compression_level(encoder,compress);
	ok &= FLAC__stream_encoder_set_channels(encoder,channels);
	ok &= FLAC__stream_encoder_set_bits_per_sample(encoder,16);
	ok &= FLAC__stream_encoder_set_sample_rate(encoder,rate);

	if(ok)
    {
		status = FLAC__stream_encoder_init_file(encoder,snd,NULL,NULL);
		if(status != FLAC__STREAM_ENCODER_INIT_STATUS_OK)
        {
		    ok = 0;
		}
	}
    return ok;
}

int  lib_Flac::write(unsigned char* buffer,int len)
{
    if(encoder == NULL)return 0;
    size_t i;
    int channels = FLAC__stream_encoder_get_channels(encoder);
    len/=(2*channels);
    for(i = 0; i < len*channels; i++)
    {
        pcm[i] = (FLAC__int32)(((FLAC__int16)(FLAC__int8)buffer[2*i+1] << 8) | (FLAC__int16)buffer[2*i]);
    }
    ok = FLAC__stream_encoder_process_interleaved(encoder,pcm,len);
    return len;
}

void lib_Flac::close()
{
    ok &= FLAC__stream_encoder_finish(encoder);
}


