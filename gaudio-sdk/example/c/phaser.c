/*
	GAudio 2.0.6.0, (C)2013 by Eric Du(E)
	This program is a part of the GAudio SDK.
	Use GAudio's phaser effect
*/
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "gaudio.h"

int main(int argc,char* argv[])
{
    printf("phaser created by D.Eric\n");

    const char* filename = "..\\media\\trek12.wav";

#ifdef WIN32
    gaudio_init("addons");
#else
    gaudio_init("/usr/local/lib/gaudio/");
#endif

    //! load soundfont for midi/mid music file
    gaudio_set_string(AUDIO_ENUM_SOUNDFONT,"instruments\\snd.cfg");

    gsource* source = gaudio_source_create_from_file(filename,FALSE);
    if(source == NULL)
    {
        printf("load file:%s error!\n",filename);
        printf("bad source.\nerror code:%d.\n",gaudio_error_get());
        gaudio_deinit();
        return -1;
    }

    gaudio_source_set_int32(source,AUDIO_ENUM_LOOP,TRUE);
    printf("play filename:%s\n",filename);

    geffect* effect = gaudio_effect_create(AUDIO_EFFECT_PHASER,0);
    if(!effect)
    {
        printf("not support autowah effect\n");
        gaudio_source_destroy(source);
        gaudio_deinit();
        return -1;
    }

    gaudio_effect_bind(effect,0);
    gaudio_source_play(source,FALSE);

    printf("\nplaying, press any key to quit.\n");
	getch();

    gaudio_effect_unbind(effect);
    gaudio_effect_destroy(effect);
    gaudio_source_stop(source);
    gaudio_source_destroy(source);

    gaudio_deinit();
    system("PAUSE");
    return EXIT_SUCCESS;
}
