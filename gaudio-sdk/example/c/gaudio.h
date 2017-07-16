/*************************************************************************
 * GAudio library - Geek Audio Library
 * API version: 2.2.4.1
 * WWW:         http://www.geek-audio.org/
 *------------------------------------------------------------------------
 * Copyright (c) 2013 - 2015 D.Eric <eric@geek-audio.org>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 *************************************************************************/
#ifndef LIB_GEEK_AUDIO_SDK_H
#define LIB_GEEK_AUDIO_SDK_H
#include <stdint.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/*************************************************************************
 * Doxygen documentation
 *************************************************************************/
/* ---------------- GAudio library related system specific defines ----------------- */
#if !(defined(WIN32) || defined(WIN64)) && !defined(__linux__)
   #error "gaudio library don't support this os at present"
#endif

#if(defined(WIN32) || defined(WIN64))
    #define GAPIENTRY __cdecl
    #define GAPICONV  __stdcall
#else
    #define GAPIENTRY
    #define GAPICONV
#endif

#if defined(BUILDING)
#if(defined(WIN32) || defined(WIN64))
# define GAPI __declspec(dllexport)
#else
# define GAPI
# define GAPI __declspec(dllimport)
#endif
#else
#if(defined(WIN32) || defined(WIN64))
# define GAPI __declspec(dllimport)
#else
# define GAPI
#endif
# define GAPI __declspec(dllimport)
#endif

#ifndef NULL
 #ifdef __cplusplus
  #define NULL 0
 #else
  #define NULL ((void *)0)
 #endif
#endif /* NULL */

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

/*************************************************************************
 * GAudio library API tokens
 *************************************************************************/

/** Enumeration of the type of GAudio 's plugin. */
enum gplugin_code
{
    AUDIO_PLUGIN_AUDIO = 1,  /**< the audio decoder of GAudio. */
    AUDIO_PLUGIN_EFFECT,     /**< the audio effect plugin of GAudio. */
    AUDIO_PLUGIN_DRIVER,     /**< the audio driver plugin of GAudio. */
    AUDIO_PLUGIN_RECORDER,   /**< the audio recoder plugin of GAUdio SDK. */
    AUDIO_PLUGIN_WRITER = AUDIO_PLUGIN_RECORDER,
};

/** Enumeration of the type of GAudio 's driver plugin loaded. */
enum gdriver_code
{
    AUDIO_DRIVER_NULL = 0,  /**< dummy audio driver plugin. */
    AUDIO_DRIVER_WINMM,     /**< Macrosoft 's winmm plugin. */
    AUDIO_DRIVER_DSOUND,    /**< Macrosoft 's dsound plugin. */
    AUDIO_DRIVER_ALSA,      /**< Advanced Linux Sound Arthiecture 's driver plugin. */
    AUDIO_DRIVER_OSS,       /**< Open Sound System driver plugin. */
    AUDIO_DRIVER_COREAUDIO, /**< Apple Inc. 's audio driver plugin. */
    AUDIO_DRIVER_PORTAUDIO  /**< Open source - Real-Time Audio Library 's driver plugin. */
};

/** Enumeration of the attenuational model of GAudio. */
enum gdistance_model
{
    AUDIO_ATTENUATION_MIN = 0,
    AUDIO_ATTENUATION_INVERSE_DISTANCE = AUDIO_ATTENUATION_MIN,
    AUDIO_ATTENUATION_INVERSE_DISTANCE_CLAMPED,
    AUDIO_ATTENUATION_LINEAR_DISTANCE,
    AUDIO_ATTENUATION_LINEAR_DISTANCE_CLAMPED,
    AUDIO_ATTENUATION_EXPONENT_DISTANCE,
    AUDIO_ATTENUATION_EXPONENT_DISTANCE_CLAMPED,
    AUDIO_ATTENUATION_MAX = AUDIO_ATTENUATION_EXPONENT_DISTANCE_CLAMPED
};

#define AUDIO_ENUM_POSITION      1
#define AUDIO_ENUM_VELOCITY      2
#define AUDIO_ENUM_FORWARD       3
#define AUDIO_ENUM_UP            4
#define AUDIO_ENUM_SAMPLERATE    5
#define AUDIO_ENUM_CHANNEL       6
#define AUDIO_ENUM_FORMAT        7
#define AUDIO_ENUM_DURATION      8
#define AUDIO_ENUM_LOOP          9
#define AUDIO_ENUM_GAIN          10
#define AUDIO_ENUM_PAN           11
#define AUDIO_ENUM_SPEED_OF_PLAY 12
#define AUDIO_ENUM_SOUND_SPEED   13
#define AUDIO_ENUM_MUTE          14
#define AUDIO_ENUM_TITLE         15
#define AUDIO_ENUM_TRACK         16
#define AUDIO_ENUM_ALBUM         17
#define AUDIO_ENUM_ARTIST        18
#define AUDIO_ENUM_PUBLISHDATE   19
#define AUDIO_ENUM_GENRE         20
#define AUDIO_ENUM_COMMENT       21
#define AUDIO_ENUM_AUDIO         22
#define AUDIO_ENUM_BITERATE      23
#define AUDIO_ENUM_VERSION       24
#define AUDIO_ENUM_DRIVER        25
#define AUDIO_ENUM_SOUNDFONT     26
#define AUDIO_ENUM_PLAYING       27
#define AUDIO_ENUM_ROLLOFF       28
#define AUDIO_ENUM_ATTENUATION   29
#define AUDIO_ENUM_INNER_ANGLE   30
#define AUDIO_ENUM_OUTER_GAIN    31

typedef struct gsource_  gsource;
typedef struct geffect_  geffect;
typedef struct gcatcher_ gcatcher;

/*************************************************************************
 * GAudio API functions
 *************************************************************************/

/*! @brief Initializes the GAudio library.
 *
 *  Library initialization function - call this before using GAudio.
 *  This function initializes the GAudio library. Before any GAudio functions can
 *  be used, GAudio must be initialized, and before a program terminates GAudio
 *  should be terminated in order to free any resources allocated during or
 *  after initialization.
 *
 *  If this function fails, it calls @ref gaudio_deinit before returning.  If it
 *  succeeds, you should call @ref gaudio_deinit before the program exits.
 *
 *  Additional calls to this function after successful initialization but before
 *  termination will succeed but will do nothing.
 *
 *  @param[in] addons Where to search and load plugins which GAudio needed.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @note This function may take several seconds to complete on some systems,
 *  while on other systems it may take only a fraction of a second to complete.
 *
 *  @sa gaudio_deinit
 *
 *  @ingroup init
 */
extern GAPI void GAPIENTRY gaudio_init(const char* addons);

/*! @brief Terminates the GAudio library.
 *
 *  This function frees any allocated resources and sets the library to an
 *  uninitialized state. Once this is called, you must again call @ref gaudio_init
 *  successfully before you will be able to use most GAudio functions.
 *
 *  If GAudio has been successfully initialized, this function should be called
 *  before the program exits. If initialization fails, there is no need to call
 *  this function, as it is called by @ref gaudio_init before it returns failure.
 *
 *  @remarks This function may be called before @ref gaudio_init.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_init
 *
 *  @ingroup init
 */
extern GAPI void GAPIENTRY gaudio_deinit();

