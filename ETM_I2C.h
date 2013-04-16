/*
  This is an extension of the built in I2C functions that provides more robust operation and easier use.

  This has been written work on devices with 1 or 2 I2C ports.

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

  DPARKER - Add I2C configuration functions
*/


#ifndef __ETM_I2C_H
#define __ETM_I2C_H

unsigned int WaitForI2CBusIdle(unsigned char i2c_port);
/*
  This will wait for bus idle on the specified I2C Bus
  
  The result will be 0x0000 if everything works properly
  The result will be 0xFA00 if there is an I2C bus fault
*/

unsigned int GenerateI2CStart(unsigned char i2c_port);
/*
  This will generate a start condition on the specified I2C bus
  
  The result will be 0x0000 if everything works properly
  The result will be 0xFA00 if there is an I2C bus fault
*/

unsigned int GenerateI2CRestart(unsigned char i2c_port);
/*
  This will generate a re-start condition on the specified I2C bus
  
  The result will be 0x0000 if everything works properly
  The result will be 0xFA00 if there is an I2C bus fault
*/

unsigned int WriteByteI2C(unsigned char data, unsigned char i2c_port);
/*
  This will write a byte to the specified I2C bus
  
  The result will be 0x0000 if everything works properly
  The result will be 0xFA00 if there is an I2C bus fault
*/

unsigned int ReadByteI2C(unsigned char i2c_port);
/*
  This will read a byte from the specified I2C bus
  
  The result will be 0x00dd if everything works properly
  The result will be 0xFA00 if there is an I2C bus fault
*/


unsigned int GenerateI2CStop(unsigned char i2c_port);
/*
  This will generate a stop condition on the specified I2C bus
  
  The result will be 0x0000 if everything works properly
  The result will be 0xFA00 if there is an I2C bus fault
*/



// A 16 bit timer needs to be dedicated to SPI error checking.
#define I2C_TIMEOUT_TIMER    4      // Must be in range 1 -> 5

// Select a Timeout - This will be your Instruction Clock * SPI_TIMER_PRESCALE * SPI_TIMEOUT_CYCLES
#define I2C_TIMER_PRESCALE   1      // Must be 256, 64, 8 or 1,  (if not any of these will default to 1)
#define I2C_TIMEOUT_CYCLES   4000   // 16 bit Max


#define I2C_PORT                 0
#define I2C_PORT_1               1
#define I2C_PORT_2               2


#endif
