/*
 * ldr_driver.c
 *
 *  Created on: Jun 5, 2023
 *      Author: Embin 7
 */
#include "ldr_driver.h"

static const char *TAG = "LDR Driver";


void ldr_adc_init(void)
{
	esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_DEFAULT, 0, &adc1_chars);
	return;
}



uint8_t get_intencity_from_ldr(void)
{
	uint16_t raw =  adc1_get_raw(ADC1_CHANNEL_6);
	ESP_LOGI(TAG, "Raw data %d", raw);
	uint8_t out = (MAX_INTENCITY - MIN_INTENCITY) * (double)( ADC_MAX - raw) / ADC_MAX  + MIN_INTENCITY;
	ESP_LOGI(TAG, "LDR OUTPUT DATA : %d", out);
	return out;
}

