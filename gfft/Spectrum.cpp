#include <math.h>

#include "Spectrum.h"
#include "FFTimpl.h"
/*
#if 1 //(UGLY_IEEE754_FLOAT32_HACK :-)

static inline float todB_a(const float *x){
  return (float)((*(wxInt32 *)x)&0x7fffffff) * 7.17711438e-7f -764.6161886f;
}

#else*/

static inline float todB_a(const float *x){
   return (*(x)==0?-400.f:log(*(x)**(x))*4.34294480f);
}

//#endif


bool ComputeSpectrum(float * data, int width,
                     int windowSize,
                     double rate, float *output,
                     bool autocorrelation, int windowFunc)
{
   if (width < windowSize)
      return false;

   if (!data || !output)
      return true;

   float *processed = new float[windowSize];

   int i;
   for (i = 0; i < windowSize; i++)
      processed[i] = float(0.0);
   int half = windowSize / 2;

   float *in = new float[windowSize];
   float *out = new float[windowSize];
   float *out2 = new float[windowSize];

   int start = 0;
   int windows = 0;
   while (start + windowSize <= width) {
      for (i = 0; i < windowSize; i++)
         in[i] = data[start + i];

      WindowFunc(windowFunc, windowSize, in);

      if (autocorrelation) {
         // Take FFT
         FFT(windowSize, false, in, 0, out, out2);

         // Compute power
         for (i = 0; i < windowSize; i++)
            in[i] = (out[i] * out[i]) + (out2[i] * out2[i]);

         // Tolonen and Karjalainen recommend taking the cube root
         // of the power, instead of the square root

         for (i = 0; i < windowSize; i++)
            in[i] = pow(in[i], 1.0f / 3.0f);

         // Take FFT
         FFT(windowSize, false, in, 0, out, out2);

      }
      else
         PowerSpectrum(windowSize, in, out);

      // Take real part of result
      for (i = 0; i < half; i++)
        processed[i] += out[i];

      start += half;
      windows++;
   }

   if (autocorrelation) {

      // Peak Pruning as described by Tolonen and Karjalainen, 2000
      /*
       Combine most of the calculations in a single for loop.
       It should be safe, as indexes refer only to current and previous elements,
       that have already been clipped, etc...
      */
      for (i = 0; i < half; i++) {
        // Clip at zero, copy to temp array
        if (processed[i] < 0.0)
            processed[i] = float(0.0);
        out[i] = processed[i];
        // Subtract a time-doubled signal (linearly interp.) from the original
        // (clipped) signal
        if ((i % 2) == 0)
           processed[i] -= out[i / 2];
        else
           processed[i] -= ((out[i / 2] + out[i / 2 + 1]) / 2);

        // Clip at zero again
        if (processed[i] < 0.0)
            processed[i] = float(0.0);
      }

      // Reverse and scale
      for (i = 0; i < half; i++)
         in[i] = processed[i] / (windowSize / 4);
      for (i = 0; i < half; i++)
         processed[half - 1 - i] = in[i];
   } else {
      // Convert to decibels
      // But do it safely; -Inf is nobody's friend
      for (i = 0; i < half; i++){
         float temp=(processed[i] / windowSize / windows);
         if (temp > 0.0)
            processed[i] = 10*log10(temp);
         else
            processed[i] = 0;
      }
   }

   for(i=0;i<half;i++)
      output[i] = processed[i];
   delete[]in;
   delete[]out;
   delete[]out2;
   delete[]processed;

   return true;
}

// Indentation settings for Vim and Emacs and unique identifier for Arch, a
// version control system. Please do not modify past this point.
//
// Local Variables:
// c-basic-offset: 3
// indent-tabs-mode: nil
// End:
//
// vim: et sts=3 sw=3
// arch-tag: 29527ca5-00d0-442d-afde-d699aee29a31

