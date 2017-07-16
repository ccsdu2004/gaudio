#include <stdio.h>
#include <stdlib.h>
#include <utility>
#include <libmad/mad.h>
#include "id2.h"
#include "../gaudio/gaddons.h"
#include "../gaudio/gdecreator.h"
#include "../gaudio/streamBuffer.h"
#include "../gaudio/gexternal.h"

typedef streamBuffer* (*stream_open_mem_ptr)(void*,uint32_t,int32_t);
typedef int32_t  (*stream_read_ptr)(streamBuffer* stream,void* ptr,int32_t size,int32_t n);
typedef int32_t  (*stream_tell_ptr)(streamBuffer* stream);
typedef int32_t  (*stream_seek_ptr)(streamBuffer* stream,int32_t offset,int32_t flag);
typedef int32_t  (*stream_close_ptr)(streamBuffer* stream);

static stream_open_mem_ptr stream_open_mem_ = NULL;
static stream_tell_ptr stream_tell_ = NULL;
static stream_read_ptr stream_read_ = NULL;
static stream_seek_ptr stream_seek_ = NULL;
static stream_close_ptr stream_close_ = NULL;

/*
struct ID3Header
{     
    char header[3];  
    char ver;  
    char revision;  
    char flag;  
    char size[4]; 
};

int id3_header_size_get(streamBuffer*);
                    
#define ID3_TITLE  "TIT2"
#define ID3_TPE1   "TPE1"
#define ID3_TPE2   "TPE2"
#define ID3_TPE3   "TPE3"
#define ID3_TPE4   "TPE4"
#define ID3_ALBUM  "TALB"
#define ID3_TRACK  "TRCK"
#define ID3_YEAR   "TYER"
#define ID3_TCON   "TCON"
#define ID3_COMM   "COMM"
#define ID3_PICT   "APIC"
#define ID3_TCOP   "TCOP"
#define ID3_TDAT   "TDAT"
#define ID3_PRIV   "PRIV"
#define ID3_AENC   "AENC"
#define ID3_COMR   "COMR"
#define ID3_ENCR   "ENCR"
#define ID3_EQUA   "EQUA"
#define ID3_ETCO   "ETCO"
#define ID3_GEOB   "GEOB"
#define ID3_GRID   "GRID"
#define ID3_TCOM   "TCOM"
#define ID3_TEXT   "TEXT"
#define ID3_TIT2   "TIT2"
#define ID3_TIT3   "TIT3"
#define ID3_TLAN   "TLAN"
#define ID3_WCOP   "WCOP"

#define id3_frame_size_get(p)\
    p[0]*0x1000000+p[1]*0x10000+p[2]*0x100+p[3];
    
#define ID3_CHECK(label)\
   !strncmp(label,ID3_TITLE,4) ||\
   !strncmp(label,ID3_TPE1,4) ||\
   !strncmp(label,ID3_TPE2,4) ||\
   !strncmp(label,ID3_TPE3,4) ||\
   !strncmp(label,ID3_TPE4,4) ||\
   !strncmp(label,ID3_ALBUM,4) ||\
   !strncmp(label,ID3_YEAR,4) ||\
   !strncmp(label,ID3_TRACK,4) ||\
   !strncmp(label,ID3_COMM,4) ||\
   !strncmp(label,ID3_TDAT,4) ||\
   !strncmp(label,ID3_TCOP,4) ||\
   !strncmp(label,ID3_PRIV,4) ||\
   !strncmp(label,ID3_COMM,4) ||\
   !strncmp(label,ID3_PICT,4) ||\
   !strncmp(label,ID3_TCOP,4) ||\
   !strncmp(label,ID3_PRIV,4) ||\
   !strncmp(label,ID3_AENC,4) ||\
   !strncmp(label,ID3_ENCR,4) ||\
   !strncmp(label,ID3_EQUA,4) ||\
   !strncmp(label,ID3_ETCO,4) ||\
   !strncmp(label,ID3_GEOB,4) ||\
   !strncmp(label,ID3_GRID,4) ||\
   !strncmp(label,ID3_TCOM,4) ||\
   !strncmp(label,ID3_TEXT,4) ||\
   !strncmp(label,ID3_TIT2,4) ||\
   !strncmp(label,ID3_TIT3,4) ||\
   !strncmp(label,ID3_TLAN,4) ||\
   !strncmp(label,ID3_WCOP,4) ||\
   !strncmp(label,ID3_TCON,4)

int id3_header_size_get(streamBuffer* stream)
{   
    ID3Header id3;
    unsigned long tell = stream_tell_(stream);
    stream_read_(stream,&id3,sizeof(ID3Header),1);
    if(strncmp(id3.header,"ID3",3))
    {   
       stream_seek_(stream,tell,SEEK_SET); 
       return 0; 
    }
    unsigned long length = id3.size[0]*0x200000+id3.size[1]*0x4000+id3.size[2]*0x80+id3.size[3];
    return length;
}*/
 
