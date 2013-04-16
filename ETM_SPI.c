#include "ETM_SPI.h"

#if defined(__dsPIC30F__)
#include <p30fxxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#elif defined(__PIC24H__)
#include <p24Hxxxx.h>
#endif




#if SPI_TIMER_PRESCALE == 256
#define SPI_TIMER_PRESCALE_BITS 0b11

#elif SPI_TIMER_PRESCALE == 64
#define SPI_TIMER_PRESCALE_BITS 0b10

#elif SPI_TIMER_PRESCALE == 8
#define SPI_TIMER_PRESCALE_BITS 0b01

#else
#define SPI_TIMER_PRESCALE_BITS 0b00

#endif




#if SPI_TIMEOUT_TIMER == 5
// Timer 5 was selected as the timer for SPI Timeout Operation
#define SPI_TIMER_REGISTER   TMR5
#define SPI_TIMER_CONFIG     T5CON
#define SPI_TIMER_CON_BITS   T5CONbits
#define SPI_TIMER_INT_FLAG   _T5IF
#define SPI_TIMER_INT_EN     _T5IE
#define SPI_TIMER_INT_PRI    _T5IP
#define SPI_TIMER_PERIOD     PR5

#elif SPI_TIMEOUT_TIMER == 4
// Timer 4 was selected as the timer for SPI Timeout Operation
#define SPI_TIMER_REGISTER   TMR4
#define SPI_TIMER_CONFIG     T4CON
#define SPI_TIMER_CON_BITS   T4CONbits
#define SPI_TIMER_INT_FLAG   _T4IF
#define SPI_TIMER_INT_EN     _T4IE
#define SPI_TIMER_INT_PRI    _T4IP
#define SPI_TIMER_PERIOD     PR4

#elif SPI_TIMEOUT_TIMER == 3
// Timer 3 was selected as the timer for SPI Timeout Operation
#define SPI_TIMER_REGISTER   TMR3
#define SPI_TIMER_CONFIG     T3CON
#define SPI_TIMER_CON_BITS   T3CONbits
#define SPI_TIMER_INT_FLAG   _T3IF
#define SPI_TIMER_INT_EN     _T3IE
#define SPI_TIMER_INT_PRI    _T3IP
#define SPI_TIMER_PERIOD     PR3

#elif SPI_TIMEOUT_TIMER == 2
// Timer 2 was selected as the timer for SPI Timeout Operation
#define SPI_TIMER_REGISTER   TMR2
#define SPI_TIMER_CONFIG     T2CON
#define SPI_TIMER_CON_BITS   T2CONbits
#define SPI_TIMER_INT_FLAG   _T2IF
#define SPI_TIMER_INT_EN     _T2IE
#define SPI_TIMER_INT_PRI    _T2IP
#define SPI_TIMER_PERIOD     PR2

#elif SPI_TIMEOUT_TIMER == 1
// Timer 1 was selected as the timer for SPI Timeout Operation
#define SPI_TIMER_REGISTER   TMR1
#define SPI_TIMER_CONFIG     T1CON
#define SPI_TIMER_CON_BITS   T1CONbits
#define SPI_TIMER_INT_FLAG   _T1IF
#define SPI_TIMER_INT_EN     _T1IE
#define SPI_TIMER_INT_PRI    _T1IP
#define SPI_TIMER_PERIOD     PR1

#endif



#define SPI_BUS_ACTIVE      0
#define SPI_DATA_SENT       1
#define SPI_DATA_RECEIVED   2 
#define SPI_BUS_TIMEOUT     3
#define SPI_BUS_OVERFLOW    4


#define SPI_TxCON_VALUE 0x0000




