#include <p30F6014a.h>
#include <libpic30.h>
#include "ETM_BUFFER_BYTE_64.h"
#include "ETM_IO_PORTS_V3.h"

_FOSC(ECIO & CSW_FSCM_OFF); 
_FWDT(WDT_ON & WDTPSA_1 & WDTPSB_2); 
_FBORPOR(PWRT_OFF & BORV_45 & PBOR_OFF & MCLR_EN);
_FBS(WR_PROTECT_BOOT_OFF & NO_BOOT_CODE & NO_BOOT_EEPROM & NO_BOOT_RAM);
_FSS(WR_PROT_SEC_OFF & NO_SEC_CODE & NO_SEC_EEPROM & NO_SEC_RAM);
_FGS(CODE_PROT_OFF);
_FICD(PGD);



int main(void) {
  Nop();
  Nop();
  Nop();
  Nop();
  Nop();
  ETMSetPin(_PIN_RC13);
  Nop();
  Nop();
  Nop();
  Nop();  
  ETMClearPin(_PIN_RC13);

  while (1) {
    Nop();
  }
}

