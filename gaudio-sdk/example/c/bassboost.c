/*
	GAudio 2.0.6.0, (C)2013 by Eric Du(E)
	This program is a part of the GAudio SDK.
	Use GAudio's bassboost effect
*/
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include "gaudio.h"

int main(int argc,char* argv[])
{
    printf("bassboost created by GAudio Group\n");

#if(defined(WIN32) || defined(WIN64))
    gaudio_init("addons");
#else
    gaudio_init("/usr/local/lib/gaudio/");
#endif

    const char* filename = "..\\media\\hurnscald.ogg";
    gsource* source = gaudio_source_create_from_file(filename,FALSE);
    if(source == NULL)
    {
        printf("bad source.\nerror code:%d.\n",gaudio_error_get());
        gaudio_deinit();
        return -1;
    }

    //! set source as loop mode.
    gaudio_source_set_int32(source,AUDIO_ENUM_LOOP,TRUE);
    //gaudio_source_set_finish_callback(source,source_playing_callback);
    printf("play filename:%s\n",filename);

    //! create bassboost effect
    geffect* effect = gaudio_effect_create(AUDIO_EFFECT_BASSBOOST,0);
    if(!effect)
    {
        printf("not support bassboost effect\n");
        gaudio_source_destroy(source);
        gaudio_deinit();
        return -1;
    }

    //! set bassboost parameter.
    gaudio_effect_set_float(effect,AUDIO_EFFECT_BASSBOOST_FREQUENCE,240.0f);
    gaudio_effect_set_float(effect,AUDIO_EFFECT_BASSBOOST_GAIN,24.0f);

    //! set bassboost as a source one.
    gaudio_effect_bind_to_source(effect,source);
    //! play source.
    gaudio_source_play(source,FALSE);

    printf("\nplaying, press any key to quit.\n");
	getch();

    //! ubind effect from source
    gaudio_effect_unbind_from_source(effect,source);
    //! stop play and destroy it.
    gaudio_source_stop(source);
    gaudio_source_destroy(source);
    //! deinit gaudio library.
    gaudio_deinit();
    system("PAUSE");
    return EXIT_SUCCESS;
}
