/*
 * ldr_driver.h
 *
 *  Created on: Jun 5, 2023
 *      Author: Embin 7
 */

#ifndef MAIN_LDR_DRIVER_H_
#define MAIN_LDR_DRIVER_H_

#include <stdio.h>
#include <stdlib.h>
#include "esp_log.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "define.h"


static esp_adc_cal_characteristics_t adc1_chars;

#define ADC_MIN	    0
#define ADC_MAX		4095

void ldr_adc_init(void);

uint8_t get_intencity_from_ldr(void);



#endif /* MAIN_LDR_DRIVER_H_ */
