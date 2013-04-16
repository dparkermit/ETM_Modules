#include <p30F6014a.h>
#include <libpic30.h>
#include "LTC2656.h"


_FOSC(ECIO & CSW_FSCM_OFF); 
_FWDT(WDT_ON & WDTPSA_1 & WDTPSB_2); 
_FBORPOR(PWRT_OFF & BORV_45 & PBOR_OFF & MCLR_EN);
_FBS(WR_PROTECT_BOOT_OFF & NO_BOOT_CODE & NO_BOOT_EEPROM & NO_BOOT_RAM);
_FSS(WR_PROT_SEC_OFF & NO_SEC_CODE & NO_SEC_EEPROM & NO_SEC_RAM);
_FGS(CODE_PROT_OFF);
_FICD(PGD);

LTC2656 U44_LTC2656;

int main(void) {
  
  U44_LTC2656.pin_cable_select_not = _PIN_RD15;
  U44_LTC2656.pin_dac_clear_not = _PIN_RB15;
  U44_LTC2656.pin_load_dac_not = _PIN_RD14;
  U44_LTC2656.pin_por_select = _PIN_NOT_CONNECTED;
  U44_LTC2656.por_select_value = 0;
  U44_LTC2656.spi_port = SPI_PORT_1;
  
  SetupLTC2656(&U44_LTC2656);


  while (1) {
    Nop();
  }
}

