#include "gmain.h"

int32_t get_suitable_buffer_size(int32_t channel,int32_t samplerate)
{
    uint64_t ret = STD_BUFFER_SIZE*channel*samplerate/AUDIO_STANDARD_SAMPLERATE;
    return ret;
}

int is_little_endian_cpu()
{
    union
    {
        unsigned int a;
        unsigned char b;
    }c;
    c.a = 1;
    return (c.b == 1);
}
