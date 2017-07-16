using System;

namespace gaudio.effect
{
    public class Modulator
    {
        public const int AUDIO_EFFECT_MODULATOR_FREQUENCY = 1;
        public const int AUDIO_EFFECT_MODULATOR_HIGHPASS_CUTOFF  = 2;
        public const int AUDIO_EFFECT_MODULATOR_WAVEFORM         = 3;

        public const float AUDIO_EFFECT_MODULATOR_FREQUENCY_MIN    = .0f;
        public const float AUDIO_EFFECT_MODULATOR_FREQUENCY_MAX    = 8000.0f;
        public const float AUDIO_EFFECT_MODULATOR_FREQUENCY_DEFAULT= 440.0f;

        public const float AUDIO_EFFECT_MODULATOR_HIGHPASS_CUTOFF_MIN    = .0f;
        public const float AUDIO_EFFECT_MODULATOR_HIGHPASS_CUTOFF_MAX    = 24000.0f;
        public const float AUDIO_EFFECT_MODULATOR_HIGHPASS_CUTOFF_DEFAULT= 800.0f;

        public const int AUDIO_EFFECT_MODULATOR_SINUSOID = 0;
        public const int AUDIO_EFFECT_MODULATOR_SAWTOOTH = 1;
        public const int AUDIO_EFFECT_MODULATOR_SQUARE   = 2;

        public const int AUDIO_EFFECT_MODULATOR_WAVEFORM_MIN    =  AUDIO_EFFECT_MODULATOR_SINUSOID;
        public const int AUDIO_EFFECT_MODULATOR_WAVEFORM_MAX    = AUDIO_EFFECT_MODULATOR_SQUARE;
        public const int AUDIO_EFFECT_MODULATOR_WAVEFORM_DEFAULT= AUDIO_EFFECT_MODULATOR_SINUSOID;
    }
}