/** Audio format type */
#define AUDIO_FORMAT_TYPE_NONE      -1
#define AUDIO_FORMAT_TYPE_INT4      0
#define AUDIO_FORMAT_TYPE_INT8      1
#define AUDIO_FORMAT_TYPE_UINT8     2
#define AUDIO_FORMAT_TYPE_INT16     3
#define AUDIO_FORMAT_TYPE_UINT16    4
#define AUDIO_FORMAT_TYPE_INT24     5
#define AUDIO_FORMAT_TYPE_INT32     6
#define AUDIO_FORMAT_TYPE_UINT32    7
#define AUDIO_FORMAT_TYPE_INT64     8
#define AUDIO_FORMAT_TYPE_FLOAT     9
#define AUDIO_FORMAT_TYPE_DOUBLE    10
#define AUDIO_FORMAT_TYPE_ULAW      11
#define AUDIO_FORMAT_TYPE_ALAW      12
#define AUDIO_FORMAT_TYPE_VOX_ADPCM 13
#define AUDIO_FORMAT_TYPE_MS_ADPCM  14
#define AUDIO_FORMAT_TYPE_GSM610    15
#define AUDIO_FORMAT_TYPE_IMA_ADPCM 16
#define AUDIO_FORMAT_TYPE_DPCM8     17
#define AUDIO_FORMAT_TYPE_DPCM16    18
#define AUDIO_FORMAT_TYPE_G723_24   19
#define AUDIO_FORMAT_TYPE_G723_32   20
#define AUDIO_FORMAT_TYPE_G723_40   21
#define AUDIO_FORMAT_TYPE_DWVW_12   22
#define AUDIO_FORMAT_TYPE_DWVW_16   23
#define AUDIO_FORMAT_TYPE_DWVW_24   24
#define AUDIO_FORMAT_TYPE_DWVW_N    25
#define AUDIO_FORMAT_TYPE_VORBIS    26
#define AUDIO_FORMAT_TYPE_MPC       27
#define AUDIO_FORMAT_TYPE_MAX       100

/** Channel type */
#define DEVICE_CHANNEL_MONO    1
#define DEVICE_CHANNEL_STEREO  (DEVICE_CHANNEL_MONO+1)
#define DEVICE_CHANNEL_QUAD    (DEVICE_CHANNEL_STEREO+1)
#define DEVICE_CHANNEL_5POINT1 (DEVICE_CHANNEL_QUAD+1)
#define DEVICE_CHANNEL_6POINT1 (DEVICE_CHANNEL_5POINT1+1)
#define DEVICE_CHANNEL_7POINT1 (DEVICE_CHANNEL_6POINT1+1)
#define DEVICE_CHANNE_MAX      (DEVICE_CHANNEL_7POINT1+1)

/** Enumeration of the current mode of GAudio SDK library. */
enum gmode_code
{
    AUDIO_PLAY_MODE = 1,    /**< current is playing mode. */
    AUDIO_RECORD_MODE,      /**< current is record/encoder mode. */
    AUDIO_WRITE_MODE = AUDIO_RECORD_MODE,
};

/** Audio file type */
#define AUDIO_FORMAT_NONE   -1
#define AUDIO_FORMAT_RAW     0
#define AUDIO_FORMAT_WAV     1
#define AUDIO_FORMAT_AU      2
#define AUDIO_FORMAT_AIFF    3
#define AUDIO_FORMAT_FLAC    4
#define AUDIO_FORMAT_MP2     5
#define AUDIO_FORMAT_MP3     6
#define AUDIO_FORMAT_APE     7
#define AUDIO_FORMAT_AAC     8
#define AUDIO_FORMAT_CAF     9
#define AUDIO_FORMAT_OGG     10
#define AUDIO_FORMAT_VOC     11
#define AUDIO_FORMAT_WV      12
#define AUDIO_FORMAT_MIDI    13
#define AUDIO_FORMAT_XM      14
#define AUDIO_FORMAT_S3M     15
#define AUDIO_FORMAT_MOD     16
#define AUDIO_FORMAT_IT      17
#define AUDIO_FORMAT_WMA     18
#define AUDIO_FORMAT_WMV     19
#define AUDIO_FORMAT_CD      20
#define AUDIO_FORMAT_VQF     21
#define AUDIO_FORMAT_MPC     22
#define AUDIO_FORMAT_TTA     23
#define AUDIO_FORMAT_AC3     24
#define AUDIO_FORMAT_TAK     25
#define AUDIO_FORMAT_RM      26
#define AUDIO_FORMAT_SND     27
#define AUDIO_FORMAT_SVX     28
#define AUDIO_FORMAT_PAF     29
#define AUDIO_FORMAT_FAP     30
#define AUDIO_FORMAT_HTK     31
#define AUDIO_FORMAT_SF      32
#define AUDIO_FORMAT_W64     33
#define AUDIO_FORMAT_MAT4    34
#define AUDIO_FORMAT_MAT5    35
#define AUDIO_FORMAT_MAT     36
#define AUDIO_FORMAT_PVF     37
#define AUDIO_FORMAT_XI      38
#define AUDIO_FORMAT_SD2     39
#define AUDIO_FORMAT_SDS     40
#define AUDIO_FORMAT_VOX     41
#define AUDIO_FORMAT_WVE     42
#define AUDIO_FORMAT_RF64    43
#define AUDIO_FORMAT_OGA     44
#define AUDIO_FORMAT_AMR     45

/** Audio effect type */
#define AUDIO_EFFECT_NONE                  -1
#define AUDIO_EFFECT_LOWPASS_FILTER         0
#define AUDIO_EFFECT_HIGHPASS_FILTER        1
#define AUDIO_EFFECT_BANDPASS_FILTER        2
#define AUDIO_EFFECT_PHASER                 3
#define AUDIO_EFFECT_INVERT                 4
#define AUDIO_EFFECT_SWAP_CHANNEL           5
#define AUDIO_EFFECT_MERGE_CHANNEL          6
#define AUDIO_EFFECT_SPEED_CHANGER          7
#define AUDIO_EFFECT_ECHO                   8
#define AUDIO_EFFECT_REVERB                 9
#define AUDIO_EFFECT_FFT                    10
#define AUDIO_EFFECT_EQUALIZER              11
#define AUDIO_EFFECT_BASSBOOST              12
#define AUDIO_EFFECT_CHORUS                 13
#define AUDIO_EFFECT_STEREO_ENHANCE         14
#define AUDIO_EFFECT_DISTORTION             15
#define AUDIO_EFFECT_FOLDBACK_DISTORTION    16
#define AUDIO_EFFECT_FLANGER                17
#define AUDIO_EFFECT_AUTOWAH                18
#define AUDIO_EFFECT_COMPRESSER             19
#define AUDIO_EFFECT_MODULATOR              20
#define AUDIO_EFFECT_FADE_IN                21
#define AUDIO_EFFECT_FADE_OUT               22
#define AUDIO_EFFECT_TREMOLO                23

/** Lowpass Filter*/
#define AUDIO_EFFECT_LOWPASS_GAIN  1
#define AUDIO_EFFECT_LOWPASS_GAINH 2

#define AUDIO_EFFECT_LOWPASS_MIN_GAIN       0.0f
#define AUDIO_EFFECT_LOWPASS_MAX_GAIN       1.0f
#define AUDIO_EFFECT_LOWPASS_DEFAULT_GAIN   AUDIO_EFFECT_LOWPASS_MAX_GAIN

#define AUDIO_EFFECT_LOWPASS_MIN_GAINHF     0.0f
#define AUDIO_EFFECT_LOWPASS_MAX_GAINHF     1.0f
#define AUDIO_EFFECT_LOWPASS_DEFAULT_GAINHF AUDIO_EFFECT_LOWPASS_MAX_GAINHF

