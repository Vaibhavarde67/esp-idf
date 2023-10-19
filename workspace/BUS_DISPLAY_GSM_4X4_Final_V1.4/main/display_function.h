/*
 * display_function.h
 *
 *  Created on: 15-Oct-2022
 *      Author: Embin 7
 */

#ifndef DISPLAY_FUNCTION_H_
#define DISPLAY_FUNCTION_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <esp_log.h>
#include "stddef.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "esp_system.h"

//Files Required for Timer operation and communication between task and ISR
#include "driver/timer.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/ledc.h"

//To avoide Triggering of Watchdog
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"

#include "define.h"




/*
 * Defining Variable for Intencity Control
 */
bool OE_ON;
uint32_t OE_ON_PERIOD, OE_OFF_PERIOD;

//Variable used to select row of display
static uint8_t ROW_ID;

/*
 * Defining SPI handel used for Display
 */
spi_device_handle_t disp_spi;

/*
 * Define structure used for spi transection
 */
spi_transaction_t trans;


/*
 * Defining Event Group For display related events
 */
EventGroupHandle_t display_events;

/* The event group allows multiple bits for each event, but currently we only care about one events:
 * - Refresh Display
 * */
#define DISPLAY_SCROLL_BIT	    BIT0
#define REFRESH_DISPLAY_BIT   	BIT1






/*
 * This function is used to initialize pin used for p6 display
 * @param None
 * @return None
 */

void p6_display_init(uint8_t row, uint8_t col);

/*
 * Function to control Intencity of Display
 * This Function will simply Change OE ON Period and OE Off period
 * @param
 * int_per  --> It is Intencity percentage it must be within the limit below 100 and above minimum limit we have set
 * @return None
 */
void set_display_intencity(uint8_t int_per);


/*
 * Function to send Line data
 */
void DispFunction_scanDisplayBySPI();



#endif /* MAIN_DISPLAY_FUNCTION_H_ */
