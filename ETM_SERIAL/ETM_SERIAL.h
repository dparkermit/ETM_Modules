/*
  The ETM Serial Module supports two modes of operation
  
  Slave Transmit Mode - 
  This is the default mode
  In this mode, the pic is a slave and only responds to commands/data requests (usually from a GUI)
  These commands are mirrored and include a Checksum (someone please write a CRC module) to ensure that the sender knows if the command was successful
  The data bandwidth is not great because there is a large overhead associated with ensuring data integrity.

  Data Logging Mode -
  When this mode is enabled the pic no longer sends back responses to commands/data requests.  These commands will still be received and the pic can process them but no data will be sent back.
  Instead the output buffer is reserved for the pic to send out logging data.  
  Logging data does not contain much formatting data or checksum.
  Becuase it is unidirectional much more data can be sent out on the serial port.
  This mode is intended to be used to log multiple data objects every pulse (400Hz+)
  This mode would be entered (via a serial command) when data logging is request and exited (also via a serial command) when data logging is no longer required
  NOTE!!! In this mode, a GUI can not monitor the state of the board parameters because the output buffer is reserved for data logging.
*/


#ifndef __ETM_SERIAL_H
#define __ETM_SERIAL_H

// User Configuration Parameters
#define ETM_SERIAL_FCY              10000000
#define ETM_SERIAL_BAUDRATE         303000
#define ETM_SERIAL_UART_INT_PRI     4
#define USE_UART_1
// #define USE_UART_2

// END User Configuration Parameters




void InitializeETMSerial(void);
void DoSerialCommand(void);
void SendLoggingDataToUart(unsigned char* data_ptr, unsigned char num_bytes);
unsigned int ApplicationDefinedCommands(unsigned char command_select, unsigned char register_select, unsigned int input_data);

extern unsigned char data_logging_output_enabled;


#endif
