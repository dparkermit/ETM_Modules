#include "MCP23017.h"
#include "ETM_I2C.h"
#include "ETM_IO_PORTS.h"


unsigned int MCP23017_read_error_count = 0;
unsigned int MCP23017_write_error_count = 0;
unsigned int MCP23017_write_verify_error_count = 0;

#define MCP23017_MAX_ERROR_COUNT  10

unsigned char SetupMCP23017(MCP23017* ptr_MCP23017) {
  unsigned int error_count;
  
  ConfigureI2C(ptr_MCP23017->i2c_port, ptr_MCP23017->i2c_configuration, ptr_MCP23017->baud_rate, ptr_MCP23017->fcy_clk, ptr_MCP23017->pulse_gobbler_delay_fcy);

  error_count = 0;
  while (error_count < MCP23017_MAX_ERROR_COUNT && (MCP23017WriteAndConfirmSingleByte(ptr_MCP23017, MCP23017_REGISTER_IOCON, ptr_MCP23017->register_iocon) != 0)) {  
    error_count++;
  }
  while (error_count < MCP23017_MAX_ERROR_COUNT && (MCP23017WriteAndConfirmSingleByte(ptr_MCP23017, MCP23017_REGISTER_IOCON, ptr_MCP23017->register_iocon) != 0)) {  
    error_count++;
  }
  while (error_count < MCP23017_MAX_ERROR_COUNT && (MCP23017WriteAndConfirmSingleByte(ptr_MCP23017, MCP23017_REGISTER_OLATA, ptr_MCP23017->output_latch_a_in_ram) != 0)) {  
    error_count++;
  }
  while (error_count < MCP23017_MAX_ERROR_COUNT && (MCP23017WriteAndConfirmSingleByte(ptr_MCP23017, MCP23017_REGISTER_OLATB, ptr_MCP23017->output_latch_b_in_ram) != 0)) {  
    error_count++;
  }
  while (error_count < MCP23017_MAX_ERROR_COUNT && (MCP23017WriteAndConfirmSingleByte(ptr_MCP23017, MCP23017_REGISTER_IODIRA, ptr_MCP23017->register_io_dir_a) != 0)) {  
    error_count++;
  }
  while (error_count < MCP23017_MAX_ERROR_COUNT && (MCP23017WriteAndConfirmSingleByte(ptr_MCP23017, MCP23017_REGISTER_IODIRB, ptr_MCP23017->register_io_dir_b) != 0)) {  
    error_count++;
  }
  while (error_count < MCP23017_MAX_ERROR_COUNT && (MCP23017WriteAndConfirmSingleByte(ptr_MCP23017, MCP23017_REGISTER_IODIRA, ptr_MCP23017->register_ipol_a) != 0)) {  
    error_count++;
  }
  while (error_count < MCP23017_MAX_ERROR_COUNT && (MCP23017WriteAndConfirmSingleByte(ptr_MCP23017, MCP23017_REGISTER_IODIRA, ptr_MCP23017->register_ipol_b) != 0)) {  
    error_count++;
  }
  
  if (error_count < MCP23017_MAX_ERROR_COUNT) {
    // the MCP23017 is properly initialized
    return error_count;
  } else {
    return 0xFF;
  }
} 

unsigned int MCP23017WriteSingleByte(MCP23017* ptr_MCP23017, unsigned char MCP23017_register, unsigned char register_data) {
  unsigned int error_check;
  
  error_check = WaitForI2CBusIdle(ptr_MCP23017->i2c_port);                                                    // Wait for I2C Bus idle

  error_check |= GenerateI2CStart(ptr_MCP23017->i2c_port);                                                    // Generate start condition 

  error_check |= WriteByteI2C((ptr_MCP23017->address | MCP23017_WRITE_CONTROL_BIT), ptr_MCP23017->i2c_port);  // Write the address to the bus in WRITE MODE
  
  error_check |= WriteByteI2C(MCP23017_register, ptr_MCP23017->i2c_port);                                     // Write the register location to the bus

  error_check |= WriteByteI2C(register_data, ptr_MCP23017->i2c_port);                                         // Write the data to the bus
  
  error_check |= GenerateI2CStop(ptr_MCP23017->i2c_port);                                                     // Generate Stop Condition
  
  if (error_check != 0) {
    MCP23017_write_error_count++;
  }
  return error_check;
}

unsigned int MCP23017ReadSingleByte(MCP23017* ptr_MCP23017, unsigned char MCP23017_register) {
  unsigned int error_check;
  
  error_check = WaitForI2CBusIdle(ptr_MCP23017->i2c_port);                                                    // Wait for I2C Bus idle

  error_check |= GenerateI2CStart(ptr_MCP23017->i2c_port);                                                    // Generate start condition 

  error_check |= WriteByteI2C((ptr_MCP23017->address | MCP23017_WRITE_CONTROL_BIT), ptr_MCP23017->i2c_port);  // Write the address to the bus in WRITE MODE

  error_check |= WriteByteI2C(MCP23017_register, ptr_MCP23017->i2c_port);                                     // Write the register location to the bus

  error_check |= GenerateI2CRestart(ptr_MCP23017->i2c_port);                                                  // Generate re-start condition 

  error_check |= WriteByteI2C((ptr_MCP23017->address | MCP23017_READ_CONTROL_BIT), ptr_MCP23017->i2c_port);   // Write the address to the bus in READ MODE

  error_check |= ReadByteI2C(ptr_MCP23017->i2c_port);                                                         // Read a byte from the bus
  
  error_check |= GenerateI2CStop(ptr_MCP23017->i2c_port);                                                     // Generate Stop Condition
  
  if ((error_check & 0xFF00) != 0) {
    MCP23017_read_error_count++;
  }
  return error_check;
}


unsigned int MCP23017WriteAndConfirmSingleByte(MCP23017* ptr_MCP23017, unsigned char MCP23017_register, unsigned char register_data) {
  unsigned int error_check;
  error_check = MCP23017WriteSingleByte(ptr_MCP23017, MCP23017_register, register_data);
  if (error_check == 0x0000) {
    error_check = MCP23017ReadSingleByte(ptr_MCP23017, MCP23017_register);
  }
  if (error_check == (0x00FF & register_data)) {
    // The readback data matches what was sent
    return 0x0000;
  } else {
    MCP23017_write_verify_error_count++;
    // There was an error on the bus or the data readback did not match the data sent
    return 0xFA00;
  }
}