static unsigned long gcd(unsigned long num1,unsigned long num2)
{
    unsigned long tmp;

    while(num2) 
    {
        tmp  = num2;
        num2 = num1 % num2;
        num1 = tmp;
    }
    return num1;
}

signed short to_short(mad_fixed_t fixed)
{
	if(fixed>=MAD_F_ONE)
		return(SHRT_MAX);
	if(fixed<=-MAD_F_ONE)
		return(-SHRT_MAX);
	fixed=fixed>>(MAD_F_FRACBITS-15);
	return((signed short)fixed);
}

gdecoder* gdecoder_create(); 

gxcreator(mp3)

int32_t gmp3decreator::is_support(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_MP3)
       return TRUE;
    return FALSE;           
}

gdecoder* gmp3decreator::create_by_extension(const char* extension)
{   
    if(!strncmp(extension,"mp3",3))
        return gdecoder_create();
    return NULL;    
}

gdecoder* gmp3decreator::create_by_decid(int32_t dec,int32_t mask)
{
    if(dec == AUDIO_FORMAT_MP3)
       return gdecoder_create();
    return NULL;              
}

gdecoder* gmp3decreator::create_by_buffer(int8_t* buffer,uint32_t length)
{    
    int n = buffer[1] >>8;      
    if(buffer[0] == 'I' && buffer[1] == 'D' && buffer[2] == '3')
        return gdecoder_create(); 
    else if(buffer[0] == 127 && n == 0xF)
        return gdecoder_create();       
    return NULL;
}

void gmp3decreator::destroy()
{
    delete this; 
}

char* decoder_maker(gdecoder*)
{
    return "duwenhua2013";        
}

struct gdecoderimpl
{   
    struct mad_stream	stream;
    struct mad_frame	frame;
    struct mad_synth	synth;
    struct mad_header   header;
    unsigned char       buffer[4096 + MAD_BUFFER_GUARD];
    mad_timer_t			timer;
    float               output[4096*4];
    long readsize;     
    long samplerate;
    long channel;
    long bitrate;
    long duration;
    long current;
    long totalsize;
    long use;
     
    gdecoderimpl()                        
    {                                
    }      
    
    ~gdecoderimpl()
    {      
    }
};

void decoder_destroy(gdecoder* decoder)
{   
    delete decoder->impl;
    decoder->impl = 0;
    delete decoder;     
}
 
uint32_t decoder_get_long(gdecoder* decoder,uint32_t flag)
{
    if(flag == AUDIO_ENUM_CHANNEL)
        return decoder->impl->channel;
    else if(flag == AUDIO_ENUM_SAMPLERATE)
        return decoder->impl->samplerate;
    else if(flag == AUDIO_ENUM_FORMAT)
        return AUDIO_FORMAT_TYPE_INT16;                  
    else if(flag == AUDIO_ENUM_BITERATE)
        return decoder->impl->bitrate;         
    else if(flag == AUDIO_ENUM_DURATION)
        return decoder->impl->duration;
    else if(flag == AUDIO_ENUM_POSITION)
    {
        long denom = MAD_TIMER_RESOLUTION / MAD_UNITS_MILLISECONDS;
        long frac = decoder->impl->timer.fraction / denom;
        return decoder->impl->timer.seconds*1000 + frac;
    }
    return FALSE;
}

