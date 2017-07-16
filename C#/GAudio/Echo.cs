using System;

namespace gaudio.effect
{
    public class Echo
    {
        public const int AUDIO_EFFECT_ECHO_DELAY    = 1;
        public const int AUDIO_EFFECT_ECHO_DECAY    = 2;

        public const float AUDIO_EFFECT_ECHO_DELAY_MIN = 0.2f;
        public const float AUDIO_EFFECT_ECHO_DELAY_MAX = 1.0f;
        public const float AUDIO_EFFECT_ECHO_DELAY_DEFAULT = 0.5f;

        public const float AUDIO_EFFECT_ECHO_DECAY_MIN = 0.1f;
        public const float AUDIO_EFFECT_ECHO_DECAY_MAX = 1.0f;
        public const float AUDIO_EFFECT_ECHO_DECAY_DEFAULT = 0.2f;
    }
}