/** Highpass Filter*/
#define AUDIO_EFFECT_HIGHPASS_GAIN   1
#define AUDIO_EFFECT_HIGHPASS_GAINLF 2

#define AUDIO_EFFECT_HIGHPASS_MIN_GAIN      0.0f
#define AUDIO_EFFECT_HIGHPASS_MAX_GAIN      1.0f
#define AUDIO_EFFECT_HIGHPASS_DEFAULT_GAIN  AUDIO_EFFECT_HIGHPASS_MAX_GAIN

#define AUDIO_EFFECT_HIGHPASS_MIN_GAINLF     0.0f
#define AUDIO_EFFECT_HIGHPASS_MAX_GAINLF     1.0f
#define AUDIO_EFFECT_HIGHPASS_DEFAULT_GAINLF AUDIO_EFFECT_HIGHPASS_MAX_GAINLF

/** Bandpass Filter*/
#define AUDIO_EFFECT_BANDPASS_GAIN   1
#define AUDIO_EFFECT_BANDPASS_GAINHF 2
#define AUDIO_EFFECT_BANDPASS_GAINLF 3

#define AUDIO_EFFECT_BANDPASS_MIN_GAIN     0.0f
#define AUDIO_EFFECT_BANDPASS_MAX_GAIN     1.0f
#define AUDIO_EFFECT_BANDPASS_DEFAULT_GAIN AUDIO_EFFECT_BANDPASS_MAX_GAIN

#define AUDIO_EFFECT_BANDPASS_MIN_GAINHF     0.0f
#define AUDIO_EFFECT_BANDPASS_MAX_GAINHF     1.0f
#define AUDIO_EFFECT_BANDPASS_DEFAULT_GAINHF AUDIO_EFFECT_BANDPASS_MAX_GAINHF

#define AUDIO_EFFECT_BANDPASS_MIN_GAINLF     0.0f
#define AUDIO_EFFECT_BANDPASS_MAX_GAINLF     1.0f
#define AUDIO_EFFECT_BANDPASS_DEFAULT_GAINLF AUDIO_EFFECT_BANDPASS_MAX_GAINLF

/** Phaser Effect*/
#define AUDIO_EFFECT_PHASER_SAMPLERATE  1
#define AUDIO_EFFECT_PHASER_DEPTH       2
#define AUDIO_EFFECT_PHASER_PHASE       3
#define AUDIO_EFFECT_PHASER_STAGE       4
#define AUDIO_EFFECT_PHASER_DRY         5
#define AUDIO_EFFECT_PHASER_FB          6

#define AUDIO_EFFECT_PHASER_FREQUENCY_LOW     .0f
#define AUDIO_EFFECT_PHASER_FREQUENCY_HIGH    40.0f
#define AUDIO_EFFECT_PHASER_FREQUENCY_DEFAULT .5f

#define AUDIO_EFFECT_PHASER_PHASE_LOW         .0f
#define AUDIO_EFFECT_PHASER_PHASE_HIGH        359.0f
#define AUDIO_EFFECT_PHASER_PHASE_DEFAULT     AUDIO_EFFECT_PHASER_PHASE_LOW

#define AUDIO_EFFECT_PHASER_DEPTH_LOW         .0f
#define AUDIO_EFFECT_PHASER_DEPTH_HIGH        255.0f
#define AUDIO_EFFECT_PHASER_DEPTH_DEFAULT     75.0f

#define AUDIO_EFFECT_PHASER_STAGE_LOW         2.0f
#define AUDIO_EFFECT_PHASER_STAGE_HIGH        24.0f
#define AUDIO_EFFECT_PHASER_STAGE_DEFAULT     AUDIO_EFFECT_PHASER_STAGE_LOW

#define AUDIO_EFFECT_PHASER_FB_LOW            -100.0f
#define AUDIO_EFFECT_PHASER_FB_HIGH           100.0f
#define AUDIO_EFFECT_PHASER_FB_DEFAULT        .0f

#define AUDIO_EFFECT_DRY_LOW                  .0f
#define AUDIO_EFFECT_DRY_HIGH                 255.0f
#define AUDIO_EFFECT_DRY_DEFAULT              128.0f

/**Echo Effect*/
#define AUDIO_EFFECT_ECHO_DELAY 1
#define AUDIO_EFFECT_ECHO_DECAY 2

#define AUDIO_EFFECT_ECHO_DELAY_MIN     0.2f
#define AUDIO_EFFECT_ECHO_DELAY_MAX     1.0f
#define AUDIO_EFFECT_ECHO_DELAY_DEFAULT 0.5f

#define AUDIO_EFFECT_ECHO_DECAY_MIN     0.1f
#define AUDIO_EFFECT_ECHO_DECAY_MAX     1.0f
#define AUDIO_EFFECT_ECHO_DECAY_DEFAULT 0.2f

/**Distortion Effect*/
#define AUDIO_EFFECT_DISTORTION_THRESHOLD 1
#define AUDIO_EFFECT_DISTORTION_GAIN      2

#define AUDIO_EFFECT_DISTORTION_THRESHOLD_LOW     .72f
#define AUDIO_EFFECT_DISTORTION_THRESHOLD_HIGH    1.0f
#define AUDIO_EFFECT_DISTORTION_THRESHOLD_DEFAULT AUDIO_EFFECT_DISTORTION_THRESHOLD_LOW

#define AUDIO_EFFECT_DISTORTION_GAIN_LOW     .86f
#define AUDIO_EFFECT_DISTORTION_GAIN_HIGH    1.2f
#define AUDIO_EFFECT_DISTORTION_GAIN_DEFAULT 1.0f

/**FFT Effect*/
#define AUDIO_EFFECT_FFT_CAPTURE_SIZE_VALUE 1
#define AUDIO_EFFECT_FFT_CHHANEL_VALUE      2

#define AUDIO_EFFECT_FFT_CAPTURE_SIZE_VALUE_MIN 32
#define AUDIO_EFFECT_FFT_CAPTURE_SIZE_VALUE_MAX 256
#define AUDIO_EFFECT_FFT_CAPTURE_SIZE_VALUE_DEFAULT 128

#define AUDIO_EFFECT_FFT_CHHANEL_VALUE_LEFT  0
#define AUDIO_EFFECT_FFT_CHHANEL_VALUE_RIGHT 1
#define AUDIO_EFFECT_FFT_CHHANEL_VALUE_AVG   2

/**Bassboost effect*/
#define AUDIO_EFFECT_BASSBOOST_FREQUENCE      1
#define AUDIO_EFFECT_BASSBOOST_GAIN           2

#define AUDIO_EFFECT_BASSBOOST_FREQUENCE_LOW     1.0f
#define AUDIO_EFFECT_BASSBOOST_FREQUENCE_HIGH    1000.0f
#define AUDIO_EFFECT_BASSBOOST_FREQUENCE_DEFAULT 250.0f

#define AUDIO_EFFECT_BASSBOOST_GAIN_LOW       1.0f
#define AUDIO_EFFECT_BASSBOOST_GAIN_HIGH      36.0f
#define AUDIO_EFFECT_BASSBOOST_GAIN_DEFAULT   3.0f

