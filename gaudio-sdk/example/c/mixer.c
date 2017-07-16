/*
	GAudio 2.0.6.0, (C)2013 by Eric Du(E)
	This program is a part of the GAudio SDK.
	Use GAudio's mixer function
*/
#include <stdlib.h>
#include <conio.h>
#include <stdio.h>
#include <stdint.h>
#ifdef WIN32
    #include "getopt.h"
#else
    #include<unistd.h>
#endif
#include "..\include\gaudio.h"

void usage() 
{
	printf("\rUsage:\tmixer -a file1 -b file2\n\n");
    printf("\t-h\tprint this help\n");
    printf("\t-a\tfor input file 1\n");
    printf("\t-a\tfor input file 2\n");
    printf("\t press any key for stop when playing");
}

int main(int argc,char* argv[])
{
    if(argc < 3)
    {   
        usage();
        return -1;
    }
    
    char* file1 = 0;
    char* file2 = 0;
    
    int ch;  
    opterr = 0;  
    while((ch = getopt(argc,argv,"a:b:h"))!=-1)  
    {  
        switch(ch)  
        {  
        case 'a':  
            file1 = optarg;  
            break;  
        case 'b':  
            file2 = optarg;    
            break;  
        case 'h':
            usage();
            break;     
        default:       
            usage();  
            return -1;
        }  
    }    
    
    printf("mixer created by D.Eric\n");

#ifdef WIN32
    gaudio_init("addons");
#else
    gaudio_init("/usr/local/lib/gaudio/");
#endif

    gaudio_set_string(AUDIO_ENUM_SOUNDFONT,"instruments\\snd.cfg");

    gsource* source[2] = {0};
    source[0] = gaudio_source_create_from_file(file1,FALSE);
    
    if(source[0] == NULL)
    {
        printf("bad source.\nerror code:%d.\n",gaudio_error_get());
        gaudio_deinit();
        return -1;
    }    
    
    gaudio_source_set_int32(source[0],AUDIO_ENUM_LOOP,1);
    source[1] = gaudio_source_create_from_file(file2,FALSE);
    gaudio_source_set_int32(source[1],AUDIO_ENUM_LOOP,1);

    if(source[1] == NULL)
    {
        printf("bad source.\nerror code:%d.\n",gaudio_error_get());
        gaudio_source_destroy(source[0]);
        gaudio_deinit();
        return -1;
    }
    
    gaudio_source_play(source[0],FALSE);
    gaudio_source_set_float(source[0],AUDIO_ENUM_GAIN,0.6f);
    gaudio_source_seek(source[1],1000);
    gaudio_source_set_float(source[1],AUDIO_ENUM_GAIN,0.2f);
    gaudio_source_play(source[1],FALSE);

    printf("\nplaying, press any key to quit.\n");
	getch();

    gaudio_source_stop(source[0]);
    gaudio_source_destroy(source[0]);
    gaudio_source_stop(source[1]);
    gaudio_source_destroy(source[1]);
    
    gaudio_deinit();
    system("PAUSE");
    return EXIT_SUCCESS;
}
