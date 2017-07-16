using System;
using System.Runtime.InteropServices;  

namespace gaudio
{   
    public class GAudio
    {
        public const int AUDIO_ENUM_POSITION     = 1;
        public const int AUDIO_ENUM_VELOCITY     = 2;
        public const int AUDIO_ENUM_FORWARD      = 3;
        public const int AUDIO_ENUM_UP           = 4;
        public const int AUDIO_ENUM_SAMPLERATE   = 5;
        public const int AUDIO_ENUM_CHANNEL      = 6;
        public const int AUDIO_ENUM_FORMAT       = 7;
        public const int AUDIO_ENUM_DURATION     = 8;
        public const int AUDIO_ENUM_LOOP         = 9;
        public const int AUDIO_ENUM_GAIN         = 10;
        public const int AUDIO_ENUM_PAN          = 11;
        public const int AUDIO_ENUM_SPEED_OF_PLAY= 12;
        public const int AUDIO_ENUM_SOUND_SPEED  = 13;
        public const int AUDIO_ENUM_MUTE         = 14;
        public const int AUDIO_ENUM_TITLE        = 15;
        public const int AUDIO_ENUM_TRACK        = 16;
        public const int AUDIO_ENUM_ALBUM        = 17;
        public const int AUDIO_ENUM_ARTIST       = 18;
        public const int AUDIO_ENUM_PUBLISHDATE  = 19;
        public const int AUDIO_ENUM_GENRE        = 20;
        public const int AUDIO_ENUM_COMMENT      = 21;
        public const int AUDIO_ENUM_AUDIO        = 22;
        public const int AUDIO_ENUM_BITERATE     = 23;
        public const int AUDIO_ENUM_VERSION      = 24;
        public const int AUDIO_ENUM_DRIVER       = 25;
        public const int AUDIO_ENUM_SOUNDFONT    = 26;
        public const int AUDIO_ENUM_PLAYING      = 27;
        public const int AUDIO_ENUM_ROLLOFF      = 28;
        public const int AUDIO_ENUM_ATTENUATION  = 29;
        public const int AUDIO_ENUM_INNER_ANGLE  = 30;
        public const int AUDIO_ENUM_OUTER_GAIN   = 31;

        public const int AUDIO_EFFECT_NONE               = -1;
        public const int AUDIO_EFFECT_PHASER             = 0;
        public const int AUDIO_EFFECT_INVERT             = 1;
        public const int AUDIO_EFFECT_SWAP_CHANNEL       = 2;
        public const int AUDIO_EFFECT_MERGE_CHANNEL      = 3;
        public const int AUDIO_EFFECT_SPEED_CHANGER      = 4;
        public const int AUDIO_EFFECT_ECHO               = 5;
        public const int AUDIO_EFFECT_REVERB             = 6;
        public const int AUDIO_EFFECT_FFT                = 7;
        public const int AUDIO_EFFECT_EQUALIZER          = 8;
        public const int AUDIO_EFFECT_BASSBOOST          = 9;
        public const int AUDIO_EFFECT_CHORUS             = 10;
        public const int AUDIO_EFFECT_STEREO_ENHANCE     = 11;
        public const int AUDIO_EFFECT_DISTORTION         = 12;
        public const int AUDIO_EFFECT_FOLDBACK_DISTORTION= 13;
        public const int AUDIO_EFFECT_FLANGER            = 14;
        public const int AUDIO_EFFECT_AUTOWAH            = 15;
        public const int AUDIO_EFFECT_COMPRESSER         = 16;
        public const int AUDIO_EFFECT_MODULATOR          = 17;
        public const int AUDIO_EFFECT_FADE_IN            = 18;
        public const int AUDIO_EFFECT_FADE_OUT           = 19;
        public const int AUDIO_EFFECT_LEVELLER           = 20;
        public const int AUDIO_EFFECT_ZIPPER             = 21;

        [DllImport("gaudio.dll",EntryPoint = "gaudio_init")]
        public static extern void gaudio_init(string path);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_deinit")]
        public static extern void gaudio_deinit();

        [DllImport("gaudio.dll",EntryPoint = "gaudio_error_get")]
        public static extern int gaudio_error_get();

        [DllImport("gaudio.dll",EntryPoint = "gaudio_is_support")]
        public static extern int gaudio_is_support(int flag,int value);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_set_float")]
        public static extern void gaudio_set_float(int flag,float value);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_get_float")]
        public static extern void gaudio_get_float(int flag,ref float value);
        
        [DllImport("gaudio.dll",EntryPoint = "gaudio_set_int32")]
        public static extern void gaudio_set_int32(int flag,int value);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_get_int32")]
        public static extern void gaudio_get_int32(int flag,ref int value);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_set_string")]
        public static extern void gaudio_set_string(int flag,string value);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_get_string")]
        public static extern int gaudio_get_string(int flag,ref string value);
        
