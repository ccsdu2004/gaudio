#include <sndfile.h>
#include "gaudio.h"
#include "..\libaudio.h"
#include "lib_Sndfile.h"

lib_Sndfile::lib_Sndfile(int enc):file(NULL)
{
     info.format = getSndfileType(enc);
}

lib_Sndfile::~lib_Sndfile()
{
    close();
}

int lib_Sndfile::open(const char* snd,int channels,int rate,int bite)
{
    close();
    info.channels = channels;
    info.samplerate = rate;
	if(info.format == SF_FORMAT_OGG)
		info.format |= SF_FORMAT_VORBIS;
	else
        info.format |= SF_FORMAT_PCM_16;

    if(!sf_format_check(&info))
    {
        return 0;
    }
    file = sf_open(snd,SFM_WRITE,&info);
    if(file)
        return 1;
    return 0;
}

int lib_Sndfile::write(unsigned char* buffer,int len)
{
    return sf_writef_short(file,(short*)buffer,len/(info.channels*2));
}

void lib_Sndfile::close()
{
    if(file)
    {
        sf_close(file);
        file = NULL;
    }
}

