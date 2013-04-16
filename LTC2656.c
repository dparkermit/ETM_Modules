#include <libpic30.h>
#include "LTC2656.h"


// DPARKER - I know that there are problems with the multichannel DAC command from monitoring the LINAC System.  This needs to be debugged
// DPARKER - On recent testing in lab it appears to work (300us write time or so, I don't know what the problem was before)
// DPARKER - Consider trying to figure out how to use the LDAC pin.  Is it even possible with our methodology

#define LTC2656_OLL_SELECT_CHIP                      0
#define LTC2656_OLL_CLEAR_OUTPUTS                    0
#define LTC2656_OLL_LOAD_DAC                         0

#define LTC2656_DELAY_DAC_CLEAR                     10
#define LTC2656_DELAY_SELECT_CHIP                   10



void SetupLTC2656(LTC2656* ptr_LTC2656) {
  // See h File For Documentation

  ETMSetPin(ptr_LTC2656->pin_dac_clear_not);
  ETMClearPin(ptr_LTC2656->pin_load_dac_not);
  ETMSetPin(ptr_LTC2656->pin_cable_select_not);
  if (ptr_LTC2656->por_select_value) {
    ETMSetPin(ptr_LTC2656->pin_por_select);
  } else {
    ETMClearPin(ptr_LTC2656->pin_por_select);
  }
  
  ETMPinTrisOutput(ptr_LTC2656->pin_cable_select_not);
  ETMPinTrisOutput(ptr_LTC2656->pin_dac_clear_not);
  ETMPinTrisOutput(ptr_LTC2656->pin_load_dac_not);
  ETMPinTrisOutput(ptr_LTC2656->pin_por_select);
}


void ClearOutputsLTC2656(LTC2656* ptr_LTC2656) {
  ETMClearPin(ptr_LTC2656->pin_dac_clear_not);
  __delay32(LTC2656_DELAY_DAC_CLEAR);
  ETMSetPin(ptr_LTC2656->pin_dac_clear_not);
}



unsigned char WriteLTC2656(LTC2656* ptr_LTC2656, unsigned int command_word, unsigned int data_word) {
  // See h File For Documentation
  
  unsigned char spi_error;
  unsigned int command_word_readback;
  unsigned int data_word_readback;
  unsigned long temp;
  
  spi_error = 0;
  
  ETMClearPin(ptr_LTC2656->pin_cable_select_not);

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
 

  ETMSetPin(ptr_LTC2656->pin_cable_select_not);
  __delay32(LTC2656_DELAY_SELECT_CHIP);
  ETMClearPin(ptr_LTC2656->pin_cable_select_not);

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

  ETMSetPin(ptr_LTC2656->pin_cable_select_not);


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

    ETMClearPin(ptr_LTC2656->pin_cable_select_not);
    
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
    
    ETMSetPin(ptr_LTC2656->pin_cable_select_not);
	
    dac_number++;
  }

  // Read back the error check from the last command/data string sent out
  ETMClearPin(ptr_LTC2656->pin_cable_select_not);

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
  
  ETMSetPin(ptr_LTC2656->pin_cable_select_not);
  
  if (command_word_readback != (LTC2656_CMD_WRITE_AND_UPDATE_N | (dac_number - 1))) {
    spi_error |= 0b00010000;
  }
  if (data_word_readback != dac_array[dac_number-1]) {
    spi_error |= 0b00100000;
  }

  return spi_error;
}
