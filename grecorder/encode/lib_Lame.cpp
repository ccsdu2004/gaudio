#include "lib_Lame.h"

lib_Lame::lib_Lame()
{
	lame = lame_init();
	//lame_set_write_id3tag_automatic(lame,0);
	file = NULL;
	buffer = new unsigned char[MP3BUFSIZE];
}

lib_Lame::~lib_Lame()
{
    lame_close(lame);
	delete []buffer;
}

int lib_Lame::open(const char* name,int channels,int rate,int bite)
{
	lame_set_num_channels(lame,channels);
    lame_set_quality(lame,5);
	lame_set_in_samplerate(lame,rate);
	lame_set_out_samplerate(lame,rate);
	lame_set_brate(lame,128);
	if(channels == 1)
	    lame_set_mode(lame,MONO);
    else
        lame_set_mode(lame,STEREO);
	int ret = lame_init_params(lame);
	if(ret < 0)
		return 0;
	file = fopen(name,"wb");
	if(file == NULL)
		return 0;
	return 1;
}

union Buffer
{
    unsigned char* uint8Buffer;
    short*         int16Buffer;
};

int  lib_Lame::write(unsigned char* pcm,int len)
{
    int channel = lame_get_num_channels(lame);
    int size =  channel == 1 ? len/2 : len/channel/2;
    Buffer thisBuffer;
    thisBuffer.uint8Buffer = pcm;
    int bytes = 0;
    if(channel == 2)
         bytes = lame_encode_buffer_interleaved(lame,thisBuffer.int16Buffer,size,buffer,MP3BUFSIZE);
    else
         bytes = lame_encode_buffer(lame,thisBuffer.int16Buffer,thisBuffer.int16Buffer,size,buffer,MP3BUFSIZE);
    if(bytes <= 0)
	{
        return 0;
    }
	else
	{
        return fwrite(buffer,1,bytes,file);
    }
}

void lib_Lame::close()
{
    int bytes = lame_encode_flush(lame,buffer,sizeof(buffer));
    if(bytes > 0)
	{
        fwrite(buffer,1,bytes,file);
    }
	fclose(file);
	file = NULL;
}