int mp3setup(gdecoder* decoder,gdecoderimpl* impl,int flag)
{
    size_t	remaining;
    unsigned char* read;                
    if(impl->stream.next_frame != NULL)
    {
        remaining = impl->stream.bufend-impl->stream.next_frame;
        if(flag)
            memmove(impl->buffer,impl->stream.next_frame,remaining);
        read = impl->buffer+remaining;
        impl->readsize = 4096 - remaining;
    }
    else 
        impl->readsize = 4096,
    
    read = impl->buffer,
    remaining = 0;

    impl->readsize = stream_read_((streamBuffer*)decoder->stream,read,1,impl->readsize);
    if(impl->readsize <= 0)
       return -1;
    mad_stream_buffer(&impl->stream,impl->buffer,impl->readsize+remaining);
    impl->stream.error = (mad_error)0;
    return 0;        
}

long mp3skip(gdecoder* decoder,gdecoderimpl* impl)
{
    if(impl->stream.buffer == NULL)
    {
        if(mp3setup(decoder,impl,1) == -1) 
            return -1;
    }
        
    while(mad_frame_decode(&impl->frame,&impl->stream) != 0)
    {
        if(MAD_RECOVERABLE(impl->stream.error))
            continue;
        else if(impl->stream.error == MAD_ERROR_BUFLEN)
        { 
            if(mp3setup(decoder,impl,1) == -1) 
                return -1;
            continue;
        }
        else
            return -1;
    }

    impl->samplerate = impl->frame.header.samplerate;	
    impl->bitrate = impl->frame.header.bitrate;
    impl->channel = (impl->frame.header.mode == MAD_MODE_SINGLE_CHANNEL) ? 1 : 2;
    mad_timer_add(&impl->timer,impl->frame.header.duration);
    return impl->synth.pcm.length;
}

unsigned long mp3_info_get(gdecoder* decoder,gdecoderimpl* impl)
{   
    while(mp3skip(decoder,impl) != -1);
    stream_seek_((streamBuffer*)decoder->stream,0,SEEK_SET);   
    long denom = MAD_TIMER_RESOLUTION / MAD_UNITS_MILLISECONDS;
    long frac = impl->timer.fraction / denom;
    unsigned long duration = impl->timer.seconds*1000+frac;
    mad_stream_init(&impl->stream);
    mad_frame_init(&impl->frame);	
    mad_synth_init(&impl->synth);
    mad_timer_reset(&impl->timer);
    return duration;
}

int32_t decoder_init(gdecoder* decoder)
{   
    mad_stream_init(&decoder->impl->stream);
    mad_frame_init(&decoder->impl->frame);	
    mad_synth_init(&decoder->impl->synth);
    mad_timer_reset(&decoder->impl->timer);
    decoder->impl->totalsize = 0;
    //decoder->impl->id2size = 0;
    //decoder->impl->id3size = 0;
    decoder->impl->duration = 0;
    decoder->impl->use = 0;
    decoder->impl->readsize = 0;
    
    //stream_seek_((streamBuffer*)decoder->stream,0,SEEK_END);
    decoder->impl->totalsize = stream_tell_((streamBuffer*)decoder->stream); 
 
    /*stream_seek_((streamBuffer*)decoder->stream,-128,SEEK_END);
    stream_read_((streamBuffer*)decoder->stream,decoder->impl->id2buffer,128,1);
    if(!strncmp("TAG",decoder->impl->id2buffer,3))
        decoder->impl->id2size = 128;
    else
        decoder->impl->id2size = 0;    
    stream_seek_((streamBuffer*)decoder->stream,0,SEEK_SET);
    int id3size = id3_header_size_get((streamBuffer*)decoder->stream);
    if(id3size != 0)
    {   
        stream_seek_((streamBuffer*)decoder->stream,id3size,SEEK_CUR);
    }*/
    decoder->impl->duration = mp3_info_get(decoder,decoder->impl);  
    return TRUE;
}

int32_t decoder_seek(gdecoder* decoder,uint32_t position)
{   
    if(position < 0 || position > decoder->impl->duration)
        return FALSE;
    unsigned long offset = decoder->impl->totalsize*position/decoder->impl->duration;    
    stream_seek_((streamBuffer*)decoder->stream,offset+decoder->metainfo.id3v2length,SEEK_SET); 
    mad_timer_set(&decoder->impl->timer,0,position,1000);        
    return TRUE; 
}
 
