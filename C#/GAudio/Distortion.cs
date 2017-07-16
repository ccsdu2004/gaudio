using System;

namespace gaudio.effect
{
    public class Distortion
    {
        public const int AUDIO_EFFECT_DISTORTION_THRESHOLD = 1;
        public const int AUDIO_EFFECT_DISTORTION_GAIN = 2;

        public const float AUDIO_EFFECT_DISTORTION_THRESHOLD_LOW = .72f;
        public const float AUDIO_EFFECT_DISTORTION_THRESHOLD_HIGH = 1.0f;
        public const float AUDIO_EFFECT_DISTORTION_THRESHOLD_DEFAULT = 0.72f;

        public const float AUDIO_EFFECT_DISTORTION_GAIN_LOW = .86f;
        public const float AUDIO_EFFECT_DISTORTION_GAIN_HIGH = 1.2f;
        public const float AUDIO_EFFECT_DISTORTION_GAIN_DEFAULT = 1.0f;
    }
}