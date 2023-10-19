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

#define MAX_NAME_LENGTH_ALLOWED	20
#define MAX_EMAIL_ID_LENGTH		50
#define MAX_PHONE_NUM_LENGTH	11		//Extra spcce to store the null character

#define MAX_NO_OF_IO_BOARD		4
#define NO_OF_DIGITAL_IP		16
#define NO_OF_ANALOG_IP			4
#define NO_OF_RS485_SENSOR		2

#define MAX_RECEIVER_EMAILS		10
#define MAX_LEVEL1_PERSONS		3
#define MAX_LEVEL2_PERSONS		3
#define MAX_LEVEL3_PERSONS		4

#define IO_BORD_SETNG_NAMESPACE "iobrd_setng_stg"		//Name should be no long
#define IO_BORD_DATA_SIZE		sizeof(io_board_input_data[0])
#define IO_BVOARD_COMM_DET_SIZE	sizeof(io_board_comm_info)
#define WIFI_DATA_SIZE			sizeof(wifi_data)
#define SCHEDULING_INFO_SIZE	sizeof(scheduling_info)


struct DIGITAL_INPUT {
	char digital_ip_name[MAX_NAME_LENGTH_ALLOWED];
};

struct ANALOG_INPUT {
	char analog_ip_name[MAX_NAME_LENGTH_ALLOWED];
	uint16_t limit;
};



struct RS_485_SENSOR {
	struct TEMP_SENSOR {
		char temperature_name[MAX_NAME_LENGTH_ALLOWED];
		uint16_t limit;
	}temperature_sensor;

	struct HUMIDITY_SENSOR {
		char humidity_name[MAX_NAME_LENGTH_ALLOWED];
		uint16_t limit;
	}humidity_sensor;
};




//****************************************************************
struct IO_BOARD {
	char io_board_name[MAX_NAME_LENGTH_ALLOWED];
	struct DIGITAL_INPUT digital_input[NO_OF_DIGITAL_IP];
	struct ANALOG_INPUT analog_input[NO_OF_ANALOG_IP];
	struct RS_485_SENSOR rs485_input[NO_OF_RS485_SENSOR];
}io_board_input_data[MAX_NO_OF_IO_BOARD];

//****************************************************************


struct EMAIL_SRV_INFO {
	char sender_email_id[MAX_EMAIL_ID_LENGTH];
	char sender_email_server[MAX_EMAIL_ID_LENGTH];
	char sender_email_password[30];
	char sender_name[MAX_NAME_LENGTH_ALLOWED];
	uint16_t port_number;
};

struct REC_EMAIL_INFO {
	char receiver_email[MAX_RECEIVER_EMAILS][MAX_EMAIL_ID_LENGTH];
};

struct MOBILE_NO_INFO {
	char level1_mobiles[MAX_LEVEL1_PERSONS][MAX_PHONE_NUM_LENGTH];
	char level2_mobiles[MAX_LEVEL2_PERSONS][MAX_PHONE_NUM_LENGTH];
	char level3_mobiles[MAX_LEVEL3_PERSONS][MAX_PHONE_NUM_LENGTH];
};


//***********************************************************
struct IO_BORD_COMM_INFO {
	struct EMAIL_SRV_INFO email_srv_info;
	struct REC_EMAIL_INFO rec_email_info;
	struct MOBILE_NO_INFO mobile_no_info;
}io_board_comm_info;
//************************************************************


struct SCHEDULING_INFO {
	uint8_t hour;
	uint8_t Minutes;
	bool days[7];
}scheduling_info;


enum setting_type {
	WIFI_SETTING = 0,
	SCHEDULING_INFO,
	DEVICE_NAME_SETTING,
	IO_BOARD_COUNT_SETTING,
	ESCALATION_TIME_SETTING,
};

struct WIFI_DATA {
	char wifi_ssid[30];
	char wifi_pswd[30];
}wifi_data;

char device_name[MAX_NAME_LENGTH_ALLOWED];
uint8_t present_io_board_count;
uint8_t ecsalation_mins;




#endif /* MAIN_CRITICAL_ALARM_DATA_STRUCTURE_H_ */
