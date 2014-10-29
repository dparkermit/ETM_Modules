#include <p30fxxxx.h>
#include "ETM_SERIAL.h"
#include "ETM_BUFFER_BYTE_64.h"


struct CommandStringStruct {
  unsigned char command_byte;
  unsigned char register_byte;
  unsigned char data_high_byte;
  unsigned char data_low_byte;
  unsigned char data_state;
};

#define COMMAND_BUFFER_EMPTY  0x00
#define COMMAND_BUFFER_FULL   0x02

#define COMMAND_LENGTH        9
#define SYNC_BYTE_1           0xF1
#define SYNC_BYTE_2           0xF2
#define SYNC_BYTE_3_RECEIVE   0xF3
#define SYNC_BYTE_3_SEND      0xF4

#define ETM_SERIAL_UART_BRG_VALUE   (unsigned int)(((ETM_SERIAL_FCY/ETM_SERIAL_BAUDRATE)/16)-1)

//#define ETM_SERIAL_UART_MODE_VALUE  (UART_EN & UART_IDLE_STOP & UART_RX_TX & UART_DIS_WAKE & UART_DIS_LOOPBACK & UART_DIS_ABAUD & UART_UXRX_IDLE_ONE & UART_BRGH_SIXTEEN & UART_NO_PAR_8BIT & UART_1STOPBIT)
//#define ETM_SERIAL_UART_STA_VALUE   (UART_INT_TX & UART_TX_ENABLE & UART_INT_RX_CHAR & UART_ADR_DETECT_DIS)

#define ETM_SERIAL_UART_MODE_VALUE  0b1010000000000000
#define ETM_SERIAL_UART_STA_VALUE   0b0000110001000000



#ifdef USE_UART_1
#define UART_STATS_BITS     U1STAbits
#define UART_TX_REG         U1TXREG
#define UART_RX_REG         U1RXREG
#define UART_BRG            U1BRG
#define UART_MODE           U1MODE
#define UART_STA            U1STA

#define UART_RX_IF         _U1RXIF
#define UART_TX_IF         _U1TXIF
#define UART_RX_IE         _U1RXIE
#define UART_TX_IE         _U1TXIE 
#define UART_RX_IP         _U1RXIP
#define UART_TX_IP         _U1TXIP


#define UART_RX_INTERRUPT  _U1RXInterrupt
#define UART_TX_INTERRUPT  _U1TXInterrupt



#else
#define UART_STATS_BITS     U2STAbits
#define UART_TX_REG         U2TXREG
#define UART_RX_REG         U2RXREG
#define UART_BRG            U2BRG
#define UART_MODE           U2MODE
#define UART_STA            U2STA

#define UART_RX_IF         _U2RXIF
#define UART_TX_IF         _U2TXIF
#define UART_RX_IE         _U2RXIE
#define UART_TX_IE         _U2TXIE 
#define UART_RX_IP         _U2RXIP
#define UART_TX_IP         _U2TXIP

#define UART_RX_INTERRUPT  _U2RXInterrupt
#define UART_TX_INTERRUPT  _U2TXInterrupt

#endif


/*
  Serial Commands

  A single command is stored in command_string
  If there is a valid command stored in command_string, then the command_string.data_state = COMMAND_BUFFER_FULL
  If there is NOT a volid command stored in command_string, then command_string.data_state = COMMAND_BUFFER_EMPTY
  
  When a byte is received on the UART it is transfered to the "serial_uart_input_buffer" by the UART receive interrupt - the input buffer is a circular buffer that is 64 Bytes deep
  (see buffer64.h for more infor on the buffer)
  
  When DoSerialCommand() is called
  If the command_string is empty, then the input buffer is searched for a valid command (the oldest valid command will be moved to command_string)

  If a command was found OR the command_string was already full, then the command is executed.
*/



void LookForCommand(void);
unsigned char CheckCRC(unsigned int crc);
unsigned int MakeCRC(unsigned char command_byte, unsigned char register_byte, unsigned int data_word);
void SendResponse(unsigned char command_byte, unsigned char register_byte, unsigned int data_word);


struct CommandStringStruct command_string;
BUFFERBYTE64 etm_serial_uart_input_buffer;
BUFFERBYTE64 etm_serial_uart_output_buffer;

