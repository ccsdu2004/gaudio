#ifndef LAME_ENCODER_H
#define LAME_ENCODER_H
#include <lame.h>
#include "lib_Encoder.h"

#define INBUFSIZE 4096
#define MP3BUFSIZE (int)(1.25 * INBUFSIZE) + 7200

class lame_Encoder : public lib_Encoder
{
public:
    lame_Encoder()
	{
	    lame = lame_init();
		file = NULL;
		buffer = new unsigned char[MP3BUFSIZE];
	}
    ~lame_Encoder()
	{
        lame_close(lame);
	    delete []buffer;
	}
public:
    bool open(const char* name,int channels,int rate,int bite)
	{
	    lame_set_num_channels(lame,channels);
		lame_set_in_samplerate(lame,rate);

	    int ret = lame_init_params(lame);
	    if(ret < 0)
		    return false;
		file = fopen(name,"wb");
		if(file == NULL)
		    return false;
		return true;
	}
    int  write(unsigned char* pcm,int len)
	{
        int bytes = lame_encode_buffer_interleaved(lame,(short*)pcm,len/2,buffer,MP3BUFSIZE);
        if(bytes <= 0)
		{
            return 0;
        }
		else
		{
            return fwrite(buffer,1,bytes,file);
        }
	}
    void close()
	{
        int bytes = lame_encode_flush(lame,buffer,sizeof(buffer));
        if(bytes > 0)
	    {
            fwrite(buffer,1,bytes,file);
        }
	}
private:
    lame_global_flags* lame;
	FILE*              file;
	unsigned char*     buffer;
};

#endif
//! ccsdu2004
