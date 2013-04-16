#ifndef __MCP23017_H
#define __MCP23017_H
/*
  This module is designed to interface a 16 bit microchip processor with 16 bit I/O Expander

  Only I2C Buses 1 and 2 are supported at this time.
*/

// DPARKER - Figure out how to remove the need to configure I2C ports separately

typedef struct {
  unsigned int pin_reset;
  unsigned int pin_int_a;
  unsigned int pin_int_b;
  unsigned char address;
  unsigned char i2c_port;
  unsigned char output_latch_a_in_ram;
  unsigned char output_latch_b_in_ram;
  unsigned char input_port_a_in_ram;
  unsigned char input_port_b_in_ram;
} MCP23017;

/*
  This module requires that the I2C bus be initialized externally before calling any of the read or write functions.
  The exact numbers will vary based on the frequency of your OSC and the Frequency that you want the bus to run at.
  Initialization should look something like this.
  Initialization will vary slightly between 24,30, and 33 series chips.
  See i2c.h for more information

*/




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


#endif
