/*
  This is an extension of the built in SPI functions that provides more robust operation and easier use.

  This has been written work on devices with 1 or 2 SPI ports.
  If you are using a device with more than 2 SPI ports those will need to be added to the source code

  *****NOTE*****
  This module requires the use of a timer (so that timeouts can be detected without causing a WDT reset
  YOU MUST SELECT A TIMER AND CONFIGURE THE TIMEOUT

  I would suggest using the same timer to timeout all "ETM_MODULES" on the chip.
  Keep in mind that you can't use this timer for any other operations or modify it within an interrupt.

  Dan Parker
  2013_04_02
*/

/*
  DPARKER - This module needs to be validated

  DPARKER - add SPI configuration functions
*/

#ifndef __ETM_SPI_H
#define __ETM_SPI_H


unsigned long SendAndReceiveSPI(unsigned int data_word, unsigned char spi_port);
/*
  This writes a word to the SPI bus and reads back a word from the SPI bus
  It sets up and maganges the TMR specified below to look for timeouts and looks for faults.
  If the SPI operation is successful, the function will return 0x0000dddd (dddd being the 16 bits read from the SPI port) 
  If the SPI operation fails, the function will return 0x11110000
*/


// A 16 bit timer needs to be dedicated to SPI error checking.
#define SPI_TIMEOUT_TIMER    4      // Must be in range 1 -> 5

// Select a Timeout - This will be your Instruction Clock * SPI_TIMER_PRESCALE * SPI_TIMEOUT_CYCLES
#define SPI_TIMER_PRESCALE   1      // Must be 256, 64, 8 or 1,  (if not any of these will default to 1)
#define SPI_TIMEOUT_CYCLES   4000   // 16 bit Max

// #defines for the spi ports
#define SPI_PORT            0
#define SPI_PORT_1          1
#define SPI_PORT_2          2

/* 
   ----------- Example Code -----------
   temp = SendAndReceiveSPI(data_transmit_word, SPI_PORT_2);
   data_receive_word = temp & 0xFFFF;
   if (temp == 0x11110000) {
      // There was an error in the SPI process, execute corrective action
   }
*/

#endif