/**AutoWah*/
#define AUDIO_EFFECT_AUTOWAH_FREQ     1
#define AUDIO_EFFECT_AUTOWAH_FREQOFF  2
#define AUDIO_EFFECT_AUTOWAH_PHASE    3
#define AUDIO_EFFECT_AUTOWAH_DEPTH    4
#define AUDIO_EFFECT_AUTOWAH_RESOURCE 5

#define AUDIO_EFFECT_AUTOWAH_FREQ_LOW     .0f
#define AUDIO_EFFECT_AUTOWAH_FREQ_HIGH    40.0f
#define AUDIO_EFFECT_AUTOWAH_FREQ_DEFAULT 2.0f

#define AUDIO_EFFECT_AUTOWAH_FREQOFF_LOW     .0f
#define AUDIO_EFFECT_AUTOWAH_FREQOFF_HIGH    100.0f
#define AUDIO_EFFECT_AUTOWAH_FREQOFF_DEFAULT 20.0f

#define AUDIO_EFFECT_AUTOWAH_PHASE_LOW     .0f
#define AUDIO_EFFECT_AUTOWAH_PHASE_HIGH    359.0f
#define AUDIO_EFFECT_AUTOWAH_PHASE_DEFAULT 0.0f

#define AUDIO_EFFECT_AUTOWAH_DEPTH_LOW     .0f
#define AUDIO_EFFECT_AUTOWAH_DEPTH_HIGH    100.0f
#define AUDIO_EFFECT_AUTOWAH_DEPTH_DEFAULT 70.0f

#define AUDIO_EFFECT_AUTOWAH_RESOURCE_LOW     .0f
#define AUDIO_EFFECT_AUTOWAH_RESOURCE_HIGH    100.0f
#define AUDIO_EFFECT_AUTOWAH_RESOURCE_DEFAULT 24.0f

/**Compersson Effect*/

/**Modulator Effect*/
#define AUDIO_EFFECT_MODULATOR_FREQUENCY               1
#define AUDIO_EFFECT_MODULATOR_HIGHPASS_CUTOFF         2
#define AUDIO_EFFECT_MODULATOR_WAVEFORM                3

#define AUDIO_EFFECT_MODULATOR_FREQUENCY_MIN           .0f
#define AUDIO_EFFECT_MODULATOR_FREQUENCY_MAX           8000.0f
#define AUDIO_EFFECT_MODULATOR_FREQUENCY_DEFAULT       440.0f

#define AUDIO_EFFECT_MODULATOR_HIGHPASS_CUTOFF_MIN     .0f
#define AUDIO_EFFECT_MODULATOR_HIGHPASS_CUTOFF_MAX     24000.0f
#define AUDIO_EFFECT_MODULATOR_HIGHPASS_CUTOFF_DEFAULT 800.0f

#define AUDIO_EFFECT_MODULATOR_SINUSOID       0
#define AUDIO_EFFECT_MODULATOR_SAWTOOTH       1
#define AUDIO_EFFECT_MODULATOR_SQUARE         2

#define AUDIO_EFFECT_MODULATOR_WAVEFORM_MIN     AUDIO_EFFECT_MODULATOR_SINUSOID
#define AUDIO_EFFECT_MODULATOR_WAVEFORM_MAX     AUDIO_EFFECT_MODULATOR_SQUARE
#define AUDIO_EFFECT_MODULATOR_WAVEFORM_DEFAULT AUDIO_EFFECT_MODULATOR_SINUSOID

/**Fade Effect*/
#define AUDIO_EFFECT_FADE_IN_VALUE    1
#define AUDIO_EFFECT_FADE_IN_MAX      3.0f
#define AUDIO_EFFECT_FADE_IN_MIN      .6f
#define AUDIO_EFFECT_FADE_IN_DEFAULT  1.2f

#define AUDIO_EFFECT_FADE_OUT_VALUE   1
#define AUDIO_EFFECT_FADE_OUT_MAX     3.0f
#define AUDIO_EFFECT_FADE_OUT_MIN     .6f
#define AUDIO_EFFECT_FADE_OUT_DEFAULT 1.2f

/**Tremo Effect*/
#define AUDIO_EFFECT_TREMOLO_DEPTH      1
#define AUDIO_EFFECT_TREMOLO_SAMPLERATE 2
#define AUDIO_EFFECT_TREMOLO_PHASER     3

#define AUDIO_EFFECT_TREMOLO_DEPTH_LOW          .0f
#define AUDIO_EFFECT_TREMOLO_DEPTH_HIGH         .2f
#define AUDIO_EFFECT_TREMOLO_DEPTH_DEFAULT      .1f

#define AUDIO_EFFECT_TREMOLO_SAMPLERATE_LOW     2000.0f
#define AUDIO_EFFECT_TREMOLO_SAMPLERATE_HIGH    8000.0f
#define AUDIO_EFFECT_TREMOLO_SAMPLERATE_DEFAULT 2000.0f

#define AUDIO_EFFECT_TREMOLO_PHASE_LOW         .0f
#define AUDIO_EFFECT_TREMOLO_PHASE_HIGH        359.0f
#define AUDIO_EFFECT_TREMOLO_PHASE_DEFAULT     .0f

/**Speed Changer Effect*/
#define AUDIO_EFFECT_SPEED_CHANGER_RATIO    1

#define AUDIO_EFFECT_SPEED_CHANGER_RATIO_LOW     0.75f
#define AUDIO_EFFECT_SPEED_CHANGER_RATIO_HIGH    1.50f
#define AUDIO_EFFECT_SPEED_CHANGER_RATIO_DEFAULT 1.0f

/**Crossfeed Effect*/
#define AUDIO_EFFECT_CROSSFFED_DEEP  1

#define AUDIO_EFFECT_CROSSFFED_LOW     1
#define AUDIO_EFFECT_CROSSFFED_HIGH    6
#define AUDIO_EFFECT_CROSSFFED_DEFAULT 3

/*! @brief check GAudio library 's feature
 *
 *  This function check GAudio library 's feature, which inclued the following type:
 *      AUDIO_PLUGIN_AUDIO
 *      AUDIO_PLUGIN_RECORDER
 *      AUDIO_PLUGIN_EFFECT
 *
 *  @param[in] param - attribute type
 *  @param[in] buffer The buffer that will be loaded.
 *  @param[out] value - TRUE for support, FALSE for else.
 *
 *  @sa gaudio_get_float
 *  @sa gaudio_get_int32
 *  @sa gaudio_set_int32
 */
extern GAPI int32_t GAPIENTRY gaudio_is_support(int32_t param,int32_t value);

/*! @brief set GAudio library 's float attribute
 *
 *  This function set GAudio library 's float attribute, which inclued the following type:
 *      AUDIO_ENUM_GAIN
 *      AUDIO_ENUM_PAN
 *
 *  @param[in] param - attribute type
 *  @param[in] value - attribute value
 *
 *  @note for AUDIO_ENUM_GAIN the range is [0.0f,1.0f]
 *  @note for AUDIO_ENUM_PAN the range is [-1.0f,1.0f]
 *
 *  @sa gaudio_get_float
 *  @sa gaudio_get_int32
 *  @sa gaudio_set_int32
 */
extern GAPI void GAPIENTRY gaudio_set_float(int32_t param,float value);

/*! @brief get GAudio library 's float attribute
 *
 *  This function get GAudio library 's float attribute, which inclued the following type:
 *      AUDIO_ENUM_GAIN
 *      AUDIO_ENUM_PAN
 *
 *  @param[in] param - attribute type
 *  @param[in] value - attribute value
 *
 *  @sa gaudio_set_float
 *  @sa gaudio_get_int32
 *  @sa gaudio_set_int32
 */
