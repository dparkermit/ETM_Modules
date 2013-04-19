#ifndef __MCP23017_H
#define __MCP23017_H
/*
  This module is designed to interface a 16 bit microchip processor with 16 bit I/O Expander

  Only I2C Buses 1 and 2 are supported at this time.

  Dan Parker
  2013_4_18
*/

/*
  This module has been validated on A34760 (pic30F6014A)
*/

/* 
   DPARKER - need to add a reset function (and use this at pic resets).  A34760.h hardware does not support this
   DPARKER - need to add ability to read the Interrupt Pins (will require adding ETMReadPin function to ETM_IO_PORTS module)
*/

#include "ETM_I2C.h"


typedef struct {
  unsigned char address;
  unsigned char i2c_port;
  unsigned long pin_reset;
  unsigned long pin_int_a;
  unsigned long pin_int_b;
  unsigned char output_latch_a_in_ram;
  unsigned char output_latch_b_in_ram;
  unsigned char input_port_a_in_ram;
  unsigned char input_port_b_in_ram;
  unsigned char register_io_dir_a;
  unsigned char register_io_dir_b;
  unsigned char register_ipol_a;
  unsigned char register_ipol_b;
  unsigned char register_iocon;
  unsigned int  i2c_configuration;
  unsigned long baud_rate;
  unsigned long fcy_clk;
  unsigned long pulse_gobbler_delay_fcy;
} MCP23017;

/* 
   The MCP23017 has 21 8-bit control registers.
   10 Registers for PortA, 10 Registers for port B and 1 configuration Register Shared between the ports
   This module will access the ports with IOCON.BANK = 0
*/ 

/* 
   IOCON.BANK = 1 PORT REGISTER DESIGNATIONS 
   #define MCP23017_REGISTER_IODIRA                 0x00
   #define MCP23017_REGISTER_IPOLA                  0x01
   #define MCP23017_REGISTER_GPINTENA               0x02
   #define MCP23017_REGISTER_DEFVALA                0x03
   #define MCP23017_REGISTER_INTCONA                0x04
   #define MCP23017_REGISTER_IOCON                  0x05
   #define MCP23017_REGISTER_GPPUA                  0x06
   #define MCP23017_REGISTER_INTFA                  0x07
   #define MCP23017_REGISTER_INTCAPA                0x08
   #define MCP23017_REGISTER_GPIOA                  0x09
   #define MCP23017_REGISTER_OLATA                  0x0A
   
   #define MCP23017_REGISTER_IODIRB                 0x10
   #define MCP23017_REGISTER_IPOLB                  0x11
   #define MCP23017_REGISTER_GPINTENB               0x12
   #define MCP23017_REGISTER_DEFVALB                0x13
   #define MCP23017_REGISTER_INTCONB                0x14
   #define MCP23017_REGISTER_IOCON_ALT              0x15
   #define MCP23017_REGISTER_GPPUB                  0x16
   #define MCP23017_REGISTER_INTFB                  0x17
   #define MCP23017_REGISTER_INTCAPB                0x18
   #define MCP23017_REGISTER_GPIOB                  0x19
   #define MCP23017_REGISTER_OLATB                  0x1A
*/



/* 
   IOCON.BANK = 0 (DEFAULT) PORT REGISTER DESIGNATIONS 
*/

#define MCP23017_REGISTER_IODIRA                 0x00
#define MCP23017_REGISTER_IODIRB                 0x01
#define MCP23017_REGISTER_IPOLA                  0x02
#define MCP23017_REGISTER_IPOLB                  0x03
#define MCP23017_REGISTER_GPINTENA               0x04
#define MCP23017_REGISTER_GPINTENB               0x05
#define MCP23017_REGISTER_DEFVALA                0x06
#define MCP23017_REGISTER_DEFVALB                0x07
#define MCP23017_REGISTER_INTCONA                0x08
#define MCP23017_REGISTER_INTCONB                0x09
#define MCP23017_REGISTER_IOCON                  0x0A
#define MCP23017_REGISTER_IOCON_ALT              0x0B
#define MCP23017_REGISTER_GPPUA                  0x0C
#define MCP23017_REGISTER_GPPUB                  0x0D
#define MCP23017_REGISTER_INTFA                  0x0E
#define MCP23017_REGISTER_INTFB                  0x0F
#define MCP23017_REGISTER_INTCAPA                0x10
#define MCP23017_REGISTER_INTCAPB                0x11
#define MCP23017_REGISTER_GPIOA                  0x12
#define MCP23017_REGISTER_GPIOB                  0x13
#define MCP23017_REGISTER_OLATA                  0x14
#define MCP23017_REGISTER_OLATB                  0x15


#define MCP23017_READ_CONTROL_BIT          0b00000001
#define MCP23017_WRITE_CONTROL_BIT         0b00000000


#define MCP23017_ADDRESS_0                 0b01000000
#define MCP23017_ADDRESS_1                 0b01000010
#define MCP23017_ADDRESS_2                 0b01000100
#define MCP23017_ADDRESS_3                 0b01000110
#define MCP23017_ADDRESS_4                 0b01001000
#define MCP23017_ADDRESS_5                 0b01001010
#define MCP23017_ADDRESS_6                 0b01001100
#define MCP23017_ADDRESS_7                 0b01001110

unsigned char SetupMCP23017(MCP23017* ptr_MCP23017);
/*
  Function Arguments:
  *ptr_MCP23017 : This is pointer to the structure that defines a particular MCP23017 chip

  Function Return:
  Will return a startup error code.  
  If the MCP23017 started up properly it will return an error code between 0 and MCP23017_MAX_ERROR_COUNT which corresponds to the number of errors durring startup
  If the MCP23017 is unable to startup, it will return 0xFF.

  This function should be called when somewhere during the startup of the processor.
  This will configure the I2C port and I/O pins (if any are used).
*/

unsigned int MCP23017WriteSingleByte(MCP23017* ptr_MCP23017, unsigned char MCP23017_register, unsigned char register_data);
/*
  This function will write a single byte to the register specified by location
  
  This function will return 0x0000 if the operation completes sucessfully
  This function will return 0xFA00 if the operation fails
*/


unsigned int MCP23017ReadSingleByte(MCP23017* ptr_MCP23017, unsigned char MCP23017_register);
/*
  This function will read a single byte from register specified by location

  This function will return 0x00dd if the operation completes sucessfully
  This function will return 0xFAxx if the operation fails
*/


unsigned int MCP23017WriteAndConfirmSingleByte(MCP23017* ptr_MCP23017, unsigned char MCP23017_register, unsigned char register_data);
/*
  This function will write a single byte to the register specified by location
  It will then poll the device to confirm that the data stored in that register is the correct value.
  This function will return 0x0000 if the operation completes sucessfully and the data is correct
  This function will return 0xFA00 if the operation fails or there was an error with the data readback
*/

extern unsigned int MCP23017_read_error_count; // This global variable counts the number of times a MCP23017 fails to read a byte
extern unsigned int MCP23017_write_error_count; // This global variable counts the number of times a MCP23017 fails to write a byte
extern unsigned int MCP23017_write_verify_error_count; // This global variable counts the number of times a MCP23017 fails to write and verify a byte


#define MCP23017_I2C_CON_DEFAULT_100KHZ_400KHZ 0b1011000000100000
#define MCP23017_I2C_CON_DEFAULT_1MHZ          0b1011001000100000

#endif
