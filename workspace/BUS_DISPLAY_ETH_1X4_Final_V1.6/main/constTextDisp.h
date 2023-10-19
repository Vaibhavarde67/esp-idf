/*
 * constTextDisp.h
 *
 *  Created on: 17-Oct-2022
 *      Author: Embin 7
 */

#ifndef CONSTTEXTDISP_H_
#define CONSTTEXTDISP_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "define.h"
#include <time.h>
#include <sys/time.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_spi_flash.h"
#include "freertos/timers.h"

#include "define.h"
//#include "font.h"
//#include "driver/timer.h"


/*
 * Function to Clear the content of Display array according to current mode
 * @param --> None
 * @return --> None
 */
void clear_p6_display_data(void);

void IRAM_ATTR duplicate_the_disp_array(uint8_t row_no);


void Cnstdata_writeProgramIntoDispMem( struct display_prg* prgm);


uint16_t constText_CharLoadInBitMem(uint8_t Character, uint16_t curser, const short int *font, const uint8_t line_no);
uint16_t constGraphics_LoadInBitMem(uint8_t graph_index, uint16_t cursor, uint8_t line_no);


void load_dummy_data(void);

esp_err_t load_initial_program(void);

void initialize_program_refresh_routine(void);

void terminate_program_refresh_routine(void);

void restart_program_refresh_routine(void);

/*
 * Function to Display Error Message
 */
void display_Message(uint8_t error_no);


#endif /* MAIN_CONSTTEXTDISP_H_ */
