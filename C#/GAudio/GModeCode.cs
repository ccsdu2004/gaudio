using System;

namespace gaudio
{
    public enum GModeCode
    {
        AUDIO_PLAY_MODE = 1,    /**< current is playing mode. */
        AUDIO_RECORD_MODE,      /**< current is record/encoder mode. */
        AUDIO_WRITE_MODE = AUDIO_RECORD_MODE,
    }
}