using System;

namespace gaudio
{
    public enum GDriverCode
    {
        AUDIO_DRIVER_NULL = 0,  /**< dummy audio driver plugin. */
        AUDIO_DRIVER_WINMM,     /**< Macrosoft 's winmm plugin. */
        AUDIO_DRIVER_DSOUND,    /**< Macrosoft 's dsound plugin. */
        AUDIO_DRIVER_ALSA,      /**< Advanced Linux Sound Arthiecture 's driver plugin. */
        AUDIO_DRIVER_OSS,       /**< Open Sound System driver plugin. */
        AUDIO_DRIVER_COREAUDIO, /**< Apple Inc. 's audio driver plugin. */
        AUDIO_DRIVER_PORTAUDIO  /**< Open source - Real-Time Audio Library 's driver plugin. */
    }
}
