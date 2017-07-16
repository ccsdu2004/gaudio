/*
	GAudio 2.1.0.1, (C)2013 by Eric Du(E)
	This program is a part of the GAudio SDK.
	Use GAudio's hello world
*/
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "gaudio.h"

int main(int argc,char* argv[])
{
#ifdef WIN32
    gaudio_init("addons");
#else
    gaudio_init("/usr/local/lib/gaudio/");
#endif

	const char* filename = "..\\media\\trek12.wav";

    gsource* source = gaudio_source_create_from_file(filename,FALSE);
    if(source == NULL)
    {
        printf("load file:%s error!\n",filename);
        printf("bad source.\nerror code:%d.\n",gaudio_error_get());
        gaudio_deinit();
        return -1;
    }

    printf("play filename:%s\n",filename);

    geffect* effect = gaudio_effect_create(AUDIO_EFFECT_FADE_IN,0);
    if(!effect)
        printf("fadein effect isn't supported.\n");
    else
        gaudio_effect_bind_to_source(effect,source);

    gaudio_source_set_int32(source,AUDIO_ENUM_LOOP,TRUE);
    gaudio_source_play(source,FALSE);

    printf("\nplaying, press any key to quit.\n");
	getch();

    if(effect)
    {
        gaudio_effect_unbind_from_source(effect,source);
        gaudio_effect_destroy(effect);
    }
    gaudio_source_stop(source);
    gaudio_source_destroy(source);

    gaudio_deinit();
    return EXIT_SUCCESS;
}
