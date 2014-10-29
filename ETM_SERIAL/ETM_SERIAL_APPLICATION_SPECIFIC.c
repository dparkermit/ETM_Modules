/*
  This file is where you defines how a particualar system response to serial commands.
*/

#include "ETM_SERIAL.h"


// Command List
#define CMD_READ_RAM_VALUE                              0x20
#define CMD_READ_TRIGGER_WIDTH                          0x30
#define CMD_SET_TARGET_POSITION                         0x30
#define CMD_MOVE_CLOCKWISE                              0x32
#define CMD_MOVE_COUNTER_CLOCKWISE                      0x34
#define CMD_SET_HOME_POSITION                           0x36
#define CMD_READ_EEPROM_REGISTER                        0x40
#define CMD_WRITE_EEPROM_REGISTER                       0x42
#define CMD_OVERCURRENT_SHUTDOWN_TEST                   0xE0
#define CMD_READ_AFC_ERROR_DATA_HISTORY                 0x50
#define CMD_READ_MEM_LOCATION                           0x54
#define CMD_SET_ERROR_OFFSET                            0x60
#define CMD_DO_POSITION_AUTO_ZERO                       0x70
#define CMD_AFC_NOT_PULSING_GO_HOME                     0x72
#define CMD_DATA_LOGGING                                0x56



// RAM Locations
#define RAM_READ_STATE                                  0x01
#define RAM_READ_VERSION                                0x02

#define RAM_READ_CURRENT_POSITION                       0x10
#define RAM_READ_TARGET_POSITION                        0x12
#define RAM_READ_HOME_POSITION                          0x14
#define RAM_READ_MAX_POSITION                           0x16
#define RAM_READ_MIN_POSITION                           0x18

#define RAM_READ_ADCBUF0                                0x30
#define RAM_READ_ADCBUF1                                0x31
#define RAM_READ_ADC_MOTOR_CURRENT_A                    0x32
#define RAM_READ_ADC_MOTOR_CURRENT_B                    0x33
#define RAM_READ_ADC_PARAMETER_INPUT                    0x3B
#define RAM_READ_ANALOG_INPUT                           0x3C
#define RAM_READ_ANALOG_OUTPUT                          0x3D

#define RAM_READ_SIGMA_DATA                             0x40
#define RAM_READ_DELTA_DATA                             0x41
#define RAM_READ_FREQUENCY_ERROR_FILTERED               0x42
#define RAM_READ_FREQUENCY_ERROR_OFFSET                 0x43
#define RAM_READ_NUMBER_PULSES_ON                       0x44
#define RAM_READ_TIME_NOT_PULSING                       0x45
#define RAM_READ_AFC_DISTANCE_FROM_HOME_AT_STOP         0x46

#define RAM_READ_PRF                                    0x50



unsigned int ReadFromRam(unsigned int ram_location);


