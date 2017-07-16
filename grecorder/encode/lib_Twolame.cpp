#include "lib_Twolame.h"

lib_Twolame::lib_Twolame()
{
    encode = twolame_init();
    mp2buffer = new unsigned char[4096*2];
    buffer2 = new short[4096];
}

lib_Twolame::~lib_Twolame()
{  
    twolame_close(&encode);    
    delete []mp2buffer; 
    delete []buffer2;                
}

int lib_Twolame::open(const char* snd,int channels,int rate,int bite)
{
    twolame_set_num_channels(encode,channels);
    if(channels == 1) 
    {
        twolame_set_mode(encode,TWOLAME_MONO);
    } 
    else 
    {
       twolame_set_mode(encode,TWOLAME_STEREO);
    }

    twolame_set_in_samplerate(encode,rate);
    twolame_set_out_samplerate(encode,rate);

    twolame_set_bitrate(encode,192);

    if(twolame_init_params(encode) != 0) 
    {
        return 0;
    }

    if((file = fopen(snd,"wb")) == 0) 
    {
        return 0;
    }     
    return 1;
} 

int  lib_Twolame::write(unsigned char* buffer,int len)
{
    len/= (twolame_get_num_channels(encode)*2);        
    int mp2fill_size = twolame_encode_buffer_interleaved(encode,(short*)buffer,len,mp2buffer,4096*2);
    return fwrite(mp2buffer,sizeof(unsigned char),mp2fill_size,file);
}   

void lib_Twolame::close()
{   
    int mp2fill_size = twolame_encode_flush(encode,mp2buffer,4096*2);
    if(file)
        fwrite(mp2buffer,sizeof(unsigned char), mp2fill_size,file);                 
}
 
          
 
