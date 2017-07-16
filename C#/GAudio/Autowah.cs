using System;

namespace gaudio.effect
{
    public class Autowah
    {
        public const int AUDIO_EFFECT_AUTOWAH_FREQ = 1;
        public const int AUDIO_EFFECT_AUTOWAH_FREQOFF = 2;
        public const int AUDIO_EFFECT_AUTOWAH_PHASE = 3;
        public const int AUDIO_EFFECT_AUTOWAH_DEPTH = 4;
        public const int AUDIO_EFFECT_AUTOWAH_RESOURCE = 5;

        public const float AUDIO_EFFECT_AUTOWAH_FREQ_LOW = .0f;
        public const float AUDIO_EFFECT_AUTOWAH_FREQ_HIGH = 40.0f;
        public const float AUDIO_EFFECT_AUTOWAH_FREQ_DEFAULT = 2.0f;

        public const float AUDIO_EFFECT_AUTOWAH_FREQOFF_LOW = .0f;
        public const float AUDIO_EFFECT_AUTOWAH_FREQOFF_HIGH = 100.0f;
        public const float AUDIO_EFFECT_AUTOWAH_FREQOFF_DEFAULT = 20.0f;

        public const float AUDIO_EFFECT_AUTOWAH_PHASE_LOW = .0f;
        public const float AUDIO_EFFECT_AUTOWAH_PHASE_HIGH = 359.0f;
        public const float AUDIO_EFFECT_AUTOWAH_PHASE_DEFAULT = 0.0f;

        public const float AUDIO_EFFECT_AUTOWAH_DEPTH_LOW = .0f;
        public const float AUDIO_EFFECT_AUTOWAH_DEPTH_HIGH = 100.0f;
        public const float AUDIO_EFFECT_AUTOWAH_DEPTH_DEFAULT = 70.0f;

        public const float AUDIO_EFFECT_AUTOWAH_RESOURCE_LOW = .0f;
        public const float AUDIO_EFFECT_AUTOWAH_RESOURCE_HIGH = 100.0f;
        public const float AUDIO_EFFECT_AUTOWAH_RESOURCE_DEFAULT = 24.0f;
    }
}