unsigned long SendAndReceiveSPI(unsigned int data_word, unsigned char spi_port) {
  unsigned char spi_bus_status;
  unsigned int return_data;

  spi_bus_status = SPI_BUS_ACTIVE;
  SPI_TIMER_REGISTER = 0;
  SPI_TIMER_PERIOD = SPI_TIMEOUT_CYCLES;
  SPI_TIMER_INT_FLAG = 0;
  SPI_TIMER_CONFIG = SPI_TxCON_VALUE;
  SPI_TIMER_CON_BITS.TON = 1;
  SPI_TIMER_CON_BITS.TCKPS = SPI_TIMER_PRESCALE_BITS;
  

#if defined(_SPIIF)
  if ((spi_port == 0) || (spi_port == 1)) {
    _SPIIF = 0;
    SPIBUF = data_word;
    while (spi_bus_status == SPI_BUS_ACTIVE) {
      if (SPI_TIMER_INT_FLAG) {
	// There was a timeout of the data read, this is effectively a fault
	spi_bus_status = SPI_BUS_TIMEOUT;
      }
      if (SPISTATbits.SPIROV) {
	// There was a data overflow in the buffer, this is a fault
	spi_bus_status = SPI_BUS_OVERFLOW;
      }
      if (!SPISTATbits.SPITBF) {
	spi_bus_status = SPI_DATA_SENT;
      }
    }
    
    while(spi_bus_status == SPI_DATA_SENT) {
      if (SPI_TIMER_INT_FLAG) {
	// There was a timeout of the data read, this is effectively a fault
	spi_bus_status = SPI_BUS_TIMEOUT;
      }
      if (SPISTATbits.SPIROV) {
	// There was a data overflow in the buffer, this is a fault
	spi_bus_status = SPI_BUS_OVERFLOW;
      }
      if (_SPIIF) {
	// Data  been received in the buffer, read the data from the return buffer
	spi_bus_status = SPI_DATA_RECEIVED;
      }    
    }
    return_data = SPIBUF;
  }
#endif


#if defined(_SPI1IF)
  if (spi_port == 1) {
    _SPI1IF = 0;
    SPI1BUF = data_word;
    while (spi_bus_status == SPI_BUS_ACTIVE) {
      if (SPI_TIMER_INT_FLAG) {
	// There was a timeout of the data read, this is effectively a fault
	spi_bus_status = SPI_BUS_TIMEOUT;
      }
      if (SPI1STATbits.SPIROV) {
	// There was a data overflow in the buffer, this is a fault
	spi_bus_status = SPI_BUS_OVERFLOW;
      }
      if (!SPI1STATbits.SPITBF) {
	spi_bus_status = SPI_DATA_SENT;
      }
    }
    
    while(spi_bus_status == SPI_DATA_SENT) {
      if (SPI_TIMER_INT_FLAG) {
	// There was a timeout of the data read, this is effectively a fault
	spi_bus_status = SPI_BUS_TIMEOUT;
      }
      if (SPI1STATbits.SPIROV) {
	// There was a data overflow in the buffer, this is a fault
	spi_bus_status = SPI_BUS_OVERFLOW;
      }
      //if (SPI1STATbits.SPIRBF) {
      // A byte has been received in the buffer, read the data from the return buffer
      if (_SPI1IF) {
	// Data  been received in the buffer, read the data from the return buffer
	spi_bus_status = SPI_DATA_RECEIVED;
      }    
    }
    return_data = SPI1BUF;
  }
#endif



#if defined(_SPI2IF)
  if (spi_port == 2) {

    _SPI2IF = 0;
    SPI2BUF = data_word;
    while (spi_bus_status == SPI_BUS_ACTIVE) {
      if (SPI_TIMER_INT_FLAG) {
	// There was a timeout of the data read, this is effectively a fault
	spi_bus_status = SPI_BUS_TIMEOUT;
      }
      if (SPI2STATbits.SPIROV) {
	// There was a data overflow in the buffer, this is a fault
	spi_bus_status = SPI_BUS_OVERFLOW;
      }
      if (!SPI2STATbits.SPITBF) {
	spi_bus_status = SPI_DATA_SENT;  // Data has been moved into the shift register for sending out
      }
    }
    
    while(spi_bus_status == SPI_DATA_SENT) {
      if (SPI_TIMER_INT_FLAG) {
	// There was a timeout of the data read, this is effectively a fault
	spi_bus_status = SPI_BUS_TIMEOUT;
      }
      if (SPI2STATbits.SPIROV) {
	// There was a data overflow in the buffer, this is a fault
	spi_bus_status = SPI_BUS_OVERFLOW;
      }
      if (_SPI2IF) {
	// Data  been received in the buffer, read the data from the return buffer
	spi_bus_status = SPI_DATA_RECEIVED;
      }    
    }
    return_data = SPI2BUF;
  }
#endif
  
  if (spi_bus_status == SPI_DATA_RECEIVED) {
    return (0x0000FFFF & return_data);
  } else {
    return (0x11110000);
  }
}