int32_t decoder_meta(gdecoder* decoder,int32_t flag,char* buffer)
{   
    if(decoder->metainfo.id3v1length != 128)
        return FALSE;
    int v = 0;
    if(flag == AUDIO_ENUM_TRACK || flag == AUDIO_ENUM_TITLE)
        v = ID2_TITLE;
    else if(flag == AUDIO_ENUM_ALBUM)
        v = ID2_ALBUM;
    else if(flag == AUDIO_ENUM_ARTIST)
        v = ID2_ARTIST;
    else if(flag == AUDIO_ENUM_GENRE)
        v = ID2_GENRE;
    else if(flag == AUDIO_ENUM_COMMENT)
        v = ID2_COMMENT;        
    return id2_meta_check(decoder->metainfo.id3v1,v,buffer);         
}

long mp3_read(gdecoder* decoder)
{   
    if(decoder->impl->stream.buffer == NULL)
    {
        if(mp3setup(decoder,decoder->impl,0) == -1) 
            return 0;
    }
        
    while(mad_frame_decode(&decoder->impl->frame,&decoder->impl->stream) != 0)
    {
        if(MAD_RECOVERABLE(decoder->impl->stream.error))
        {
            if(decoder->impl->stream.error!=MAD_ERROR_LOSTSYNC)
            {
            }
            continue;
        }
        else if(decoder->impl->stream.error==MAD_ERROR_BUFLEN)
        { 
            if(mp3setup(decoder,decoder->impl,0) == -1) 
                return 0;
            continue;
        }
        else
            return 0;
    }
        
    mad_synth_frame(&decoder->impl->synth,&decoder->impl->frame);
    decoder->impl->samplerate = decoder->impl->synth.pcm.samplerate; 	
    decoder->impl->bitrate = decoder->impl->frame.header.bitrate;
    decoder->impl->channel = (decoder->impl->frame.header.mode == MAD_MODE_SINGLE_CHANNEL) ? 1 : 2;
	
    mad_timer_add(&decoder->impl->timer,decoder->impl->frame.header.duration);
  
    int j = decoder->impl->use;
    for(int i = 0;i<decoder->impl->synth.pcm.length;i++)
    {
        signed short sample;
        sample = to_short(decoder->impl->synth.pcm.samples[0][i]);
        decoder->impl->output[j++] = gtofloat(sample);
        if(MAD_NCHANNELS(&decoder->impl->frame.header)==2)
            sample = to_short(decoder->impl->synth.pcm.samples[1][i]);
        decoder->impl->output[j++] = gtofloat(sample);
    }
    return decoder->impl->synth.pcm.length*MAD_NCHANNELS(&decoder->impl->frame.header); 
}
 
