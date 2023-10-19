/*
 * sntp.h
 *
 *  Created on: Feb 16, 2023
 *      Author: Embin 7
 */


#ifndef MAIN_SNTP_H_
#define MAIN_SNTP_H_

#include "esp_attr.h"
#include "esp_sntp.h"
#include <time.h>
#include <sys/time.h>
#include "esp_system.h"
#include "esp_log.h"

#define MAX_SNTP_RETRY_LIMIT	25

esp_err_t obtain_time(void);


/*
 * Function to Print Current Time
 */
void print_current_time(void);


#endif /* MAIN_SNTP_H_ */
