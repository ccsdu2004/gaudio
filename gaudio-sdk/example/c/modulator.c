/*
	GAudio 2.0.6.0, (C)2013 by Eric Du(E)
	This program is a part of the GAudio SDK.
	Use GAudio's modulator effect
*/
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <stdint.h>
#include "gaudio.h"

int main(int argc,char* argv[])
{
    printf("modulator effect created by Eric\n");

    const char* filename = "..\\media\\trek12.wav";

#if(defined(WIN32) || defined(WIN64))
    gaudio_init("addons");
#else
    gaudio_init("/usr/local/lib/gaudio/");
#endif

    //! load soundfont for midi/mid music file
    gaudio_set_string(AUDIO_ENUM_SOUNDFONT,"instruments\\snd.cfg");

    gsource* source = gaudio_source_create_from_file(filename,FALSE);
    if(source == NULL)
    {
        printf("bad source.\nerror code:%d.\n",gaudio_error_get());
        gaudio_deinit();
        return -1;
    }

    gaudio_source_set_int32(source,AUDIO_ENUM_LOOP,TRUE);
    printf("play filename:%s\n",filename);

    geffect* effect = gaudio_effect_create(AUDIO_EFFECT_MODULATOR,0);
    if(!effect)
    {
        printf("not support modulator effect\n");
        gaudio_source_destroy(source);
        gaudio_deinit();
        return -1;
    }

    gaudio_effect_bind_to_source(effect,source);
    gaudio_source_play(source,FALSE);

    printf("\nplaying, press any key to quit.\n");
	getch();

    gaudio_source_stop(source);
    gaudio_source_destroy(source);
    gaudio_effect_destroy(effect);

    gaudio_deinit();
    return EXIT_SUCCESS;
}