uint32_t decoder_read(gdecoder* decoder,float* pcm,int32_t length)
{   
    printf("read %d\n",length);
    while(true)
    {    
        uint32_t read = mp3_read(decoder);
        //printf("read size:%d\n",read);
        if(read == -1 || read == 0)
        {
            for(uint32_t i=0;i<decoder->impl->use;i++)
                pcm[i] = decoder->impl->output[i];
            int32_t ret = decoder->impl->use;
            decoder->impl->use = 0;  
            return ret;    
        }
        else 
        {   
            decoder->impl->use += read;
            //printf("read increase - use:%d\n",decoder->impl->use);
            if(decoder->impl->use >= length)
            {
                for(uint32_t i=0;i<length;i++)
                    pcm[i] = decoder->impl->output[i];
                std::copy(decoder->impl->output+length,
				          decoder->impl->output+decoder->impl->use,
						  decoder->impl->output);    
                decoder->impl->use -= length;
                //printf("read decrease - use:%d\n",decoder->impl->use);
                //printf("@%d,@%d@\n",decoder->impl->use,length);
                return length;       
            }
            //printf("read - use:%d\n",decoder->impl->use);
        }
    }
}
/*
struct id3v2Tag
{
    char title[255];
    char artist[255];
    char album[255];
    char genre;
    char comment[255];
    char year[5];       
};

void id3v2_get(gdecoder* decoder)
{   
	streamBuffer* stream = stream_open_mem_(decoder->impl->id3pair.second,decoder->impl->id3pair.first,0);

	char buf[128] = {0};
	stream_read_((streamBuffer*)decoder->stream,buf,3,1);
	
	if(strcmp(buf,"ID3")!=0)
	{
		stream_close_(stream);
		return;
	}
    	
	int32_t  ver;
	int32_t  revision;
	char flag;
	char size[4];

    stream_read_((streamBuffer*)decoder->stream,&ver,sizeof(int32_t),1); 
	stream_read_((streamBuffer*)decoder->stream,&revision,sizeof(int32_t),1); 
	stream_read_((streamBuffer*)decoder->stream,&flag,sizeof(char),1); 
	stream_read_((streamBuffer*)decoder->stream,size,sizeof(char),4); 

	int total_size;
	total_size =(size[0]&0x7F)*0x200000+(size[1]&0x7F)*0x4000+(size[2]&0x7F)*0x80+(size[3]&0x7F)-10;
	
	while(total_size>0)
	{
		char fid[5]="\0\0\0\0";
		CString FrameID;
		char Size[4];
		char Flags[2];
		char *obuf = NULL,*buf = NULL;
		
		cFile.Read(fid,4);
        FrameID = fid;
		cFile.Read(Size,4);
		cFile.Read(Flags,2);

		int FSize;
		FSize=Size[0]*0x1000000+Size[1]*0x10000+Size[2]*0x100+Size[3];
		obuf = new char[FSize+1];
		memset(obuf,0,FSize+1);
		cFile.ReadHuge(obuf,FSize);
		total_size-= FSize+10;
		if(*obuf) buf = obuf;
		else buf = obuf + 1;
		
		if (FrameID=="TIT2")
			Title = buf;
		else
		if (FrameID=="TPE1")
			Artist = buf;
		else
		if (FrameID=="TALB")
			Album = buf;
		else
		if (FrameID=="TRCK")
		{
			int i,p=0;
			tracknum[0] = tracknum[1] = 0;
			for(i=0;isdigit(buf[i])||(buf[i]=='/'&&p<2);i++)
				if(buf[i]=='/') 
                    p++;
				else 
                    tracknum[p]=tracknum[p]*10+(buf[i]-48);
		}
		else
		if (FrameID=="TYER")
		{
			int i;
			for(i=0;isdigit(buf[i]);i++)
				year=year*10+(buf[i]-48);
		}
		else
		if (FrameID=="TCON")
			Genre = buf;
		else
		if (FrameID=="COMM")
		{
			char *p=buf+1;
			while(*p) p++;
			p++;
			Comment = p;
		}

		delete[] obuf;
	}   
    
    stream_close_(stream); 
}*/

gdecoder* gdecoder_create()
{
    gdecoder* decoder = new gdecoder();
    decoder->meta = decoder_meta;
    decoder->destroy = decoder_destroy;
    decoder->init = decoder_init;
    decoder->seek = decoder_seek;
    decoder->read = decoder_read;
    decoder->resample[0] = decoder->resample[1] = 0;
    decoder->decid = AUDIO_FORMAT_MP3;
    decoder->version = 1010;
    decoder->maker = decoder_maker;

    decoder->stream = 0;
    decoder->ratio = 1.0f;
    decoder->impl = new gdecoderimpl();
    
    decoder->getint = decoder_get_long;
    return decoder;          
}

static gaddons addon;

gaddons* GAPIENTRY ginstance(void* ctx)
{
    stream_open_mem_ = (stream_open_mem_ptr)gaudio_address_get("stream_opem_mem");     
    stream_tell_ = (stream_tell_ptr)gaudio_address_get("stream_tell");
    stream_seek_ = (stream_seek_ptr)gaudio_address_get("stream_seek"); 
    stream_read_ = (stream_read_ptr)gaudio_address_get("stream_read"); 
    stream_close_ = (stream_close_ptr)gaudio_address_get("stream_close");
    addon.handle = ctx;
    addon.flag = AUDIO_PLUGIN_AUDIO;
    addon.addon = new gmp3decreator();
    return &addon;
}






