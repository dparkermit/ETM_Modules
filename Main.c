#include <p30F6014a.h>
#include <libpic30.h>
#include "MCP23017.h"
#include "LTC2656.h"

_FOSC(ECIO & CSW_FSCM_OFF); 
_FWDT(WDT_ON & WDTPSA_1 & WDTPSB_2); 
_FBORPOR(PWRT_OFF & BORV_45 & PBOR_OFF & MCLR_EN);
_FBS(WR_PROTECT_BOOT_OFF & NO_BOOT_CODE & NO_BOOT_EEPROM & NO_BOOT_RAM);
_FSS(WR_PROT_SEC_OFF & NO_SEC_CODE & NO_SEC_EEPROM & NO_SEC_RAM);
_FGS(CODE_PROT_OFF);
_FICD(PGD);

MCP23017 U64_MCP23017;
LTC2656 U44_LTC2656;



#define MCP23017_DEFAULT_IOCON          0b00000000        // Default IOCON settings
#define MCP23017_U64_IODIRA_VALUE       0b11111000        // Port A.0 -> A.2 are Outputs, A.3-A.7 are inputs
#define MCP23017_U64_IODIRB_VALUE       0x00              // Port B is all Outputs
#define MCP23017_U64_LATA_INITIAL       0x00              // Outputs on Port A all start High (LED/OPTO NO LIGHTS)
#define MCP23017_U64_LATB_INITIAL       0x00              // Outputs on Port B all start High (LED/OPTO NO LIGHTS)
#define MCP23017_U64_IPOLA_VALUE        0x00              // Do Not Invert the inputs on Port A
#define MCP23017_U64_IPOLB_VALUE        0x00              // Do Not Invert the inputs on Port B

#define PULSE_GOBBLER_DELAY_FCY_30F6014A 4000000          // This corresponds to 250ns Pulse Gobbler Delay

unsigned int dac_value_array[8];
unsigned int ramp_value = 0;


int main(void) {
  unsigned char dan_temp;
  unsigned int i2c_test = 0;

  ADPCFG = 0xFFFF;
  
  U64_MCP23017.address = MCP23017_ADDRESS_0;
  U64_MCP23017.i2c_port = I2C_PORT;
  U64_MCP23017.pin_reset = _PIN_NOT_CONNECTED;
  U64_MCP23017.pin_int_a = _PIN_NOT_CONNECTED;
  U64_MCP23017.pin_int_b = _PIN_NOT_CONNECTED;
  U64_MCP23017.output_latch_a_in_ram = MCP23017_U64_LATA_INITIAL;
  U64_MCP23017.output_latch_b_in_ram = MCP23017_U64_LATB_INITIAL;
  U64_MCP23017.register_io_dir_a = MCP23017_U64_IODIRA_VALUE;
  U64_MCP23017.register_io_dir_b = MCP23017_U64_IODIRB_VALUE;
  U64_MCP23017.register_ipol_a = MCP23017_U64_IPOLA_VALUE;
  U64_MCP23017.register_ipol_a = MCP23017_U64_IPOLB_VALUE;
  U64_MCP23017.register_iocon = MCP23017_DEFAULT_IOCON;
  U64_MCP23017.i2c_configuration = MCP23017_I2C_CON_DEFAULT_1MHZ;
  U64_MCP23017.baud_rate = 1000000;
  U64_MCP23017.fcy_clk   = 10000000;
  U64_MCP23017.pulse_gobbler_delay_fcy = PULSE_GOBBLER_DELAY_FCY_30F6014A;

  dan_temp = SetupMCP23017(&U64_MCP23017);
  


  // --------------- Initialize U44 - LTC2656 ------------------------- //
  U44_LTC2656.pin_cable_select_not = _PIN_RD15;
  U44_LTC2656.pin_dac_clear_not = _PIN_NOT_CONNECTED;
  U44_LTC2656.pin_load_dac_not = _PIN_RD14;
  U44_LTC2656.pin_por_select = _PIN_NOT_CONNECTED;
  U44_LTC2656.por_select_value = 0;
  U44_LTC2656.spi_port = ETM_SPI_PORT_1;
  U44_LTC2656.spi_con1_value = LTC2656_SPI_CON_VALUE;
  U44_LTC2656.spi_con2_value = LTC2656_SPI_CON2_VALUE;
  U44_LTC2656.spi_stat_value = LTC2656_SPI_STAT_VALUE;
  U44_LTC2656.spi_bit_rate = 10000000;
  U44_LTC2656.fcy_clk      = 10000000;
  
  
  
  SetupLTC2656(&U44_LTC2656);
  
  
  
  while (1) {
    ramp_value += 0x10;

    dac_value_array[0] = ramp_value;
    dac_value_array[1] = ramp_value+1;
    dac_value_array[2] = ramp_value+2;
    dac_value_array[3] = ramp_value+3;
    dac_value_array[4] = ramp_value+4;
    dac_value_array[5] = ramp_value+5;
    dac_value_array[6] = ramp_value+6;
    dac_value_array[7] = ramp_value+7;
  

    //dan_temp = WriteLTC2656(&U44_LTC2656, LTC2656_WRITE_AND_UPDATE_DAC_C, ramp_value);

    dan_temp = WriteLTC2656AllDacChannels(&U44_LTC2656, dac_value_array);
    __delay32(1000000);
      i2c_test |= MCP23017WriteAndConfirmSingleByte(&U64_MCP23017, MCP23017_REGISTER_OLATA, 0x00);
    __delay32(1000000);
      i2c_test |= MCP23017WriteAndConfirmSingleByte(&U64_MCP23017, MCP23017_REGISTER_OLATA, 0xFF);
  }
}