unsigned char data_logging_output_enabled;

void InitializeETMSerial(void) {
  BufferByte64Initialize(&etm_serial_uart_input_buffer);
  BufferByte64Initialize(&etm_serial_uart_output_buffer);
  
  command_string.data_state = COMMAND_BUFFER_EMPTY;

  UART_RX_IP = ETM_SERIAL_UART_INT_PRI;
  UART_TX_IP = ETM_SERIAL_UART_INT_PRI;

  UART_RX_IF = 0;
  UART_TX_IF = 0;
  UART_RX_IE = 1;
  UART_TX_IE = 1;


  UART_BRG  = ETM_SERIAL_UART_BRG_VALUE;
  UART_MODE = ETM_SERIAL_UART_MODE_VALUE;
  UART_STA  = ETM_SERIAL_UART_STA_VALUE;
  
}


void DoSerialCommand(void) {
  /* 
     Look for a command and execute it.
  */
  
  unsigned int data_word;
  unsigned int return_data_word;
  
  if (command_string.data_state != COMMAND_BUFFER_FULL) {
    LookForCommand();
  }
  
  if (command_string.data_state == COMMAND_BUFFER_FULL) {
    //ExecuteCommand();    
    data_word = command_string.data_high_byte;
    data_word = data_word << 8;
    data_word = data_word + command_string.data_low_byte;
    return_data_word = ApplicationDefinedCommands(command_string.command_byte, command_string.register_byte, data_word);
    SendResponse(command_string.command_byte, command_string.register_byte, return_data_word);
    command_string.data_state = COMMAND_BUFFER_EMPTY;
  }
}


void LookForCommand(void) {
  unsigned char read_byte;
  unsigned int crc;
  /*
    If the command string is empty, then looks for a command in the input buffer
    
    To look for a command in the buffer.
    1) See if there are enough bytes in the buffer to contain a command.
    2) If there are look for command sync
       2b) If there are less bytes in the buffer than it takes to make a command, exit
    3) If command Syncs, check the checksum, increment the read_position as each byte is read
       3b) If command does not sync, increment the the read positon and return to step 1    
    4) If the checksum checks out, set the flag to indicate that the command buffer is full 
       4b) If the checksum fails, return to step 1     
  */
  
  while ((command_string.data_state == COMMAND_BUFFER_EMPTY) && (BufferByte64BytesInBuffer(&etm_serial_uart_input_buffer) >= COMMAND_LENGTH)) {
    // Look for a command
    read_byte = BufferByte64ReadByte(&etm_serial_uart_input_buffer);
    if (read_byte == SYNC_BYTE_1) {
      read_byte = BufferByte64ReadByte(&etm_serial_uart_input_buffer);
      if (read_byte == SYNC_BYTE_2) {
	read_byte = BufferByte64ReadByte(&etm_serial_uart_input_buffer);
	if (read_byte == SYNC_BYTE_3_RECEIVE) {
	  // All of the sync bytes matched, this should be a valid command
	  command_string.command_byte   = BufferByte64ReadByte(&etm_serial_uart_input_buffer);
	  command_string.data_high_byte = BufferByte64ReadByte(&etm_serial_uart_input_buffer);
	  command_string.data_low_byte  = BufferByte64ReadByte(&etm_serial_uart_input_buffer);
	  command_string.register_byte  = BufferByte64ReadByte(&etm_serial_uart_input_buffer);
	  crc                           = BufferByte64ReadByte(&etm_serial_uart_input_buffer);
	  crc                           = (crc << 8) + BufferByte64ReadByte(&etm_serial_uart_input_buffer);
	  if (CheckCRC(crc)) {
	    command_string.data_state = COMMAND_BUFFER_FULL;
	  }
	}
      }
    }
  }
}

