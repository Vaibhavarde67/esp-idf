/*
 * critical_alarm_data_structure.h
 *
 *  Created on: 02-Oct-2023
 *      Author: vaibh
 */

#ifndef MAIN_CRITICAL_ALARM_DATA_STRUCTURE_H_
#define MAIN_CRITICAL_ALARM_DATA_STRUCTURE_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>


#define MAX_NO_OF_IO_BOARD		4
#define NO_OF_DIGITAL_IP		16
#define NO_OF_ANALOG_IP			4
#define NO_OF_RS485_SENSOR		2


struct DIGITAL_INPUT {
	char digital_ip_name[20];
};

struct ANALOG_INPUT {
	char analog_ip_name[20];
	uint16_t limit;
};



struct RS_485_SENSOR {
	struct TEMP_SENSOR {
		char temperature_name[20];
		uint16_t limit;
	}temperature_sensor;

	struct HUMIDITY_SENSOR {
		char humidity_name[20];
		uint16_t limit;
	}humidity_sensor;
	char humidity_name[20];
};


struct RS485_INPUT{
	struct RS_485_SENSOR rs_485_sensor[NO_OF_RS485_SENSOR];
};



struct IO_BOARD {
	char io_board_name[20];
	struct DIGITAL_INPUT digital_input[NO_OF_DIGITAL_IP];
	struct ANALOG_INPUT analog_input[NO_OF_ANALOG_IP];
	struct RS485_INPUT rs485_input[NO_OF_RS485_SENSOR];
}io_board;





#endif /* MAIN_CRITICAL_ALARM_DATA_STRUCTURE_H_ */
