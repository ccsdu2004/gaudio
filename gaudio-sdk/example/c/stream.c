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
#else
#endif
#ifdef WIN32
    #include "getopt.h"
#else
    #include<unistd.h>
#endif
#include "gaudio.h"

void usage()
{
	printf("\rUsage:\tstream -i file -m mode -f fade\n\n");
    printf("\t-h\tprint this help\n");
    printf("\t-i\tfor playing input file\n");
    printf("\t-m\tfor stream mode,1 for std mode,2 for buffer mode and 3 for virtual io mode\n");
    printf("\t-f\tfor use fade in effect or not,1 for use and 0 for without it\n");
}

char* formatmap[] = {
"AUDIO_FORMAT_TYPE_INT4",
"AUDIO_FORMAT_TYPE_INT8",
"AUDIO_FORMAT_TYPE_UINT8",
"AUDIO_FORMAT_TYPE_INT16",
"AUDIO_FORMAT_TYPE_UINT16",
"AUDIO_FORMAT_TYPE_INT24",
"AUDIO_FORMAT_TYPE_INT32",
"AUDIO_FORMAT_TYPE_UINT32",
"AUDIO_FORMAT_TYPE_INT64",
"AUDIO_FORMAT_TYPE_FLOAT",
"AUDIO_FORMAT_TYPE_DOUBLE",
"AUDIO_FORMAT_TYPE_ULAW",
"AUDIO_FORMAT_TYPE_ALAW",
"AUDIO_FORMAT_TYPE_VOX_ADPCM",
"AUDIO_FORMAT_TYPE_MS_ADPCM",
"AUDIO_FORMAT_TYPE_GSM610",
"AUDIO_FORMAT_TYPE_IMA_ADPCM",
"AUDIO_FORMAT_TYPE_DPCM8",
"AUDIO_FORMAT_TYPE_DPCM16",
"AUDIO_FORMAT_TYPE_G723_24",
"AUDIO_FORMAT_TYPE_G723_32",
"AUDIO_FORMAT_TYPE_G723_40",
"AUDIO_FORMAT_TYPE_DWVW_12",
"AUDIO_FORMAT_TYPE_DWVW_16",
"AUDIO_FORMAT_TYPE_DWVW_24",
"AUDIO_FORMAT_TYPE_DWVW_N",
"AUDIO_FORMAT_TYPE_VORBIS"};

void GAPIENTRY tellpos(gsource* source,int32_t position)
{
    printf("\rplay position:%d",position);
}

int32_t GAPIENTRY myread(void* buffer,int32_t size,int32_t count,void* ctx)
{
    FILE* file = (FILE*)ctx;
    return fread(buffer,size,count,file);
}

int32_t GAPIENTRY myseek(void* ctx,int32_t offset,int32_t flag)
{
    FILE* file = (FILE*)ctx;
    return fseek(file,offset,flag);
}

int32_t GAPIENTRY mytell(void* ctx)
{
    FILE* file = (FILE*)ctx;
    return ftell(file);
}

int32_t GAPIENTRY myclose(void* ctx)
{
    FILE* file = (FILE*)ctx;
    fclose(file);
    return 1;
}

int main(int argc,char* argv[])
{
    if(argc < 7)
    {
        usage();
        return -1;
    }
    int mode = 0;
    char* filename = 0;
    int fade = 0;

    int ch;
    opterr = 0;
    while((ch = getopt(argc,argv,"i:f:hm:"))!=-1)
    {
        switch(ch)
        {
        case 'i':
            filename = optarg;
            break;
        case 'h':
            usage();
            break;
        case 'f':
            fade = atoi(optarg);
            break;
        case 'm':
            mode = atoi(optarg);
            break;
        default:
            usage();
            return -1;
        }
    }

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

    gsource* source = NULL;
    int8_t* buffer = NULL;

    if(mode < 0 || mode > 2)
        mode = 0;
    if(fade < 0 || fade > 1)
        fade = 0;

    if(mode == 0)
        source = gaudio_source_create_from_file(filename,FALSE);
    else if(mode == 1)
    {
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
    }
    else if(mode == 2)
    {
        FILE* file = fopen((char*)filename,"rb");
        if(!file)
        {
            gaudio_deinit();
            printf("load file:%s error!\n",filename);
            return -1;
        }
        source = gaudio_source_create_from_virtual_io(myread,myseek,mytell,myclose,(void*)file);
    }
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
    gaudio_source_get_int32(source,AUDIO_ENUM_FORMAT,&value);
    if(value == -1)
        printf("source sample formate bad!\n");
    else
        printf("source sample format:%s\n",formatmap[value]);

    gaudio_source_get_int32(source,AUDIO_ENUM_BITERATE,&value);
    printf("source bitrate:%d\n",value);

    char comment[256] = {0};
    value = gaudio_source_get_string(source,AUDIO_ENUM_TITLE,comment);
    if(value == TRUE)
    {
        printf("source title:%s\n",comment);
        memset(comment,0,sizeof(char)*256);
    }
    value = gaudio_source_get_string(source,AUDIO_ENUM_ALBUM,comment);
    if(value == TRUE)
    {
        printf("source album:%s\n",comment);
        memset(comment,0,sizeof(char)*256);
    }
    value = gaudio_source_get_string(source,AUDIO_ENUM_ARTIST,comment);
    if(value == TRUE)
    {
        printf("source artist:%s\n",comment);
        memset(comment,0,sizeof(char)*256);
    }
    value = gaudio_source_get_string(source,AUDIO_ENUM_GENRE,comment);
    if(value == TRUE)
    {
        printf("source genre:%s\n",comment);
        memset(comment,0,sizeof(char)*256);
    }
    value = gaudio_source_get_string(source,AUDIO_ENUM_PUBLISHDATE,comment);
    if(value == TRUE)
    {
        printf("source published date:%s\n",comment);
        memset(comment,0,sizeof(char)*256);
    }
    value = gaudio_source_get_string(source,AUDIO_ENUM_COMMENT,comment);
    if(value == TRUE)
    {
        printf("source comment:%s\n",comment);
        memset(comment,0,sizeof(char)*256);
    }

    gaudio_source_get_int32(source,AUDIO_ENUM_CHANNEL,&value);
    printf("source channel:%d\n",value);
    gaudio_source_get_int32(source,AUDIO_ENUM_DURATION,&value);
    printf("source duration:%d\n",value);

    geffect* fadein = NULL;
    if(fade)
        fadein = gaudio_effect_create(AUDIO_EFFECT_FADE_IN,0);

    if(fadein)
        gaudio_effect_bind_to_source(fadein,source);

    gaudio_set_float(AUDIO_ENUM_GAIN,0.9f);
    gaudio_source_play(source,FALSE);

    printf("\nplaying, press any key to quit.\n");
	getch();

    if(fade)
        gaudio_effect_unbind_from_source(fadein,source);

    gaudio_source_stop(source);
    gaudio_source_destroy(source);
    if(mode==1)
        free(buffer);
    if(fade)
        gaudio_effect_destroy(fadein);

    gaudio_deinit();
#ifdef WIN32
    system("PAUSE");
#endif
    return EXIT_SUCCESS;
}