extern GAPI void GAPIENTRY gaudio_get_float(int32_t param,float* value);

/*! @brief set GAudio library 's integer attribute
 *
 *  This function set GAudio library 's integer attribute, which inclued the following type:
 *      AUDIO_PLAY_MODE
 *      AUDIO_RECORD_MODE
 *      AUDIO_ENUM_MUTE
 *      AUDIO_ENUM_ATTENUATION
 *
 *  @param[in] param - attribute type
 *  @param[in] value - attribute value
 *
 *  @note for instance gaudio_set_int32(AUDIO_ENUM_MUTE,TRUE);
 *  @note when param is AUDIO_ENUM_ATTENUATION, input value will be the following:
 *
 *      AUDIO_ATTENUATION_INVERSE_DISTANCE
 *      AUDIO_ATTENUATION_INVERSE_DISTANCE_CLAMPED
 *      AUDIO_ATTENUATION_LINEAR_DISTANCE
 *      AUDIO_ATTENUATION_LINEAR_DISTANCE_CLAMPED
 *      AUDIO_ATTENUATION_EXPONENT_DISTANCE
 *      AUDIO_ATTENUATION_EXPONENT_DISTANCE_CLAMPED
 *
 *  @sa gaudio_set_float
 *  @sa gaudio_get_float
 *  @sa gaudio_get_int32
 */
extern GAPI void GAPIENTRY gaudio_set_int32(int32_t param,int32_t value);

/*! @brief get GAudio library 's integer attribute
 *
 *  This function get GAudio library 's integer attribute, which inclued the following type:
 *      AUDIO_PLAY_MODE
 *      AUDIO_RECORD_MODE
 *      AUDIO_ENUM_MUTE
 *
 *  @param[in] param - attribute type
 *  @param[in] value - attribute value
 *
 *  @example for instance gaudio_set_int32(AUDIO_ENUM_MUTE,&value) for checking GAudio is in
 *  mute or not.
 *
 *  @sa gaudio_set_float
 *  @sa gaudio_get_float
 *  @sa gaudio_set_int32
 */
extern GAPI void GAPIENTRY gaudio_get_int32(int32_t param,int32_t* value);
extern GAPI void GAPIENTRY gaudio_set_string(int32_t param,const char* value);
extern GAPI int32_t GAPIENTRY gaudio_get_string(int32_t param,char* value);

/** \defgroup gerror gaudio error handling
 *
 * Functions to get code value version of the error numbers and an enumeration
 * of the error codes returned by gaudio.
 *
 * Most gaudio functions operating return GAUDIO_NO_ERROR on success and others for some thing error.
 * The positive range of return values is used for "useful" values when appropriate.
 *
 * @{
 */

/** Enumeration of the error codes and returned by gaudio functions. */
enum gerror_code
{
    AUDIO_NO_ERROR = 0,  /**< Success */
    AUDIO_NOT_INIT,      /**< No initialized */
    AUDIO_BAD_TAG,       /**< Invalid input tag parameter */
    AUDIO_BAD_VALUE,     /**< Invalid input value parameter */
    AUDIO_NOT_SUPPORT,   /**< GAudio don't support this function*/
    AUDIO_BAD_OPERATION, /**< This operation is prohibited */
    AUDIO_OUT_OF_MEMORY, /**< Out of memory -- some malloc() failed. */
    AUDIO_INNER_ERROR    /**< GAudio internal error */
};

/*! @brief get the plain errcode of GAudio SDK.
 *
 *  This function return the error code which be one of the following:
 *      AUDIO_NO_ERROR
 *      AUDIO_NOT_INIT
 *      AUDIO_BAD_TAG
 *      AUDIO_BAD_VALUE
 *      AUDIO_NOT_SUPPORT
 *      AUDIO_BAD_OPERATION
 *      AUDIO_OUT_OF_MEMORY
 *
 *  @return error code value
 */
extern GAPI int32_t GAPIENTRY gaudio_error_get();

/*! @brief set the listener's 3-demension information
 *
 *  This function set listener's 3-demension information, which inclued the following type:
 *      AUDIO_ENUM_POSITION
 *      AUDIO_ENUM_VELOCITY
 *      AUDIO_ENUM_FORWARD
 *      AUDIO_ENUM_UP
 *
 *  @param[in] param - the listener 's attribute type
 *
 *  @note This function only worked in 3D playing mode
 *
 *  @sa gaudio_listener_get_float3
 *  @sa gaudio_source_get_float3
 *  @sa gaudio_source_set_float3
 */
extern GAPI void GAPIENTRY gaudio_listener_set_float3(int32_t param,float x,float y,float z);

/*! @brief get the listener's 3-demension information
 *
 *  This function get listener's 3-demension information, which inclued the following type:
 *      AUDIO_ENUM_POSITION
 *      AUDIO_ENUM_VELOCITY
 *      AUDIO_ENUM_FORWARD
 *      AUDIO_ENUM_UP
 *
 *  @param[in] param - the listener 's attribute type
 *
 *  @note This function only worked in 3D playing mode
 *
 *  @sa gaudio_listener_set_float3
 *  @sa gaudio_source_get_float3
 *  @sa gaudio_source_set_float3
 */
extern GAPI void GAPIENTRY gaudio_listener_get_float3(int32_t param,float* x,float* y,float* z);

/*! @brief read data buffer from file.
 *
 *  This function read data buffer from file
 *
 *  @param[in] buf - the data buffer 's target.
 *  @param[in] size - the data 's size.
 *  @param[in] count - the data 's count.
 *  @param[in] ctx The file handler.
 *  @return actually read count of data
 *
 *  @note This function same as standard IO library function :fread
 *
 *  @sa gtell
 *  @sa gseek
 *  @sa gclose
 *
 *  @ingroup gsource
 */
typedef int32_t(GAPICONV* gread)(void* buf,int32_t size,int32_t count,void* ctx);

/*! @brief Seek given file 's reading position.
 *
 *  This function seek file 's reading position.
 *
 *  @param[in] ctx The file handler.
 *  @param[in] offset The offset of seeking.
 *  @param[in] where The seeking 's flag, which will be SEEK_SET,SEEK_CUR,SEEK_END (in stdio.h)
 *  @return 0 for success, or non-zera if the error occur.
 *
 *  @note This function same as standard IO library function :fseek
 *
 *  @sa gread
 *  @sa gtell
 *  @sa gclose
 *
 *  @ingroup gsource
 */
typedef int32_t(GAPICONV* gseek)(void* ctx,int32_t offset,int32_t where);

/*! @brief Tell given file 's current position.
 *
 *  This function getting current reading position of given audio file name
 *
 *  @param[in] ctx The file handler.
 *  @return reading position of given file
 *
 *  @note This function same as standard IO library function :ftell
 *
 *  @sa gread
 *  @sa gseek
 *  @sa gclose
 *
 *  @ingroup gsource
 */
typedef int32_t(GAPICONV* gtell)(void* ctx);

/*! @brief Close given file handler.
 *
 *  This function create gsource from given audio file name,
 *
 *  @param[in] ctx The file handler which needed to be closed.
 *  @return An gsource instance pointer, or `NULL` if the error occur.
 *
 *  @note This function same as standard IO library function :fclose
 *
 *  @sa gread
 *  @sa gseek
 *  @sa gtell
 *
 *  @ingroup gsource
 */
