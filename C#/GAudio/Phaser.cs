using System;

namespace gaudio.effect
{
    public class Phaser
    {
        public const int AUDIO_EFFECT_PHASER_SAMPLERATE         = 1;
		public const int AUDIO_EFFECT_PHASER_DEPTH              = 2;
        public const int AUDIO_EFFECT_PHASER_PHASE              = 3;
        public const int AUDIO_EFFECT_PHASER_STAGE              = 4;
        public const int AUDIO_EFFECT_PHASER_DRY                = 5;
        public const int AUDIO_EFFECT_PHASER_FB                 = 6;

        public const float AUDIO_EFFECT_PHASER_FREQUENCY_LOW    = .0f;
        public const float AUDIO_EFFECT_PHASER_FREQUENCY_HIGH   = 40.0f;
        public const float AUDIO_EFFECT_PHASER_FREQUENCY_DEFAULT= .5f;

        public const float AUDIO_EFFECT_PHASER_PHASE_LOW        = .0f;
        public const float AUDIO_EFFECT_PHASER_PHASE_HIGH       = 359.0f;
        public const float AUDIO_EFFECT_PHASER_PHASE_DEFAULT    = .0f;
 
        public const float AUDIO_EFFECT_PHASER_DEPTH_LOW        = .0f;
        public const float AUDIO_EFFECT_PHASER_DEPTH_HIGH       = 255.0f; 
        public const float AUDIO_EFFECT_PHASER_DEPTH_DEFAULT    = 75.0f;
 
        public const float AUDIO_EFFECT_PHASER_STAGE_LOW        = 2.0f;
        public const float AUDIO_EFFECT_PHASER_STAGE_HIGH       = 24.0f;
        public const float AUDIO_EFFECT_PHASER_STAGE_DEFAULT    = 2.0f;
 
        public const float AUDIO_EFFECT_PHASER_FB_LOW           = -100.0f;
        public const float AUDIO_EFFECT_PHASER_FB_HIGH          = 100.0f; 
        public const float AUDIO_EFFECT_PHASER_FB_DEFAULT       = .0f; 
 
        public const float AUDIO_EFFECT_DRY_LOW                 = .0f;
        public const float AUDIO_EFFECT_DRY_HIGH                = 255.0f;
        public const float AUDIO_EFFECT_DRY_DEFAULT             = 128.0f;
    }
}