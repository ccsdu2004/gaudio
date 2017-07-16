/*
	GAudio 2.0.6.0, (C)2013 by Eric Du(E)
	This program is a part of the GAudio SDK.
    Use GAudio's feature 
*/

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "gaudio.h"

void print_driver_info(int32_t id,int32_t value,const char* msg);
void print_decoder_info(int32_t id,const char* msg);
void print_encoder_info(int32_t id,const char* msg);
void print_effect_info(int32_t id,const char* msg);

int main(int argc,char* argv[])
{   
    printf("feature of GAudio lib.");
    printf("feature created by D.Eric\n");

#ifdef WIN32
    gaudio_init("addons");
#else
    gaudio_init("/usr/local/lib/gaudio/");
#endif
    
    int32_t value;
    
    gaudio_get_int32(AUDIO_ENUM_VERSION,&value);
    printf("gaudio library version:%d.%d.%d.%d\n",value/1000,value/100-value/1000*10,value%100/10,value%10);
    
    gaudio_get_int32(AUDIO_ENUM_DRIVER,&value);
    print_driver_info(AUDIO_DRIVER_WINMM,    value,"current audio drvier:winmm.\n");
    print_driver_info(AUDIO_DRIVER_DSOUND,   value,"current audio drvier:dsound.\n");
    print_driver_info(AUDIO_DRIVER_OSS,      value,"current audio drvier:oss.\n");
    print_driver_info(AUDIO_DRIVER_ALSA,     value,"current audio drvier:alsa.\n");
    print_driver_info(AUDIO_DRIVER_COREAUDIO,value,"current audio drvier:coreaudio.\n");
    print_driver_info(AUDIO_DRIVER_PORTAUDIO,value,"current audio drvier:portaudio.\n");
 
    print_decoder_info(AUDIO_FORMAT_RAW, "gaudio support input format - raw:%d\n");    
    print_decoder_info(AUDIO_FORMAT_AU,  "gaudio support input format - au:%d\n"); 
    print_decoder_info(AUDIO_FORMAT_AIFF,"gaudio support input format - aiff:%d\n"); 
    print_decoder_info(AUDIO_FORMAT_WAV, "gaudio support input format - wav:%d\n");    
    print_decoder_info(AUDIO_FORMAT_FLAC,"gaudio support input format - flac:%d\n");  
    print_decoder_info(AUDIO_FORMAT_MP2, "gaudio support input format - mp2:%d\n"); 
    print_decoder_info(AUDIO_FORMAT_MP3, "gaudio support input format - mp3:%d\n");
    print_decoder_info(AUDIO_FORMAT_AAC, "gaudio support input format - aac:%d\n");      
    print_decoder_info(AUDIO_FORMAT_APE, "gaudio support input format - ape:%d\n");     
    print_decoder_info(AUDIO_FORMAT_CAF, "gaudio support input format - caf:%d\n");     
    print_decoder_info(AUDIO_FORMAT_OGG, "gaudio support input format - ogg:%d\n");     
    print_decoder_info(AUDIO_FORMAT_VOC, "gaudio support input format - voc:%d\n");     
    print_decoder_info(AUDIO_FORMAT_WV,  "gaudio support input format - wv:%d\n");     
    print_decoder_info(AUDIO_FORMAT_MIDI,"gaudio support input format - midi:%d\n");     
    print_decoder_info(AUDIO_FORMAT_WMA, "gaudio support input format - wma:%d\n");     
    print_decoder_info(AUDIO_FORMAT_MOD, "gaudio support input format - mod:%d\n");     
    print_decoder_info(AUDIO_FORMAT_XM,  "gaudio support input format - xm:%d\n");  
    print_decoder_info(AUDIO_FORMAT_IT,  "gaudio support input format - it:%d\n"); 
    print_decoder_info(AUDIO_FORMAT_S3M, "gaudio support input format - s3m:%d\n");  
    print_decoder_info(AUDIO_FORMAT_WV,  "gaudio support input format - wmv:%d\n"); 
    print_decoder_info(AUDIO_FORMAT_CD,  "gaudio support input format - cd:%d\n");  
    print_decoder_info(AUDIO_FORMAT_VQF, "gaudio support input format - vqf:%d\n"); 
    print_decoder_info(AUDIO_FORMAT_MPC, "gaudio support input format - mpc:%d\n"); 
    print_decoder_info(AUDIO_FORMAT_TTA, "gaudio support input format - tta:%d\n"); 
    print_decoder_info(AUDIO_FORMAT_AC3, "gaudio support input format - ac3:%d\n"); 
     
    print_encoder_info(AUDIO_FORMAT_RAW, "gaudio support output format - raw:%d\n");    
    print_encoder_info(AUDIO_FORMAT_AU,  "gaudio support output format - au:%d\n"); 
    print_encoder_info(AUDIO_FORMAT_AIFF,"gaudio support output format - aiff:%d\n"); 
    print_encoder_info(AUDIO_FORMAT_WAV, "gaudio support output format - wav:%d\n");    
    print_encoder_info(AUDIO_FORMAT_FLAC,"gaudio support output format - flac:%d\n");  
    print_encoder_info(AUDIO_FORMAT_MP2, "gaudio support output format - mp2:%d\n"); 
    print_encoder_info(AUDIO_FORMAT_MP3, "gaudio support output format - mp3:%d\n");
    print_encoder_info(AUDIO_FORMAT_AAC, "gaudio support output format - aac:%d\n");      
    print_encoder_info(AUDIO_FORMAT_APE, "gaudio support output format - ape:%d\n");     
    print_encoder_info(AUDIO_FORMAT_CAF, "gaudio support output format - caf:%d\n");     
    print_encoder_info(AUDIO_FORMAT_OGG, "gaudio support output format - ogg:%d\n");     
    print_encoder_info(AUDIO_FORMAT_VOC, "gaudio support output format - voc:%d\n");     
    print_encoder_info(AUDIO_FORMAT_WV,  "gaudio support output format - wv:%d\n");     
    print_encoder_info(AUDIO_FORMAT_MIDI,"gaudio support output format - midi:%d\n");     
    print_encoder_info(AUDIO_FORMAT_WMA, "gaudio support output format - wma:%d\n");     
    print_encoder_info(AUDIO_FORMAT_MOD, "gaudio support output format - mod:%d\n");     
    print_encoder_info(AUDIO_FORMAT_XM,  "gaudio support output format - xm:%d\n");  
    print_encoder_info(AUDIO_FORMAT_IT,  "gaudio support output format - it:%d\n"); 
    print_encoder_info(AUDIO_FORMAT_S3M, "gaudio support output format - s3m:%d\n");  
    print_encoder_info(AUDIO_FORMAT_WV,  "gaudio support output format - wmv:%d\n"); 
    print_encoder_info(AUDIO_FORMAT_CD,  "gaudio support output format - cd:%d\n");  
    print_encoder_info(AUDIO_FORMAT_VQF, "gaudio support output format - vqf:%d\n"); 
    print_encoder_info(AUDIO_FORMAT_MPC, "gaudio support output format - mpc:%d\n"); 
    print_encoder_info(AUDIO_FORMAT_TTA, "gaudio support output format - tta:%d\n"); 
    print_encoder_info(AUDIO_FORMAT_AC3, "gaudio support output format - ac3:%d\n");  
    
    print_effect_info(AUDIO_EFFECT_PHASER,"gaudio support effect - phaser:%d\n"); 
    print_effect_info(AUDIO_EFFECT_INVERT,"gaudio support effect - invert:%d\n");
    print_effect_info(AUDIO_EFFECT_SWAP_CHANNEL,"gaudio support effect - swap channel:%d\n");        
    print_effect_info(AUDIO_EFFECT_MERGE_CHANNEL,"gaudio support effect - merge channel:%d\n");      
    print_effect_info(AUDIO_EFFECT_SPEED_CHANGER,"gaudio support effect - speed changer:%d\n"); 
    print_effect_info(AUDIO_EFFECT_ECHO,"gaudio support effect - echo:%d\n");
    print_effect_info(AUDIO_EFFECT_REVERB,"gaudio support effect - reverb:%d\n");
    print_effect_info(AUDIO_EFFECT_FFT,"gaudio support effect - fft:%d\n");
    print_effect_info(AUDIO_EFFECT_EQUALIZER,"gaudio support effect - equalizer:%d\n");
    print_effect_info(AUDIO_EFFECT_BASSBOOST,"gaudio support effect - bassboost:%d\n");
    print_effect_info(AUDIO_EFFECT_CHORUS,"gaudio support effect - chorus:%d\n");
    print_effect_info(AUDIO_EFFECT_STEREO_ENHANCE,"gaudio support effect - stereo enhance:%d\n"); 
    print_effect_info(AUDIO_EFFECT_DISTORTION,"gaudio support effect - distortion:%d\n");
    print_effect_info(AUDIO_EFFECT_FOLDBACK_DISTORTION,"gaudio support effect - foldback distortion:%d\n");
    print_effect_info(AUDIO_EFFECT_FLANGER,"gaudio support effect - flanger:%d\n");
    print_effect_info(AUDIO_EFFECT_COMPRESSER,"gaudio support effect - compresser:%d\n");
    print_effect_info(AUDIO_EFFECT_AUTOWAH,"gaudio support effect - autowah:%d\n");
    print_effect_info(AUDIO_EFFECT_MODULATOR,"gaudio support effect - modulator:%d\n");
    print_effect_info(AUDIO_EFFECT_FADE_IN,"gaudio support effect - fade in:%d\n");
    print_effect_info(AUDIO_EFFECT_FADE_OUT,"gaudio support effect - fade out:%d\n");
    print_effect_info(AUDIO_EFFECT_LEVELLER,"gaudio support effect - leveller:%d\n");
    print_effect_info(AUDIO_EFFECT_PCM,"gaudio support effect - pcm:%d\n");
                                                                                                                           
    gaudio_deinit();
    system("PAUSE");
    return EXIT_SUCCESS;
}

void print_driver_info(int32_t id,int32_t value,const char* msg)
{
    if(id == value)printf(msg);     
}

void print_decoder_info(int32_t id,const char* msg)
{
    int32_t value = gaudio_is_support(AUDIO_PLUGIN_AUDIO,id);
    printf(msg,value);   
}

void print_encoder_info(int32_t id,const char* msg)
{
    int32_t value = gaudio_is_support(AUDIO_PLUGIN_RECORDER,id);
    printf(msg,value);   
}

void print_effect_info(int32_t id,const char* msg)
{
    int32_t value = gaudio_is_support(AUDIO_PLUGIN_EFFECT,id);
    printf(msg,value);   
}