        [DllImport("gaudio.dll",EntryPoint = "gaudio_listener_set_float3")]
        public static extern void gaudio_listener_set_float3(int flag,float x,float y,float z);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_listener_get_float3")]
        public static extern void gaudio_listener_get_float3(int flag,ref float x,ref float y,ref float z);
      
        [DllImport("gaudio.dll",EntryPoint = "gaudio_source_create_from_file")]
        public static extern IntPtr gaudio_source_create_from_file(string file,int flag);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_source_create_from_buffer")]
        public static extern IntPtr gaudio_source_create_from_buffer(byte[] buffer,int length);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_source_create_from_buffer2")]
        public static extern IntPtr gaudio_source_create_from_buffer2(uint samplerate,uint channels,uint format,byte[] pcm,int length); 

        [DllImport("gaudio.dll",EntryPoint = "gaudio_source_destroy")]
        public static extern void gaudio_source_destroy(IntPtr source);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_source_set_user")]
        public static extern void gaudio_source_set_user(IntPtr source,IntPtr user);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_source_get_user")]
        public static extern IntPtr gaudio_source_get_user(IntPtr source);

        public delegate void gerror(IntPtr source,int param,int value);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_source_set_error_callback")]
        public static extern IntPtr gaudio_source_set_error_callback(IntPtr source,gerror error);

        public delegate void gposition(IntPtr source,int pos);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_source_set_position_callback")]
        public static extern void gaudio_source_set_position_callback(IntPtr source,gposition position);

        public delegate void gfinish(IntPtr source);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_source_set_finish_callback")]
        public static extern void gaudio_source_set_finish_callback(IntPtr source,gfinish finish);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_source_play")]
        public static extern void gaudio_source_play(IntPtr source,int flag);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_source_play3")]
        public static extern void gaudio_source_play3(IntPtr source,int flag);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_source_pause")]
        public static extern void gaudio_source_pause(IntPtr source);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_source_stop")]
        public static extern void gaudio_source_stop(IntPtr source);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_source_seek")]
        public static extern void gaudio_source_seek(IntPtr source,int seek);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_source_set_float")]
        public static extern void gaudio_source_set_float(IntPtr source,int param,int x,int y,int z);
        
        [DllImport("gaudio.dll",EntryPoint = "gaudio_source_seek")]
        public static extern void gaudio_source_get_float(IntPtr source,int param,ref int x,ref int y,ref int z);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_source_set_float")]
        public static extern void gaudio_source_set_float(IntPtr source,int flag,float value);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_source_get_float")]
        public static extern void gaudio_source_get_float(IntPtr source,int flag,ref float value);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_source_set_int32")]
        public static extern void gaudio_source_set_int32(IntPtr source,int flag,int value);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_source_get_int32")]
        public static extern void gaudio_source_get_int32(IntPtr source,int flag,ref int value);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_effect_create")]
        public static extern IntPtr gaudio_effect_create(int effect,int flag);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_effect_destroy")]
        public static extern void gaudio_effect_destroy(IntPtr effect);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_effect_bind")]
        public static extern void gaudio_effect_bind(IntPtr effect,int flag);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_effect_bind_to_source")]
        public static extern void gaudio_effect_bind_to_source(IntPtr effect,IntPtr source);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_effect_unbind")]
        public static extern void gaudio_effect_unbind(IntPtr effect);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_effect_unbind_from_source")]
        public static extern void gaudio_effect_unbind_from_source(IntPtr effect,IntPtr source);

        [DllImport("gaudio.dll",EntryPoint = "gaudio_effect_set_float")]
        public static extern void gaudio_effect_set_float(IntPtr effect,int flag,float value);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_effect_get_float")]
        public static extern void gaudio_effect_get_float(IntPtr effect,int flag,ref float value);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_effect_set_int32")]
        public static extern void gaudio_effect_set_int32(IntPtr effect,int flag,int value);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_effect_get_int32")]
        public static extern void gaudio_effect_get_int32(IntPtr effect,int flag,ref int value);

        public delegate void gefxcb(IntPtr effect,float[] pcm,int length,int a,int b);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_effect_set_fft_callback")]
        public static extern void gaudio_effect_set_fft_callback(IntPtr effect,gefxcb cb);

        public delegate void gefxpcmcb(IntPtr effect, float[][] pcm,int channel,int samplerate,int length);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_recorder_create")]
        public static extern IntPtr gaudio_recorder_create(string filename,uint type);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_recorder_destroy")]
        public static extern void gaudio_recorder_destroy(IntPtr recorder);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_recorder_write")]
        public static extern void gaudio_recorder_write(IntPtr recorder);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_recorder_pause")]
        public static extern void gaudio_recorder_pause(IntPtr recorder);

        [DllImport("gaudio.dll", EntryPoint = "gaudio_recorder_stop")]
        public static extern void gaudio_recorder_stop(IntPtr recorder);
    }
}
//! by D.Eric

