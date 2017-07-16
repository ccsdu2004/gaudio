/*
	GAudio 2.0.6.0, (C)2013 by Eric Du(E)
	This program is a part of the GAudio SDK.
	Basic use of GAudio SDK
*/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <conio.h>
#include "gaudio.h"

void GAPICONV tellpos(gsource* source,int32_t position)
{
    printf("\rplay position:%d",position);
}

int main(int argc,char* argv[])
{
  
#ifdef WIN32
    gaudio_init("addons");
#else
    gaudio_init("/usr/local/lib/gaudio/");
#endif

    int32_t value;
    gaudio_get_int32(AUDIO_ENUM_DRIVER,&value);
    if(value == AUDIO_DRIVER_NULL)
    {
        printf("not found any sound driver available\n");
        gaudio_deinit();
        return -1;         
    }
    
	const char* filename = "..\\media\\trek12.wav";
    gsource* source = NULL;
    int8_t* buffer = NULL;

    FILE* file = fopen((char*)filename,"rb");
    if(!file)
    {
        gaudio_deinit();
        printf("load file:%s error!\n",filename);
        return -1;
    }
    fseek(file,0,SEEK_END);
    uint32_t size = ftell(file);
    buffer = (int8_t*)malloc(sizeof(int8_t)*size);
    fseek(file,0,SEEK_SET);
    fread(buffer,sizeof(int8_t),size,file);
    source = gaudio_source_create_from_buffer(buffer,size);
    fclose(file);
   
    if(source == NULL)
    {
        printf("bad source.\nerror code:%d.\n",gaudio_error_get());
        gaudio_deinit();
        system("PAUSE");
        return -1;
    }

    gaudio_source_set_int32(source,AUDIO_ENUM_LOOP,TRUE);
    gaudio_source_set_position_callback(source,tellpos);
    printf("play filename:%s\n",filename);

    gaudio_source_get_int32(source,AUDIO_ENUM_SAMPLERATE,&value);
    printf("source resample rate:%d\n",value);

    gaudio_source_get_int32(source,AUDIO_ENUM_BITERATE,&value);
    printf("source bitrate:%d\n",value);

    gaudio_source_get_int32(source,AUDIO_ENUM_CHANNEL,&value);
    printf("source channel:%d\n",value);
    gaudio_source_get_int32(source,AUDIO_ENUM_DURATION,&value);
    printf("source duration:%d\n",value);

    geffect* fadein = NULL;
    fadein = gaudio_effect_create(AUDIO_EFFECT_FADE_IN,0);
    if(fadein)
        gaudio_effect_bind_to_source(fadein,source);

    gaudio_set_float(AUDIO_ENUM_GAIN,0.9f);
    gaudio_source_play(source,FALSE);

    printf("\nplaying, press any key to quit.\n");
	getch();

    if(fadein)
        gaudio_effect_unbind_from_source(fadein,source);

    gaudio_source_stop(source);
    gaudio_source_destroy(source);
    free(buffer);
    if(fadein)
        gaudio_effect_destroy(fadein);
    
    gaudio_deinit();
#ifdef WIN32
    system("PAUSE");
#endif
    return EXIT_SUCCESS;
}