typedef int32_t(GAPICONV* gclose)(void* ctx);

/*! @brief Create gsource from given audio file name.
 *
 *  This function create gsource from given audio file name,
 *
 *  @param[in] file The file that will be loaded.
 *  @param[in] flag A flag value which script the load mode
 *  @return An gsource instance pointer, or `NULL` if the error occur.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_source_create_from_buffer
 *  @sa gaudio_source_create_from_virtual_io
 *  @sa gaudio_source_create_from_buffer2
 *  @sa gaudio_source_destroy
 *
 *  @ingroup gsource
 */
extern GAPI gsource* GAPIENTRY gaudio_source_create_from_file(const char* file,int32_t flag);

/*! @brief Create gsource by given buffer
 *
 *  This function create gsource from given buffer, the buffer length is pointed by user.
 *
 *  @param[in] buffer The buffer that will be loaded.
 *  @param[in] length The length of buffer
 *  @return An gsource instance pointer, or `NULL` if the error occur.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_source_create_from_file
 *  @sa gaudio_source_create_from_virtual_io
 *  @sa gaudio_source_create_from_buffer2
 *  @sa gaudio_source_destroy
 *
 *  @ingroup gsource
 */
extern GAPI gsource* GAPIENTRY gaudio_source_create_from_buffer(int8_t* buffer,uint32_t length);

/*! @brief Create gsource by user io
 *
 *  This function create gsource from user io, the io operation is pointed by user.
 *
 *  @param[in] read The read callback function.
 *  @param[in] seek The seek callback function.
 *  @param[in] tell The tell callback function.
 *  @param[in] close The close callback function.
 *  @param[in] user The user client data pointer.
 *  @return An gsource instance pointer, or `NULL` if the error occur.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_source_create_from_file
 *  @sa gaudio_source_create_from_buffer
 *  @sa gaudio_source_create_from_buffer2
 *  @sa gaudio_source_destroy
 *
 *  @ingroup gsource
 */
extern GAPI gsource* GAPIENTRY gaudio_source_create_from_virtual_io(gread read,gseek seek,gtell tell,gclose close,void* user);

/*! @brief Create gsource by given pcm data
 *
 *  This function create gsource from user io, the io operation is pointed by user.
 *
 *  @param[in] samplerate The given source's samplerate.
 *  @param[in] channels The given source's chgannels.
 *  @param[in] format The given source's format.
 *  @param[in] pcm The given source's data buffer.
 *  @param[in] length The length of given source data.
 *  @return An gsource instance pointer, or `NULL` if the error occur.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_source_create_from_file
 *  @sa gaudio_source_create_from_buffer
 *  @sa gaudio_source_create_from_virtual_io
 *  @sa gaudio_source_destroy
 *
 *  @ingroup gsource
 */
extern GAPI gsource* GAPIENTRY gaudio_source_create_from_buffer2(int32_t samplerate,int32_t channels,int32_t format,int8_t* pcm,int32_t length);

/*! @brief Destroy gsource instance.
 *
 *  This function destroy given gsource instance,
 *
 *  @param[in] source A gsource instance which will be destroyed.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_source_create_from_file
 *  @sa gaudio_source_create_from_buffer
 *  @sa gaudio_source_create_from_virtual_io
 *  @sa gaudio_source_create_from_buffer2
 *
 *  @ingroup gsource
 */
extern GAPI void  GAPIENTRY gaudio_source_destroy(gsource* source);

/*! @brief set gsource 's client data.
 *
 *  @param[in] source A gsource handler.
 *  @param[in] user A user data want to set.
 *
 *  @sa gaudio_source_get_user
 *
 *  @ingroup gsource
 */
extern GAPI void  GAPIENTRY gaudio_source_set_user(gsource* source,void* user);

/*! @brief get gsource 's client data.
 *
 *  @param[in]  source A gsource handler.
 *  @param[out] client data of gaudio handler.
 *
 *  @sa gaudio_source_set_user
 *
 *  @ingroup gsource
 */
extern GAPI void* GAPIENTRY gaudio_source_get_user(gsource* source);

/*! @brief The function signature for source content finish callbacks.
 *
 *  This is the function signature for source finish callback functions.
 *
 *  @param[in] source The source whose content needs to be triggered.
 *
 *  @sa gaudio_source_set_finish_callback
 *
 *  @ingroup gsource
 */
typedef void(GAPICONV* gfinish)(gsource* source);

/*! @brief Sets the gsource callback.
 *
 *  This function sets the playing finished callback of the specified gsource, which is
 *  called when source playing finished.
 *
 *  @param[in] source The source whose callback to set.
 *  @param[in] finish The new gfinish callback.
 *
 *  @ingroup gsource
 */
extern GAPI void GAPIENTRY gaudio_source_set_finish_callback(gsource* source,gfinish finish);

/*! @brief The function signature for gsource error callbacks.
 *
 *  This is the function signature for gsource error callback functions.
 *
 *  @param[in] source The source whose content needs to be triggered.
 *
 *  @sa gaudio_source_set_error_callback
 *
 *  @ingroup gsource
 */
typedef void(GAPICONV* gerror)(gsource* source,int32_t param,int32_t value);

/*! @brief Sets the gsource error callback.
 *
 *  This function sets the playing error callback of the specified gsource, which is
 *  called when error occur.
 *
 *  @param[in] source The source whose callback to set.
 *  @param[in] error The error callback function.
 *
 *  @ingroup gsource
 */
extern GAPI void GAPIENTRY gaudio_source_set_error_callback(gsource* source,gerror error);

/*! @brief get the source's 3-demension information
 *
 *  This function get source's 3-demension information, which inclued the following type:
 *      AUDIO_ENUM_POSITION
 *      AUDIO_ENUM_VELOCITY
 *      AUDIO_ENUM_FORWARD
 *      AUDIO_ENUM_UP
 *
 *  @param[in] param - the source's 3D attribute type
 *
 *  @note This function only worked in 3D playing mode
 *
 *  @sa gaudio_listener_set_float3
 *  @sa gaudio_listener_get_float3
 *  @sa gaudio_source_get_float3
 */
extern GAPI void GAPIENTRY gaudio_source_set_float3(gsource* source,int32_t param,float x,float y,float z);

/*! @brief set the source's 3-demension information
 *
 *  This function set source's 3-demension information, which inclued the following type:
 *      AUDIO_ENUM_POSITION
 *      AUDIO_ENUM_VELOCITY
 *      AUDIO_ENUM_FORWARD
 *      AUDIO_ENUM_UP
 *
 *  @param[in] param - the source's 3D attribute type
 *
 *  @note This function only worked in 3D playing mode
 *
 *  @sa gaudio_listener_set_float3
 *  @sa gaudio_listener_get_float3
 *  @sa gaudio_source_set_float3
 */
extern GAPI void GAPIENTRY gaudio_source_get_float3(gsource* source,int32_t param,float* x,float* y,float* z);

/*! @brief play the specified audio source.
 *
 *  This function play the specified audio source, if it was previously
 *  paused.
 *
 *  @param[in] source The audio source to play.
 *  @param[in] flag useless at present.
 *
 *  @sa gaudio_source_pause
 *  @sa gaudio_source_stop
 *
 *  @ingroup gsource
 */
extern GAPI void GAPIENTRY gaudio_source_play(gsource* source,int32_t flag);

