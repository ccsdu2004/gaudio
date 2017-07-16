#include <stdlib.h>
/*
	GAudio 2.0.6.0, (C)2013 by Eric Du(E)
	This program is a part of the GAudio SDK.
	Use GAudio's recorder function
*/
#include <string.h>
#include <conio.h>
#include <stdio.h>
#ifdef WIN32
    #include "getopt.h"
#else
    #include<unistd.h>
#endif
#include "gaudio.h"

void usage()
{
	printf("\rUsage:\trecorder -i input -o output -f format\n\n");
    printf("\t-h\tprint this help\n");
    printf("\t-i\tfor input file\n");
    printf("\t-o\tfor output file\n");
    printf("\t-f\tfor audio format eg.MP3,APE...\n");
    printf("\tfor instance : recorder -i D:/mylove.mpe -o D:/mylove.ape -f APE\n");
}

int32_t flag = 1;

void GAPIENTRY source_finish_callback(gsource* source)
{
    flag = 0;
    printf("converting finished\n");
}

int32_t to_format_string(const char* str)
{   
    char string[255] = {0};
    strcpy(string,str);
    strupr(string);
    
#define CHECK_FORMAT(ext,s,id)\
    if(!strcmp(ext,s))\
        format = id;    
        
    int32_t format = -1;
    CHECK_FORMAT("WAV",string,AUDIO_FORMAT_WAV)
    CHECK_FORMAT("AU",string,AUDIO_FORMAT_AU)
    CHECK_FORMAT("AIFF",string,AUDIO_FORMAT_AIFF)
    CHECK_FORMAT("FLAC",string,AUDIO_FORMAT_FLAC)
    CHECK_FORMAT("MP2",string,AUDIO_FORMAT_MP2)
    CHECK_FORMAT("MP3",string,AUDIO_FORMAT_MP3)
    CHECK_FORMAT("APE",string,AUDIO_FORMAT_APE)    
    CHECK_FORMAT("AAC",string,AUDIO_FORMAT_AAC)
    CHECK_FORMAT("CAF",string,AUDIO_FORMAT_CAF)
    CHECK_FORMAT("OGG",string,AUDIO_FORMAT_OGG)
    CHECK_FORMAT("VOC",string,AUDIO_FORMAT_VOC)
    CHECK_FORMAT("WV",string,AUDIO_FORMAT_WV)
    CHECK_FORMAT("MIDI",string,AUDIO_FORMAT_MIDI)
    CHECK_FORMAT("XM",string,AUDIO_FORMAT_XM)
    CHECK_FORMAT("S3M",string,AUDIO_FORMAT_S3M)
    CHECK_FORMAT("MOD",string,AUDIO_FORMAT_MOD)
    CHECK_FORMAT("IT",string,AUDIO_FORMAT_IT)
    CHECK_FORMAT("WMA",string,AUDIO_FORMAT_WMA)
    CHECK_FORMAT("WMV",string,AUDIO_FORMAT_WMV)
    CHECK_FORMAT("CD",string,AUDIO_FORMAT_CD)
    CHECK_FORMAT("VQF",string,AUDIO_FORMAT_VQF)   
    CHECK_FORMAT("MPC",string,AUDIO_FORMAT_MPC)
    CHECK_FORMAT("TTA",string,AUDIO_FORMAT_TTA)
    CHECK_FORMAT("AC3",string,AUDIO_FORMAT_AC3)
    CHECK_FORMAT("TAK",string,AUDIO_FORMAT_TAK)
    CHECK_FORMAT("RM",string,AUDIO_FORMAT_RM)
    CHECK_FORMAT("SND",string,AUDIO_FORMAT_SND)
    CHECK_FORMAT("SVX",string,AUDIO_FORMAT_SVX)
    CHECK_FORMAT("PAF",string,AUDIO_FORMAT_PAF)
    CHECK_FORMAT("FAP",string,AUDIO_FORMAT_FAP)
    CHECK_FORMAT("HTK",string,AUDIO_FORMAT_HTK)
    CHECK_FORMAT("SF",string,AUDIO_FORMAT_SF)
    CHECK_FORMAT("W64",string,AUDIO_FORMAT_W64)
    CHECK_FORMAT("MAT4",string,AUDIO_FORMAT_MAT4)
    CHECK_FORMAT("MAT5",string,AUDIO_FORMAT_MAT5)
    CHECK_FORMAT("PVF",string,AUDIO_FORMAT_PVF)
    CHECK_FORMAT("XI",string,AUDIO_FORMAT_XI)
    CHECK_FORMAT("SD2",string,AUDIO_FORMAT_SD2)
    CHECK_FORMAT("VOX",string,AUDIO_FORMAT_VOX)
    CHECK_FORMAT("WVE",string,AUDIO_FORMAT_WVE)
    CHECK_FORMAT("RF64",string,AUDIO_FORMAT_RF64)
    CHECK_FORMAT("OGA",string,AUDIO_FORMAT_OGA)       
    CHECK_FORMAT("AMR",string,AUDIO_FORMAT_AMR)
    return format;
}

int main(int argc,char* argv[])
{
    if(argc < 4)
    {
        usage();
        return -1;
    }

    printf("recorder created by Eric.D\n");

    const char* input = NULL;
    int32_t format = -1;
    const char* output = NULL;

    int ch;
    opterr = 0;
    while((ch = getopt(argc,argv,"i:o:f:"))!=-1)
    {
        switch(ch)
        {
        case 'i':
            input = optarg;
            break;
        case 'o':
            output = optarg;
            break;
        case 'f':
            format = to_format_string(optarg);
            break;
        case 'h':
            usage();
            break;
        default:
            usage();
            return -1;
        }
    }

#if(defined(WIN32) || defined(WIN64))
    gaudio_init("addons");
#else
    gaudio_init("/usr/local/lib/gaudio/");
#endif

    gsource* source = gaudio_source_create_from_file(input,FALSE);
    if(source == NULL)
    {
        printf("bad source.\nerror code:%d.\n",gaudio_error_get());
        gaudio_deinit();
        return -1;
    }

	//! disable loop mode
    gaudio_source_set_int32(source,AUDIO_ENUM_LOOP,FALSE);
    //gaudio_source_set_position_callback(source,source_position_callback);
    gaudio_source_set_finish_callback(source,source_finish_callback);

    grecorder* recorder = gaudio_recorder_create(output,format);
    if(!recorder)
    {
        printf("unsurpport output format.\n");
        int32_t err = gaudio_error_get();      
        gaudio_source_destroy(source);
        gaudio_deinit();
        return -1;
    }

	//! setup recording mode
    gaudio_set_int32(AUDIO_RECORD_MODE,TRUE);
    gaudio_recorder_write(recorder);
    gaudio_source_play(source,FALSE);

    while(flag);

    gaudio_recorder_stop(recorder);
    gaudio_recorder_destroy(recorder);
    gaudio_source_stop(source);
    gaudio_source_destroy(source);
    gaudio_set_int32(AUDIO_RECORD_MODE,FALSE);
    
    printf("recording work is finished!");
    gaudio_deinit();
    system("PAUSE");
    return EXIT_SUCCESS;
}
