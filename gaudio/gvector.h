#ifndef G_VECTOR_H
#define G_VECTOR_H
#include "gaudio.h"

static inline void gcrossproduct(const float* vec1,const float* vec2,float* out)
{
    out[0] = vec1[1]*vec2[2] - vec1[2]*vec2[1];
    out[1] = vec1[2]*vec2[0] - vec1[0]*vec2[2];
    out[2] = vec1[0]*vec2[1] - vec1[1]*vec2[0];
}

static inline float gdotproduct(const float* vec1,const float* vec2)
{
    return vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2];
}

static inline void gnormalize(float* vec)
{
    gfloat lengthsqr = gdotproduct(vec,vec);
    if(lengthsqr > 0.0f)
    {
        gfloat inv = 1.0f/sqrtf(lengthsqr);
        vec[0] *= inv;
        vec[1] *= inv;
        vec[2] *= inv;
    }
}

static inline void gmatrixvector(float* vector,float w,float(*matrix)[4])
{
    float temp[4] = {vector[0],vector[1],vector[2],w};
    vector[0] = temp[0]*matrix[0][0] + temp[1]*matrix[1][0] + temp[2]*matrix[2][0] + temp[3]*matrix[3][0];
    vector[1] = temp[0]*matrix[0][1] + temp[1]*matrix[1][1] + temp[2]*matrix[2][1] + temp[3]*matrix[3][1];
    vector[2] = temp[0]*matrix[0][2] + temp[1]*matrix[1][2] + temp[2]*matrix[2][2] + temp[3]*matrix[3][2];
}

#endif
//! by duwenhua2013
