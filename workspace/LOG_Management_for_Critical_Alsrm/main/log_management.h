/*
 * log_management.h
 *
 *  Created on: Jun 19, 2023
 *      Author: Embin 7
 */

#ifndef MAIN_LOG_MANAGEMENT_H_
#define MAIN_LOG_MANAGEMENT_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include<string.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "lwip/err.h"


#define LOG_NAMESPACE "log_storage"

#define LOGENTRY_SIZE			9
#define MAX_LOG_ENTRY			100



enum type_of_device {
	digital_input = 0,
	analog_input,
	rs485_input
};

enum type_of_action {
	ack = 0,
	fault
};

enum type_of_ack {
	physical = 0,
	remote,
	none,
};


typedef struct {
	uint32_t epoch_time;
	uint8_t device_id;
	uint8_t type;
	uint8_t ip_id;
	uint8_t action;
	uint8_t ackt;
}log_data_struct;


esp_err_t staore_log_entry(uint16_t log_id, log_data_struct *log_data);

esp_err_t read_log_entry(uint16_t log_id, log_data_struct *log_data);

#endif /* MAIN_LOG_MANAGEMENT_H_ */
