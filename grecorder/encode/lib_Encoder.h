#ifndef LIB_ENCODER_H
#define LIB_ENCODER_H

class lib_Encoder
{
public:
    lib_Encoder(){}
    virtual ~lib_Encoder(){}
public:
    virtual int  open(const char* name,int channels,int rate,int bite) = 0;
    virtual int  write(unsigned char* buffer,int len) = 0;
    virtual void close() = 0;
private:
};

#endif
//! ccsdu2004