/*! @brief play the specified audio source in 3D mode.
 *
 *  This function play the specified audio source with 3D mode, if it was previously
 *  paused.
 *
 *  @param[in] source The audio source to play.
 *  @param[in] flag useless at present.
 *
 *  @sa gaudio_source_start
 *  @sa gaudio_source_pause
 *  @sa gaudio_source_stop
 *
 *  @ingroup gsource
 */
extern GAPI void GAPIENTRY gaudio_source_play3(gsource* source,int32_t flag);

#define gaudio_source_start_3d gaudio_source_play3

/*! @brief pause the specified audio source.
 *
 *  This function pause the specified audio source, if it was previously
 *  playing.
 *
 *  @param[in] source The audio source to pause.
 *
 *  @sa gaudio_source_play
 *  @sa gaudio_source_play3
 *  @sa gaudio_source_stop
 *
 *  @ingroup gsource
 */
extern GAPI void GAPIENTRY gaudio_source_pause(gsource* source);

/*! @brief stop the specified audio source.
 *
 *  This function stop the specified audio source, if it was previously
 *  playing or pause.
 *
 *  @param[in] source The audio source to stop.
 *
 *  @sa gaudio_source_play
 *  @sa gaudio_source_play3
 *  @sa gaudio_source_pause
 *
 *  @ingroup gsource
 */
extern GAPI void GAPIENTRY gaudio_source_stop(gsource* source);

/*! @brief seek the specified audio source to given position.
 *
 *  This function seek the specified audio source, if it was previously
 *  playing or pause.
 *
 *  @param[in] source The audio source to seek.
 *
 *  @sa gaudio_source_play
 *  @sa gaudio_source_play3
 *  @sa gaudio_source_stop
 *  @sa gaudio_source_seek
 *
 *  @ingroup gsource
 */
extern GAPI void GAPIENTRY gaudio_source_seek(gsource* source,int32_t missecond);

/*! @brief The function signature for gsource position callbacks.
 *
 *  This is the function signature for gsource position callback functions.
 *
 *  @param[in] source The source whose content needs to be triggered.
 *
 *  @sa gaudio_source_set_position_callback
 *
 *  @ingroup gsource
 */
typedef void(GAPICONV* gposition)(gsource* source,int32_t position);

/*! @brief The function signature for gsource position callbacks.
 *
 *  Use this function set source 's position callback
 *
 *  @param[in] source The source whose content needs to be triggered.
 *  @param[in] position The callback function whose content will be added to source.
 *
 *  @sa gaudio_source_set_finish_callback
 *
 *  @ingroup gsource
 */
extern GAPI void GAPIENTRY gaudio_source_set_position_callback(gsource* source,gposition position);

/*! @brief set param value to source.
 *
 *  This function set source's float attribute' s information, which inclued the following type:
 *      AUDIO_ENUM_GAIN
 *
 *  @param[in] source A target source instance
 *  @param[in] param param type.
 *  @param[in] value param 's value
 *
 *  @sa gaudio_source_get_float3
 *  @sa gaudio_source_set_float3
 *  @sa gaudio_source_get_float
 *  @sa gaudio_source_get_int32
 *  @sa gaudio_source_set_int32
 *
 *  @ingroup gsource
 */
extern GAPI void GAPIENTRY gaudio_source_set_float(gsource* source,int32_t param,float value);

/*! @brief get param value from source.
 *
 *  This function get source's float attribute' s information, which inclued the following type:
 *      AUDIO_ENUM_GAIN
 *
 *  @param[in] source A target source instance
 *  @param[in] param param type.
 *  @param[in] return param 's value.
 *
 *  @sa gaudio_source_get_float3
 *  @sa gaudio_source_set_float3
 *  @sa gaudio_source_set_float
 *  @sa gaudio_source_get_int32
 *  @sa gaudio_source_set_int32
 *
 *  @ingroup gsource
 */
extern GAPI void GAPIENTRY gaudio_source_get_float(gsource* source,int32_t param,float* value);

/*! @brief set param value from source.
 *
 *  This function get source's integer attribute' s information, which inclued the following type:
 *      AUDIO_ENUM_LOOP
 *      AUDIO_ENUM_MUTE
 *
 *  @param[in] source A target source instance
 *  @param[in] param param type.
 *  @param[in] return param 's value.
 *
 *  @sa gaudio_source_get_float3
 *  @sa gaudio_source_set_float3
 *  @sa gaudio_source_set_float
 *  @sa gaudio_source_get_float
 *  @sa gaudio_source_set_int32
 *
 *  @ingroup gsource
 */
extern GAPI void GAPIENTRY gaudio_source_set_int32(gsource* source,int32_t param,int32_t value);

/*! @brief get param value from source.
 *
 *  This function get source's integer attribute' s information, which inclued the following type:
 *      AUDIO_ENUM_LOOP
 *      AUDIO_ENUM_MUTE
 *      AUDIO_ENUM_SAMPLERATE
 *      AUDIO_ENUM_CHANNEL
 *      AUDIO_ENUM_FORMAT
 *      AUDIO_ENUM_DURATION
 *
 *  @param[in] source A target source instance
 *  @param[in] param param type.
 *  @param[in] return param 's value.
 *
 *  @sa gaudio_source_get_float3
 *  @sa gaudio_source_set_float3
 *  @sa gaudio_source_set_float
 *  @sa gaudio_source_get_float
 *  @sa gaudio_source_get_int32
 *
 *  @ingroup gsource
 */
extern GAPI void GAPIENTRY gaudio_source_get_int32(gsource* source,int32_t param,int32_t* value);

/*! @brief get param string value from source.
 *
 *  This function get source's integer attribute' s information, which inclued the following type:
 *      AUDIO_ENUM_TITLE
 *      AUDIO_ENUM_TRACK
 *      AUDIO_ENUM_ALBUM
 *      AUDIO_ENUM_ARTIST
 *      AUDIO_ENUM_PUBLISHDATE
 *      AUDIO_ENUM_GENRE
 *      AUDIO_ENUM_COMMENT
 *
 *  @param[in] source A target source instance
 *  @param[in] param param type.
 *  @param[in] return param 's value.
 *
 *  @sa gaudio_source_get_float3
 *  @sa gaudio_source_set_float3
 *  @sa gaudio_source_set_float
 *  @sa gaudio_source_get_float
 *  @sa gaudio_source_get_int32
 *
 *  @ingroup gsource
 */
extern GAPI int32_t GAPIENTRY gaudio_source_get_string(gsource* source,int32_t param,char* value);

/*! @brief Create geffect instance.
 *
 *  This function create given geffect instance,
 *
 *  @param[in] effect A effect id which will be created.
  * @param[out] A effect pointer instance, NULL for error occur.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_effect_destroy
 *
 *  @ingroup geffect
 */
extern GAPI geffect* GAPIENTRY gaudio_effect_create(int32_t effect,int32_t flag);

/*! @brief Destroy geffect instance.
 *
 *  This function destroy given geffect instance,
 *
 *  @param[in] effect A effect instance which will be destroyed.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_effect_create
 *
 *  @ingroup geffect
 */
extern GAPI void GAPIENTRY gaudio_effect_destroy(geffect* effect);

/*! @brief The function signature for geffect fft callbacks.
 *
 *  This is the function signature for geffect fft calc callback functions.
 *
 *  @param[in] source The source whose content needs to be triggered.
 *
 *  @ingroup geffect
 */
typedef void(GAPICONV* gefxcb)(geffect*,float*,int32_t,int32_t,int32_t);
extern GAPI void GAPIENTRY gaudio_effect_set_fft_callback(geffect*,gefxcb);

