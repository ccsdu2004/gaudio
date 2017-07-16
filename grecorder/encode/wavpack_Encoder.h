#ifndef WAVPACK_ENCODER_H
#define WAVPACK_ENCODER_H
#include <stdio.h>
#include "lib_Encoder.h"
#include "wavpack.h"

typedef struct 
{
    uint32_t bytes_written,first_block_size;
    FILE *file;
    int error;
}write_id;

class wavpack_Encoder : public lib_Encoder
{
public:
    wavpack_Encoder();
    ~wavpack_Encoder();
public:
    int  open(const char* name,int channels,int rate,int bite);
    int  write(unsigned char* buffer,int len);
    void close();
private:
    WavpackContext* wpc;
    write_id wv;
    int32_t* sample_buffer;
};

#endif
//! ccsdu2004
