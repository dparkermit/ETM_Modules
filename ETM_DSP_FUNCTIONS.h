#ifndef __ETM_DSP_FUNCTIONS_H
#define __ETM_DSP_FUNCTIONS_H

unsigned int AverageADC128(unsigned int* array_to_avg);
/*
  This function takes a pointer to an array and of 12 bit adc values and averages.
  The result is a 16 bit number.
  Effectivly Sum[0->127] / 8
  If the array is not at least 128 unsigned integers long, bad things will happen
*/

unsigned int Scale13Q3(unsigned int value, unsigned int scale);

unsigned int MakeScale13Q3(unsigned int num, unsigned int den);

unsigned int Scale16Bit(unsigned int value, unsigned int num, unsigned int den);  // DPARKER - this needs more descriptive name

unsigned int RCFilterNTau(unsigned int previous_value, unsigned int reading, unsigned char FILTER_TAU_BITS);
/*
  This performs a fast "almost RC" filter where Tau is 2^FILTER_TAU_BITS samples.
  The max value of FILTER_TAU_BITS is 15

  If "previous_value" is zero, this is a special case where the output will be set to reading.  
  This is so that when this is first called (previous value = zero), the filtered value will not start at zero (which could take a long time to reach correct value)
*/


extern unsigned int saturation_scale13Q3_count;
extern unsigned int saturation_makescale13Q3_count;
extern unsigned int saturation_scale16_count;
#endif
