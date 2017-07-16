/*
	GAudio 2.0.6.0, (C)2013 by Eric Du(E)
	This program is a part of the GAudio SDK.
	Use GAudio's distortion effect
*/
#include <stdlib.h>
#include <stdio.h>
#ifdef WIN32
    #include <conio.h>
    #include "getopt.h"
#else
    #include<unistd.h>
#endif
#include "gaudio.h"

int main(int argc,char* argv[])
{
    printf("distortion created by D.Eric\n");

    const char* filename = "..\\media\\trek12.wav";

#ifdef WIN32
    gaudio_init("addons");
#else
    gaudio_init("/usr/local/lib/gaudio/");
#endif

    gsource* source = gaudio_source_create_from_file(filename,FALSE);
    if(source == NULL)
    {
        printf("bad source.\nerror code:%d.\n",gaudio_error_get());
        gaudio_deinit();
        return -1;
    }

    gaudio_source_set_int32(source,AUDIO_ENUM_LOOP,FALSE);
    //gaudio_source_set_finish_callback(source,source_playing_callback);

    printf("play filename:%s\n",filename);

    geffect* effect = gaudio_effect_create(AUDIO_EFFECT_DISTORTION,0);
    if(!effect)
    {
        printf("not support echo effect\n");
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
