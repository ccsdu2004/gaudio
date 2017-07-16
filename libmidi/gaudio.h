/*************************************************************************
 * GAudio library - An Audio library
 * API version: 1.9.8.2
 * WWW:         http://www.gaudio.org/
 *------------------------------------------------------------------------
 * Copyright (c) 2013-2013 Eric Du <283317457@qq.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would
 *    be appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
 *************************************************************************/
 
#ifndef LIB_G_AUDIO_SDK_H
#define LIB_G_AUDIO_SDK_H
#include <stdint.h>

#if defined(__cplusplus)
extern "C"
{
#endif /* __cplusplus */

/*************************************************************************
 * Doxygen documentation
 *************************************************************************/

/* ---------------- GAudio library related system specific defines ----------------- */ 
#if !defined(WIN32) && !defined(__linux__)
   #error "gaudio library don't support this os at present"
#endif

#if defined(_WIN32)
 #define GAPIENTRY __cdecl
#else
 #define GAPIENTRY
#endif

#if defined(BUILDING)
#ifdef _WIN32
# define GAPI __declspec(dllexport)
#else
# define GAPI
#endif
#else
#ifdef _WIN32
# define GAPI __declspec(dllimport)
#else
# define GAPI
#endif
#endif

#ifndef NULL
   #define NULL 0
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    1
#endif

/*************************************************************************
 * GAudio library API tokens
 *************************************************************************/

#define AUDIO_DRIVER_WINMM     1
#define AUDIO_DRIVER_DSOUND    2
#define AUDIO_DRIVER_ALSA      3
#define AUDIO_DRIVER_OSS       4
#define AUDIO_DRIVER_PORTAUDIO 5

#define AUDIO_PLUGIN_DECODER 1
#define AUDIO_PLUGIN_EFFECT  2
#define AUDIO_PLUGIN_DRIVER  3

#define MAX_SOURCE  8
#define MAX_DECODER 32
#define MAX_EFFECT  8

#define POSITION    1
#define VELOCITY    2
#define FORWARD     3
#define UP          4
#define SAMPLERATE  5
#define CHANNEL     6
#define FORMAT      7
#define DURATION    8
#define LOOP        9
#define GAIN        10
#define GAIN_LEFT   11
#define GAIN_RIGHT  12
#define SILENCE     13
#define TITLE       14
#define TRACK       15
#define ALBUM       16
#define ARTIST      17
#define PUBLISHDATE 18
#define GENRE       19
#define COMMENT     20
#define AUDIO       21
#define BITERATE    22
#define VERSION     23
#define DRIVER      24
#define BALANCE     25
#define CROSSFEDD   26
#define SOUNDFONT   27

#define BALANCE_MIN     -10
#define BALANCE_MAX      10
#define BALANCE_DEFAULT   0

#define CROSSFEED_MIN     0
#define CROSSFEED_MAX     2
#define CROSSFEED_DEFAULT 1

typedef int32_t(GAPIENTRY* gread)(void*,int32_t,int32_t,void*);
typedef int32_t(GAPIENTRY* gseek)(void*,int32_t,int32_t);
typedef int32_t(GAPIENTRY* gtell)(void*);
typedef int32_t(GAPIENTRY* gclose)(void*);

typedef struct gsource   gsource;
typedef struct geffect   geffect;
typedef struct grecorder grecorder;

/*************************************************************************
 * GAudio API functions
 *************************************************************************/

/*! @brief Initializes the GAudio library.
 *
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
 *  @param[in] add-ons Where to search and load plug-ins which GAudio needed. 
 *
 *  @return `TRUE` if successful, or `FALSE` if an error occurred.
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
extern GAPI void gaudio_init(const char* addons);

/*! @brief Terminates the GAudio library.
 *
 *  This function frees any allocated resources and sets the library to an 
 *  uninitialized state.  Once this is called, you must again call @ref gaudio_init 
 *  successfully before you will be able to use most GAudio functions.
 *
 *  If GAudio has been successfully initialized, this function should be called
 *  before the program exits.  If initialization fails, there is no need to call
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
extern GAPI void gaudio_deinit();

/* Audio format type */
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

/* Channel type */
#define DEVICE_CHANNEL_MONO    1
#define DEVICE_CHANNEL_STEREO  (DEVICE_CHANNEL_MONO+1)
#define DEVICE_CHANNEL_QUAD    (DEVICE_CHANNEL_STEREO+1)
#define DEVICE_CHANNEL_5POINT1 (DEVICE_CHANNEL_QUAD+1)
#define DEVICE_CHANNEL_6POINT1 (DEVICE_CHANNEL_5POINT1+1)
#define DEVICE_CHANNEL_7POINT1 (DEVICE_CHANNEL_6POINT1+1)

#define AUDIO_VERSON       0
#define AUDIO_PLAY_MODE    1
#define AUDIO_RECORD_MODE  2

/* GAudio plugin type */
#define AUDIO_PLUGIN_AUDIO    1
#define AUDIO_PLUGIN_EFFECT   2
#define AUDIO_PLUGIN_DRIVER   3
#define AUDIO_PLUGIN_RECORDER 4

/* Audio file type */
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

/* Audio effect type */
#define AUDIO_EFFECT_NONE                  -1
#define AUDIO_EFFECT_PHASER                 0
#define AUDIO_EFFECT_INVERT                 1
#define AUDIO_EFFECT_SWAP_CHANNEL           2
#define AUDIO_EFFECT_MERGE_CHANNEL          3
#define AUDIO_EFFECT_SPEED_CHANGER          4
#define AUDIO_EFFECT_ECHO                   5
#define AUDIO_EFFECT_REVERB                 6
#define AUDIO_EFFECT_FFT                    7
#define AUDIO_EFFECT_EQUALIZER              8
#define AUDIO_EFFECT_BASSBOOST              9
#define AUDIO_EFFECT_CHORUS                 10
#define AUDIO_EFFECT_STEREO_ENHANCE         11
#define AUDIO_EFFECT_DISTORTION             12
#define AUDIO_EFFECT_FOLDBACK_DISTORTION    13
#define AUDIO_EFFECT_FLANGER                14
#define AUDIO_EFFECT_AUTOWAH                15
#define AUDIO_EFFECT_COMPRESSER             16
#define AUDIO_EFFECT_MODULATOR              17
#define AUDIO_EFFECT_FADE_IN                18
#define AUDIO_EFFECT_FADE_OUT               19

extern GAPI int32_t gaudio_is_support(int32_t,int32_t);

extern GAPI void gaudio_set_float(int32_t,float);
extern GAPI void gaudio_get_float(int32_t,float*);
extern GAPI void gaudio_set_int32(int32_t,int32_t);
extern GAPI void gaudio_get_int32(int32_t,int32_t*);
extern GAPI void gaudio_set_string(int32_t,const char*);
extern GAPI int32_t gaudio_get_string(int32_t,char*);

/*! @defgroup errors Error codes
 *  @ingroup error
 *  @{ */
/*! @brief GAudio has not been initialized.
 */
#define AUDIO_NO_ERROR      0
/*! @brief GAudio has not been initialized.
 */
#define AUDIO_NOT_INIT      1
#define AUDIO_BAD_TAG       2
#define AUDIO_BAD_VALUE     3
/*! @brief GAudio could not support for the requested client API on the
 *  system.
 */
#define AUDIO_NOT_SUPPORT   4
#define AUDIO_BAD_OPERATION 5
/*! @brief A memory allocation failed.
 */
#define AUDIO_OUT_OF_MEMORY 6
#define AUDIO_OVER_COUNT    7
#define AUDIO_INNER_ERROR   8

extern GAPI int32_t gaudio_error_get();
extern GAPI void gaudio_source_set_float(gsource*,int32_t,float);
extern GAPI void gaudio_source_get_float(gsource*,int32_t,float*);
extern GAPI void gaudio_source_set_int32(gsource*,int32_t,int32_t);
extern GAPI void gaudio_source_get_int32(gsource*,int32_t,int32_t*);
extern GAPI int32_t gaudio_source_get_string(gsource*,int32_t,char*);
extern GAPI void gaudio_listener_set_float3(int32_t,float,float,float);
extern GAPI void gaudio_listener_get_float3(int32_t,float*,float*,float*);

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
extern GAPI gsource* gaudio_source_create_from_file(const char* file,int32_t flag);
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
extern GAPI gsource* gaudio_source_create_from_buffer(int8_t* buffer,uint32_t length);
extern GAPI gsource* gaudio_source_create_from_virtual_io(gread,gseek,gtell,gclose,void*);
extern GAPI gsource* gaudio_source_create_from_buffer2(int8_t*,uint32_t,uint32_t,uint32_t,uint32_t);
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
extern GAPI void GAPIENTRY gaudio_source_destroy(gsource* source);
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
typedef void(GAPIENTRY* gfinish)(gsource* source);
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
extern GAPI void gaudio_source_set_finish_callback(gsource* source,gfinish finish);
typedef void(GAPIENTRY* gerror)(gsource*,int32_t,int32_t);
extern GAPI void gaudio_source_set_error_callback(gsource*,gerror);
extern GAPI void gaudio_source_set(gsource*,int32_t,float,float,float);
extern GAPI void gaudio_source_get(gsource*,int32_t,float*,float*,float*);
extern GAPI void gaudio_source_start(gsource* source,int32_t flag);
extern GAPI void gaudio_source_start_3d(gsource*,int32_t);
extern GAPI void gaudio_source_pause(gsource*);
extern GAPI void gaudio_source_stop(gsource*);
extern GAPI void gaudio_source_seek(gsource*,int32_t);
typedef void(GAPIENTRY* gposition)(gsource*,int32_t);
extern GAPI void gaudio_source_set_position_callback(gsource* source,gposition);

/*! @brief Create geffect instance.
 *
 *  This function create given geffect instance,
 *
 *  @param[in] effect A effect id which will be created.
  * @param[out] A effect pointer instance, NULL for error occur.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_effect_create
 *
 *  @ingroup geffect
 */
extern GAPI geffect* gaudio_effect_create(int32_t effect,int32_t flag);
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
extern GAPI void gaudio_effect_destroy(geffect* effect);
typedef void(GAPIENTRY* gefxcb)(geffect*,float*,int32_t,int32_t,int32_t);
extern GAPI void gaudio_effect_set_callback(geffect*,gefxcb);
extern GAPI void gaudio_effect_bind(geffect* effect,int32_t flag);
extern GAPI void gaudio_effect_bind_to_source(geffect*,gsource*);
extern GAPI void gaudio_effect_unbind(geffect*);
extern GAPI void gaudio_effect_unbind_from_source(geffect*,gsource*);
extern GAPI void gaudio_effect_set_float(geffect*,int32_t,float);
extern GAPI void gaudio_effect_get_float(geffect*,int32_t,float*);
#define AUDIO_EFFECT_ENABLE  0
extern GAPI void gaudio_effect_set_int32(geffect*,int32_t,int32_t);
extern GAPI void gaudio_effect_get_int32(geffect*,int32_t,int32_t*);

extern GAPI grecorder* gaudio_recorder_create(const char*,uint32_t,uint32_t,uint32_t,uint32_t);
extern GAPI void gaudio_recorder_start(grecorder*);
extern GAPI void gaudio_recorder_stop(grecorder*);
/*! @brief Destroy grecorder instance.
 *
 *  This function destroy given grecorder instance,
 *
 *  @param[in] source A grecorder instance which will be destroyed.
 *
 *  @note This function may only be called from the main thread.
 *
 *  @sa gaudio_recorder_create
 *
 *  @ingroup grecorder
 */
extern GAPI void gaudio_recorder_destroy(grecorder* recorder);

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
extern GAPI void* gaudio_address_get(const char* name);

#if defined(__cplusplus)
}
#endif

/*************************************************************************
 * Global definition cleanup
 *************************************************************************/

#endif  /* _gaudio_h_ */
//! by duwenhua2013









