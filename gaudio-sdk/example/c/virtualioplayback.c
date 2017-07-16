/*
	GAudio 2.0.6.0, (C)2013 by Eric Du(E)
	This program is a part of the GAudio SDK.
	Basic use of GAudio SDK
*/
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#ifdef _WIN32
    #include <conio.h>
#endif
#include "gaudio.h"

void GAPICONV tellpos(gsource* source,int32_t position)
{
    printf("\rplay position:%d",position);
}

int32_t GAPICONV myread(void* buffer,int32_t size,int32_t count,void* ctx)
{
    FILE* file = (FILE*)ctx;
    return fread(buffer,size,count,file);
}

int32_t GAPICONV myseek(void* ctx,int32_t offset,int32_t flag)
{
    FILE* file = (FILE*)ctx;
    return fseek(file,offset,flag);
}

int32_t GAPICONV mytell(void* ctx)
{
    FILE* file = (FILE*)ctx;
    return ftell(file);
}

int32_t GAPICONV myclose(void* ctx)
{
    FILE* file = (FILE*)ctx;
    fclose(file);
    return 1;
}

int main(int argc,char* argv[])
{
#ifdef WIN32
    gaudio_init("addons");
#else
    gaudio_init("/usr/local/lib/gaudio/");
#endif

    gaudio_set_string(AUDIO_ENUM_SOUNDFONT,"instruments\\snd.cfg");
    
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

    source = gaudio_source_create_from_virtual_io(myread,myseek,mytell,myclose,(void*)file);
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
 
    gaudio_set_float(AUDIO_ENUM_GAIN,0.9f);
    gaudio_source_play(source,FALSE);

    printf("\nplaying, press any key to quit.\n");
	getch();

    gaudio_source_stop(source);
    gaudio_source_destroy(source);
   
    gaudio_deinit();
#ifdef WIN32
    system("PAUSE");
#endif
    return EXIT_SUCCESS;
}
