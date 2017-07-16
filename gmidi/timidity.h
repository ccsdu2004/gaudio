/*

    libTiMidity -- MIDI to WAVE converter library
    Copyright (C) 1995 Tuukka Toivonen <toivonen@clinet.fi>
    Copyright (C) 2004 Konstantin Korikov <lostclus@ua.fm>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef TIMIDITY_H
#define TIMIDITY_H

#include <stdio.h>
#include <stdlib.h>
#include "streamBuffer.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef HAVE_MIDI
#ifdef LIBMIDI_EXPORTS
#define MIDI_API __declspec(dllexport)
#else
#define MIDI_API __declspec(dllimport)
#endif
#else
#define MIDI_API
#endif

#define LIBTIMIDITY_VERSION_MAJOR 0L
#define LIBTIMIDITY_VERSION_MINOR 1L
#define LIBTIMIDITY_PATCHLEVEL    0L

#define LIBTIMIDITY_VERSION \
        ((LIBTIMIDITY_VERSION_MAJOR<<16)| \
         (LIBTIMIDITY_VERSION_MINOR<< 8)| \
         (LIBTIMIDITY_PATCHLEVEL))

/* Audio format flags (defaults to LSB byte order)
 */
#define MID_AUDIO_U8        0x0008	/* Unsigned 8-bit samples */
#define MID_AUDIO_S8        0x8008	/* Signed 8-bit samples */
#define MID_AUDIO_U16LSB    0x0010	/* Unsigned 16-bit samples */
#define MID_AUDIO_S16LSB    0x8010	/* Signed 16-bit samples */
#define MID_AUDIO_U16MSB    0x1010	/* As above, but big-endian byte order */
#define MID_AUDIO_S16MSB    0x9010	/* As above, but big-endian byte order */
#define MID_AUDIO_U16       MID_AUDIO_U16LSB
#define MID_AUDIO_S16       MID_AUDIO_S16LSB

/* Core Library Types
 */
  typedef unsigned char uint8;
  typedef signed char sint8;
  typedef unsigned short uint16;
  typedef signed short sint16;
  typedef unsigned int uint32;
  typedef signed int sint32;

 
  typedef struct _MidDLSPatches MidDLSPatches;
  typedef struct _MidSong MidSong;

  typedef struct _MidSongOptions MidSongOptions;
  struct _MidSongOptions
  {
    sint32 rate;	  /* DSP frequency -- samples per second */
    uint16 format;	  /* Audio data format */
    uint8 channels;	  /* Number of channels: 1 mono, 2 stereo */
    uint16 buffer_size;	  /* Sample buffer size in samples */
  };

  typedef enum
  {
    MID_SONG_TEXT = 0,
    MID_SONG_COPYRIGHT = 1
  } MidSongMetaId;
 
 extern MIDI_API int  mid_init(char* config);
 extern MIDI_API int  mid_init_no_config(void);
 extern MIDI_API void mid_exit(void);
 extern MIDI_API MidDLSPatches* mid_dlspatches_load(streamBuffer* stream);
 extern MIDI_API void mid_dlspatches_free(MidDLSPatches* patches);
 extern MIDI_API MidSong *mid_song_load(streamBuffer* stream,MidSongOptions* options);
 extern MIDI_API MidSong *mid_song_load_dls(streamBuffer* stream,
				     MidDLSPatches * patches,
				     MidSongOptions * options);


 extern MIDI_API void mid_song_set_volume(MidSong* song,int volume);
 extern MIDI_API void mid_song_start(MidSong* song);
 extern MIDI_API size_t mid_song_read_wave(MidSong* song,void *ptr,size_t size);
 extern MIDI_API void mid_song_seek(MidSong* song,uint32 ms);
 extern MIDI_API uint32 mid_song_get_total_time(MidSong* song);
 extern MIDI_API uint32 mid_song_get_time(MidSong* song);
 extern MIDI_API char *mid_song_get_meta(MidSong* song,MidSongMetaId what);
 extern MIDI_API void mid_song_free(MidSong * song);

#ifdef __cplusplus
}
#endif
#endif				/* TIMIDITY_H */
