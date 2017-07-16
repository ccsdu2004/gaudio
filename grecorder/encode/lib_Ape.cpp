#include "lib_Ape.h"
/*
lib_Ape::lib_Ape()
{
    ptr = CreateIAPECompress();
}

lib_Ape::~lib_Ape()
{
}

int lib_Ape::open(const char* snd,int channels,int rate,int bite)
{
    WAVEFORMATEX tmp;
	tmp.cbSize = 0;
    tmp.nSamplesPerSec = rate;
    tmp.wBitsPerSample = 16;
    tmp.nChannels = channels;
    tmp.wFormatTag = 1;
    tmp.nBlockAlign = (tmp.wBitsPerSample / 8) * tmp.nChannels;
    tmp.nAvgBytesPerSec = tmp.nBlockAlign * tmp.nSamplesPerSec;

	//if(level == AUDIO_ENCODE_LEVEL_FAST)
	//	level = COMPRESSION_LEVEL_FAST;
	//else if(level == AUDIO_ENCODE_LEVEL_HIGH)
    //    level = COMPRESSION_LEVEL_HIGH;
    //else if(level == AUDIO_ENCODE_LEVEL_EXTRA_HIGH)
    //    level = COMPRESSION_LEVEL_EXTRA_HIGH;
    //else
    int  level = COMPRESSION_LEVEL_NORMAL;
	ptr->Start(CAPECharacterHelper::GetUTF16FromANSI(snd),
		       &tmp,
               MAX_AUDIO_BYTES_UNKNOWN,
			   level,
               NULL,
		       CREATE_WAV_HEADER_ON_DECOMPRESSION);
    return 1;
}

int  lib_Ape::write(unsigned char* buffer,int len)
{
    return ptr->AddData(buffer,len);
}

void lib_Ape::close()
{
	ptr->Finish(NULL,0,0);
}*/