void SendResponse(unsigned char command_byte, unsigned char register_byte, unsigned int data_word) {
  unsigned int crc;
  if (!data_logging_output_enabled) {
    crc = MakeCRC(command_byte, register_byte, data_word);
    BufferByte64WriteByte(&etm_serial_uart_output_buffer, SYNC_BYTE_1);
    BufferByte64WriteByte(&etm_serial_uart_output_buffer, SYNC_BYTE_2);
    BufferByte64WriteByte(&etm_serial_uart_output_buffer, SYNC_BYTE_3_SEND);
    BufferByte64WriteByte(&etm_serial_uart_output_buffer, command_byte);
    BufferByte64WriteByte(&etm_serial_uart_output_buffer, (data_word >> 8));
    BufferByte64WriteByte(&etm_serial_uart_output_buffer, (data_word & 0x00FF));
    BufferByte64WriteByte(&etm_serial_uart_output_buffer, register_byte);
    BufferByte64WriteByte(&etm_serial_uart_output_buffer, (crc >> 8));
    BufferByte64WriteByte(&etm_serial_uart_output_buffer, (crc & 0x00FF));
    
    if ((!UART_STATS_BITS.UTXBF) && (BufferByte64IsNotEmpty(&etm_serial_uart_output_buffer))) {
      /*
	There is at least one byte available for writing in the outputbuffer and the transmit buffer is not full.
	Move a byte from the output buffer into the transmit buffer
	All subsequent bytes will be moved from the output buffer to the transmit buffer by the UART TX Interrupt
      */
      UART_TX_REG = BufferByte64ReadByte(&etm_serial_uart_output_buffer);
    }
  }
}



void SendLoggingDataToUart(unsigned char* data_ptr, unsigned char num_bytes) {
  unsigned char n;
  if (data_logging_output_enabled) {
    BufferByte64WriteByte(&etm_serial_uart_output_buffer, 0xFE);
    BufferByte64WriteByte(&etm_serial_uart_output_buffer, 0x1C);
    for (n=0; n < num_bytes, n++) {
      while (n < num_bytes) {
	BufferByte64WriteByte(&etm_serial_uart_output_buffer, *(data_ptr + n));
      }
      
      if ((!UART_STATS_BITS.UTXBF) && (BufferByte64IsNotEmpty(&etm_serial_uart_output_buffer))) {
	/*
	  There is at least one byte available for writing in the outputbuffer and the transmit buffer is not full.
	  Move a byte from the output buffer into the transmit buffer
	  All subsequent bytes will be moved from the output buffer to the transmit buffer by the UART TX Interrupt
	*/
	UART_TX_REG = BufferByte64ReadByte(&etm_serial_uart_output_buffer);
      }
    }
  }
}


unsigned int MakeCRC(unsigned char command_byte, unsigned char register_byte, unsigned int data_word) {
  unsigned int crc;
  crc = SYNC_BYTE_1 + SYNC_BYTE_2 + SYNC_BYTE_3_SEND;
  crc += command_byte + register_byte;
  crc += (data_word >> 8);
  crc += (data_word & 0x00FF);
  
  return crc;
  // DPAKRER Make real CRC
}


unsigned char CheckCRC(unsigned int crc) {
  unsigned int crcCheck;
  // At the moment the CRC is just a checksum
  crcCheck = SYNC_BYTE_1 + SYNC_BYTE_2 + SYNC_BYTE_3_RECEIVE; 
  crcCheck += command_string.command_byte + command_string.register_byte;
  crcCheck += command_string.data_high_byte + command_string.data_low_byte;
  if (crcCheck == crc) {
    return 1;
  } else {
    return 0;
  }
  // DPARKER make Real CRC

}



void __attribute__((interrupt(__save__(CORCON,SR)),no_auto_psv)) UART_RX_INTERRUPT(void) {
  UART_RX_IF = 0;
  while (UART_STATS_BITS.URXDA) {
    BufferByte64WriteByte(&etm_serial_uart_input_buffer, UART_RX_REG);
  }
}



void __attribute__((interrupt(__save__(CORCON,SR)),no_auto_psv)) UART_TX_INTERRUPT(void) {
  UART_TX_IF = 0;
  while ((!UART_STATS_BITS.UTXBF) && (BufferByte64BytesInBuffer(&etm_serial_uart_output_buffer))) {
    /*
      There is at least one byte available for writing in the outputbuffer and the transmit buffer is not full.
      Move a byte from the output buffer into the transmit buffer
    */
    UART_TX_REG = BufferByte64ReadByte(&etm_serial_uart_output_buffer);
  }
}