/*! @brief The function signature for geffect pcm callbacks.
 *
 *  This is the function signature for geffect pcm callback functions.
 *
 *  @param[in] source The source whose content needs to be triggered.
 *
 *  @ingroup geffect
 */
typedef void(GAPICONV* gefxpcmcb)(geffect* efx,float** pcm,int32_t channel,int32_t samplerate,int32_t length);
extern GAPI void GAPIENTRY gaudio_effect_set_pcm_callback(geffect* efx,gefxpcmcb cb);

/*! @brief bind effect with given source.
 *
 *  @param[in] effect An effect pointer which will be bind to source.
 *  @param[in] flag useless present.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_effect_unbind
 *  @sa gaudio_effect_unbind_from_source
 *  @sa gaudio_effect_bind_to_source
 *
 *  @ingroup geffect
 */
extern GAPI void GAPIENTRY gaudio_effect_bind(geffect* effect,int32_t flag);

/*! @brief bind effect with given source.
 *
 *  @param[in] effect An effect pointer which will be bind to source.
 *  @param[in] source A source pointer which will be bind to effect.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_effect_bind
 *  @sa gaudio_effect_unbind
 *  @sa gaudio_effect_unbind_from_source
 *
 *  @ingroup geffect
 */
extern GAPI void GAPIENTRY gaudio_effect_bind_to_source(geffect* effect,gsource* source);

/*! @brief unbind effect from global context.
 *
 *  @param[in] effect An effect pointer which will be unbind from global.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_effect_bind_to_source
 *  @sa gaudio_effect_unbind
 *  @sa gaudio_effect_unbind_from_source
 *
 *  @ingroup geffect
 */
extern GAPI void GAPIENTRY gaudio_effect_unbind(geffect* effect);

/*! @brief unbind effect with given source.
 *
 *  @param[in] effect An effect pointer which will be unbind to source.
 *  @param[in] source A source pointer which will be unbind to effect.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_effect_bind
 *  @sa gaudio_effect_unbind
 *  @sa gaudio_effect_bind_to_source
 *
 *  @ingroup geffect
 */
extern GAPI void GAPIENTRY gaudio_effect_unbind_from_source(geffect* effect,gsource* source);

/*! @brief set effect object 's float attribute
 *
 *  This function set given effect object 's float attribute, param and value detailed are decided by effect object detailed
 *
 *  @note bassboost, for instance, gaudio_set_float(bassboost,AUDIO_EFFECT_BASSBOOST_FREQUENCE,10.0f);
 *
 *  @sa gaudio_effect_get_float
 *  @sa gaudio_effect_set_int32
 *  @sa gaudio_effect_get_int32
 */
extern GAPI void GAPIENTRY gaudio_effect_set_float(geffect* effect,int32_t param,float value);

/*! @brief get effect object 's float attribute
 *
 *  This function get given effect object 's float attribute, param detailed are decided by effect object detailed
 *
 *  @note for bassboost effect, you can use gaudio_get_float(bassboost,AUDIO_EFFECT_BASSBOOST_FREQUENCE,&value).
 *
 *  @sa gaudio_effect_set_float
 *  @sa gaudio_effect_set_int32
 *  @sa gaudio_effect_get_int32
 */
extern GAPI void GAPIENTRY gaudio_effect_get_float(geffect* effect,int32_t param,float* value);
#define AUDIO_EFFECT_ENABLE  0

/*! @brief set effect object 's integer attribute
 *
 *  This function set given effect object 's integer attribute, param detailed are decided by effect object detailed
 *
 *  @note modulator, for instance, gaudio_set_int32(modulator,AUDIO_EFFECT_MODULATOR_WAVEFORM,AUDIO_EFFECT_MODULATOR_SINUSOID);
 *
 *  @sa gaudio_effect_set_float
 *  @sa gaudio_effect_get_float
 *  @sa gaudio_effect_get_int32
 */
extern GAPI void GAPIENTRY gaudio_effect_set_int32(geffect* effect,int32_t param,int32_t value);

/*! @brief get effect object 's integer attribute
 *
 *  This function get given effect object 's integer attribute, param detailed are decided by effect object detailed
 *
 *  @note modulator, for instance, gaudio_get_int32(modulator,AUDIO_EFFECT_MODULATOR_WAVEFORM,&value);
 *
 *  @sa gaudio_effect_set_float
 *  @sa gaudio_effect_get_float
 *  @sa gaudio_effect_set_int32
 */
extern GAPI void GAPIENTRY gaudio_effect_get_int32(geffect* effect,int32_t param,int32_t* value);

/*! @brief create catcher handler by given parameters
 *
 *  This function create catcher handler, input param are audio filename and audio format respectively.
 *
 *  @note output samplerate is 44100hz.
 *  @note output audio channel is 2.
 *  @note output data format is 16-bits.
 *
 *  @sa gaudio_catcher_destroy
 */
typedef void (GAPICONV* gcatcher_callback)(void* ptr,uint32_t channel,uint32_t format,uint32_t samplerate,int16_t* buffer,uint32_t length);
extern GAPI gcatcher* GAPIENTRY gaudio_catcher_create(gcatcher_callback callback,uint32_t channels,uint32_t format,uint32_t samplerate,uint32_t flag);

/*! @brief switch catcher status as write. And then, catcher will write captured pcm data.
 *
 *  @sa gaudio_catcher_pause
 *  @sa gaudio_catcher_stop
 */
extern GAPI void GAPIENTRY gaudio_catcher_write(gcatcher* catcher);

/*! @brief switch gcatcher status as pause mode. And then gcatcher will be suspended.
 *
 *  @sa gaudio_catcher_pause
 *  @sa gaudio_catcher_stop
 */
extern GAPI void GAPIENTRY gaudio_catcher_pause(gcatcher* catcher);

/*! @brief switch catcher status as stop. And then gcatcher will be closed.
 *
 *  @sa gaudio_catcher_destroy
 *  @sa gaudio_catcher_pause
 *  @sa gaudio_catcher_stop
 */
extern GAPI void GAPIENTRY gaudio_catcher_stop(gcatcher* catcher);

/*! @brief Destroy grecorder instance.
 *
 *  This function destroy given gcatcher instance,
 *
 *  @param[in] source A gcatcher instance which will be destroyed.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_catcher_create
 *
 *  @ingroup gcatcher
 */
extern GAPI void GAPIENTRY gaudio_catcher_destroy(gcatcher* catcher);

/*! @brief Returns the address of the specified function for the current
 *  context.
 *
 *  This function returns the address of the specified
 *  [client API or extension function], if it is supported
 *  by the current context.
 *
 *  @param[in] name The ASCII encoded name of the function.
 *  @return The address of the function, or `NULL` if the function is
 *  unavailable.
 *
 *  @remarks This function may be called from secondary threads.
 *
 *  @note The addresses of these functions are not guaranteed to be the same for
 *  all contexts, especially if they use different client APIs or even different
 *  context creation hints.
 *
 *  @ingroup context
 */
extern GAPI void* GAPIENTRY gaudio_address_get(const char* name);

#if defined(__cplusplus)
}
#endif

/*! \file gaudio.h
    \brief Main header file of the GAudio SDK library, the only file needed to include.
*/

/*************************************************************************
 * Global definition cleanup
 *************************************************************************/

#endif  /** _gaudio_h_ */
//! by GAudio Group.