unsigned int ApplicationDefinedCommands(unsigned char command_select, unsigned char register_select, unsigned int input_data) {
  unsigned int *ram_pointer;
  unsigned int return_data = 0;
 
  switch (command_select) 
    {
      
    case CMD_READ_RAM_VALUE:
      return_data = ReadFromRam(register_select);
      break;
      
    case CMD_SET_TARGET_POSITION:
      if ((control_state != STATE_AFC_PULSING) && (control_state != STATE_AFC_NOT_PULSING)) {
	SetMotorTarget(POSITION_TYPE_ABSOLUTE_POSITION, input_data); 
      }
      break;
      
    case CMD_MOVE_CLOCKWISE:
      if (control_state == STATE_MANUAL_MODE) {
	SetMotorTarget(POSITION_TYPE_RELATIVE_CLOCKWISE, input_data);
      }
      break;
      
    case CMD_MOVE_COUNTER_CLOCKWISE:
      if (control_state == STATE_MANUAL_MODE) {
	SetMotorTarget(POSITION_TYPE_RELATIVE_COUNTER_CLOCKWISE, input_data);
      }
      break;
      
    case CMD_DO_POSITION_AUTO_ZERO:
      auto_zero_requested = 1;
      break;

    case CMD_READ_EEPROM_REGISTER:
      return_data = M24LC64FReadWord(&U23_M24LC64F, command_string.register_byte);
      break;
    
    case CMD_WRITE_EEPROM_REGISTER:
      // DPARKER this is not implimented for now
      break;

    case CMD_READ_AFC_ERROR_DATA_HISTORY:
      return_data = afc_data.frequency_error_history[command_string.register_byte];
      break;
      
    case CMD_READ_MEM_LOCATION:
      if (input_data >= 0x09FF) {
	input_data = 0x09FF;
      }
      input_data &= 0b1111111111111110;
      ram_pointer = input_data;
      // NOTE!!!! This will generate a complier warning (as it should for direct memory access)
      return_data = *ram_pointer;
      break;

    case CMD_SET_ERROR_OFFSET:
      if (command_string.register_byte == 0) {
	afc_data.frequency_error_offset = input_data;
      } else if (command_string.register_byte == 1) {
	afc_data.frequency_error_offset = 0;
	afc_data.frequency_error_offset -= input_data;
      }
      M24LC64FWriteWord(&U23_M24LC64F, EEPROM_REGISTER_ERROR_OFFSET, afc_data.frequency_error_offset);
      break;

    case CMD_SET_HOME_POSITION:
      afc_motor.home_position = input_data;
      M24LC64FWriteWord(&U23_M24LC64F, EEPROM_REGISTER_HOME_POSITION, afc_motor.home_position);
      break;

    case CMD_DATA_LOGGING:
      if (command_string.register_byte == 1) {
	data_logging_to_uart = 1;
      } else {
      	data_logging_to_uart = 0;
      }
      break;

    case CMD_OVERCURRENT_SHUTDOWN_TEST:
      /*
	// DO NOTHING
	IOCON1 = 0b0000001100000000;
	IOCON2 = 0b0000001100000000;
	IOCON3 = 0b0000001100000000;
	IOCON4 = 0b0000001100000000;
      */
      break;

    case CMD_AFC_NOT_PULSING_GO_HOME:
      if (control_state == STATE_AFC_NOT_PULSING) {
	afc_data.time_off_100ms_units = 0xFF00;
      }
      break;
      
    }
}



unsigned int ReadFromRam(unsigned int ram_location) {
  unsigned int data_return;
  
  switch (ram_location) 
    {
      
    case RAM_READ_STATE:
      data_return = control_state;
      break;
      
    case RAM_READ_VERSION:
      data_return = A34405_SOFTWARE_VERSION;
      break;
      
    case RAM_READ_CURRENT_POSITION:
      data_return = afc_motor.current_position;
      break;
      
    case RAM_READ_TARGET_POSITION:
      data_return = afc_motor.target_position;
      break;

    case RAM_READ_HOME_POSITION:
      data_return = afc_motor.home_position;
      break;

    case RAM_READ_MAX_POSITION:
      data_return = afc_motor.max_position;
      break;

    case RAM_READ_MIN_POSITION:
      data_return = afc_motor.min_position;
      break;

    case RAM_READ_ADCBUF0:
      data_return = ADCBUF0;
      break;

    case RAM_READ_ADCBUF1:
      data_return = ADCBUF1;
      break;
      
    case RAM_READ_ADC_MOTOR_CURRENT_A:
      data_return = afc_motor.adc_motor_current_a;
      break;

    case RAM_READ_ADC_MOTOR_CURRENT_B:
      data_return = afc_motor.adc_motor_current_b;
      break;

    case RAM_READ_ADC_PARAMETER_INPUT:
      data_return = adc_parameter_input;
      break;

    case RAM_READ_SIGMA_DATA:
      data_return = afc_data.sigma_data;
      break;

    case RAM_READ_DELTA_DATA:
      data_return = afc_data.delta_data;
      break;

    case RAM_READ_FREQUENCY_ERROR_FILTERED:
      data_return = afc_data.frequency_error_filtered;
      break;
      
    case RAM_READ_FREQUENCY_ERROR_OFFSET:
      data_return = afc_data.frequency_error_offset;
      break;

    case RAM_READ_NUMBER_PULSES_ON:
      data_return = afc_data.pulses_on;
      break;

    case RAM_READ_PRF:
      data_return = pulse_frequency;
      break;

    case RAM_READ_ANALOG_INPUT:
      data_return = adc_analog_value_input;
      break;

    case RAM_READ_ANALOG_OUTPUT:
      data_return = U24_MCP4725.data_12_bit;
      break;

    case RAM_READ_TIME_NOT_PULSING:
      data_return = afc_data.time_off_100ms_units;
      break;

    case RAM_READ_AFC_DISTANCE_FROM_HOME_AT_STOP:
      data_return = afc_data.distance_from_home_at_stop;
      break;

    }  
  return data_return;
}
