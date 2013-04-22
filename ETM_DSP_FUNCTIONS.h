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

extern unsigned int saturation_scale13Q3_count;
extern unsigned int saturation_makescale13Q3_count;
extern unsigned int saturation_scale16_count;
#endif
