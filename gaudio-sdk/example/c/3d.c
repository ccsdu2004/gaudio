/*
	GAudio 2.1.0.8, (C)2013 by Eric Du(E)
	This program is a part of the GAudio SDK.
	Use GAudio's echo effect
*/
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "gaudio.h"

float circle       = 0;
const float radius = 5;

void GAPIENTRY tellpos(gsource* source,int32_t position)
{
    circle += 0.04f;
    float x = radius * cosf(circle);
    float z = radius * sinf(circle * 0.5f);
    gaudio_source_set_float3(source,AUDIO_ENUM_POSITION,x,0,z);
}

int main(int argc,char* argv[])
{
    printf("3d effect created by D.Eric\n");

#if(defined(WIN32) || defined(WIN64))
    gaudio_init("addons");
#else
    gaudio_init("/usr/local/lib/gaudio/");
#endif

    //! load soundfont for midi/mid music file
    gaudio_set_string(AUDIO_ENUM_SOUNDFONT,"instruments\\snd.cfg");

	const char* filename = "..\\media\\intro.wav";

    gsource* source = gaudio_source_create_from_file(filename,FALSE);
    if(source == NULL)
    {
        printf("load file:%s error!\n",filename);
        printf("bad source.\nerror code:%d.\n",gaudio_error_get());
        gaudio_deinit();
        return -1;
    }

    gaudio_source_set_int32(source,AUDIO_ENUM_LOOP,FALSE);
    gaudio_source_set_position_callback(source,tellpos);
    gaudio_listener_set_float3(AUDIO_ENUM_POSITION,0,0,0);
    gaudio_listener_set_float3(AUDIO_ENUM_FORWARD,0,0,1);
    printf("play filename:%s\n",filename);

    gaudio_source_play3(source,FALSE);

    printf("\nplaying, press any key to quit.\n");
	getch();

    gaudio_source_stop(source);
    gaudio_source_destroy(source);

    gaudio_deinit();
    system("PAUSE");
    return EXIT_SUCCESS;
}
