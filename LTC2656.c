#include <libpic30.h>
#include "LTC2656.h"


// DPARKER - I know that there are problems with the multichannel DAC command from monitoring the LINAC System.  This needs to be debugged


#define LTC2656_OLL_SELECT_CHIP                      0
#define LTC2656_OLL_CLEAR_OUTPUTS                    0
#define LTC2656_OLL_LOAD_DAC                         0

#define LTC2656_DELAY_DAC_CLEAR                     10
#define LTC2656_DELAY_SELECT_CHIP                   10



void SetupLTC2656(LTC2656* ptr_LTC2656) {
  // See h File For Documentation
  
  PinSetValue(ptr_LTC2656->pin_dac_clear, !LTC2656_OLL_CLEAR_OUTPUTS);
  PinSetValue(ptr_LTC2656->pin_load_dac, LTC2656_OLL_LOAD_DAC);
  PinSetValue(ptr_LTC2656->pin_cable_select, !LTC2656_OLL_SELECT_CHIP);
  PinSetValue(ptr_LTC2656->pin_por_select, ptr_LTC2656->por_select_value);
  
  PinSetTris(ptr_LTC2656->pin_cable_select, TRIS_DIGITAL_OUTPUT);
  PinSetTris(ptr_LTC2656->pin_dac_clear, TRIS_DIGITAL_INPUT);
  PinSetTris(ptr_LTC2656->pin_load_dac, TRIS_DIGITAL_OUTPUT);
  PinSetTris(ptr_LTC2656->pin_por_select, TRIS_DIGITAL_OUTPUT);  
}


void ClearOutputsLTC2656(LTC2656* ptr_LTC2656) {
  PinSetValue(ptr_LTC2656->pin_dac_clear, LTC2656_OLL_CLEAR_OUTPUTS);
  __delay32(LTC2656_DELAY_DAC_CLEAR);
  PinSetValue(ptr_LTC2656->pin_dac_clear, !LTC2656_OLL_CLEAR_OUTPUTS);
}



unsigned char WriteLTC2656(LTC2656* ptr_LTC2656, unsigned int command_word, unsigned int data_word) {
  // See h File For Documentation
  
  unsigned char spi_error;
  unsigned int command_word_readback;
  unsigned int data_word_readback;
  unsigned long temp;
  
  spi_error = 0;
  
  PinSetValue(ptr_LTC2656->pin_cable_select, LTC2656_OLL_SELECT_CHIP);


  temp = SendAndReceiveSPI(command_word, ptr_LTC2656->spi_port);
  if (temp == 0x11110000) {
    spi_error = 0b00000001;
  } 

  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(data_word, ptr_LTC2656->spi_port);
    if (temp == 0x11110000) {
      spi_error |= 0b00000010;
    } 
  }
 

  PinSetValue(ptr_LTC2656->pin_cable_select, !LTC2656_OLL_SELECT_CHIP);
  __delay32(LTC2656_DELAY_SELECT_CHIP);
  PinSetValue(ptr_LTC2656->pin_cable_select, LTC2656_OLL_SELECT_CHIP);


  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(LTC2656_CMD_NO_OPERATION, ptr_LTC2656->spi_port);
    command_word_readback = temp & 0xFFFF;
    if (temp == 0x11110000) {
      spi_error |= 0b00000100;
    } 
  }

  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(0, ptr_LTC2656->spi_port);
    data_word_readback = temp & 0xFFFF;
    if (temp == 0x11110000) {
      spi_error |= 0b00001000;
    } 
  }

  PinSetValue(ptr_LTC2656->pin_cable_select, !LTC2656_OLL_SELECT_CHIP);


  if (command_word_readback != command_word) {
    spi_error |= 0b00010000;
  }
  if (data_word_readback != data_word) {
    spi_error |= 0b00100000;
  }
  

  return spi_error;
}


unsigned char WriteLTC2656AllDacChannels(LTC2656* ptr_LTC2656, unsigned int *dac_array) {
  // See h File For Documentation
  
  unsigned char spi_error;
  unsigned int command_word_readback;
  unsigned int data_word_readback;
  unsigned long temp;
  unsigned char dac_number;

  spi_error = 0;
  dac_number = 0;
  
  while ((spi_error == 0) && (dac_number < 8)) {
    // Send out two 16 bit words on the SPI BUS

    PinSetValue(ptr_LTC2656->pin_cable_select, LTC2656_OLL_SELECT_CHIP);
    
    temp = SendAndReceiveSPI(LTC2656_CMD_WRITE_AND_UPDATE_N | dac_number, ptr_LTC2656->spi_port);
    command_word_readback = temp & 0xFFFF;
    if (temp == 0x11110000) {
      spi_error = 0b00000001;
    } 
    
    if (spi_error == 0) { 
      temp = SendAndReceiveSPI(dac_array[dac_number], ptr_LTC2656->spi_port);
      data_word_readback = temp & 0xFFFF;
      if (temp == 0x11110000) {
	spi_error |= 0b00000010;
      } 
    }

    // Check that the command/data returned match the command/data sent out on the previous cycle
    if (dac_number > 0) {
      if (command_word_readback != (LTC2656_CMD_WRITE_AND_UPDATE_N | (dac_number - 1))) {
	spi_error |= 0b00000100;
      }
      if (data_word_readback != dac_array[dac_number-1]) {
	spi_error |= 0b00001000;
      }   
    }
    
    PinSetValue(ptr_LTC2656->pin_cable_select, !LTC2656_OLL_SELECT_CHIP);
    
    dac_number++;
  }

  // Read back the error check from the last command/data string sent out
  PinSetValue(ptr_LTC2656->pin_cable_select, LTC2656_OLL_SELECT_CHIP);

  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(LTC2656_CMD_NO_OPERATION, ptr_LTC2656->spi_port);
    command_word_readback = temp & 0xFFFF;
    if (temp == 0x11110000) {
      spi_error |= 0b00000001;
    } 
  }

  if (spi_error == 0) { 
    temp = SendAndReceiveSPI(0, ptr_LTC2656->spi_port);
    data_word_readback = temp & 0xFFFF;
    if (temp == 0x11110000) {
      spi_error |= 0b00000010;
    } 
  }
  
  PinSetValue(ptr_LTC2656->pin_cable_select, !LTC2656_OLL_SELECT_CHIP);
  
  
  if (command_word_readback != (LTC2656_CMD_WRITE_AND_UPDATE_N | (dac_number - 1))) {
    spi_error |= 0b00010000;
  }
  if (data_word_readback != dac_array[dac_number-1]) {
    spi_error |= 0b00100000;
  }

  return spi_error;
}
