/*
 * graphics.h
 *
 *  Created on: Dec 6, 2022
 *      Author: Embin 7
 */

#ifndef MAIN_GRAPHICS_H_
#define MAIN_GRAPHICS_H_

#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_log.h"

#define STORAGE_NAMESPACE "graphc_storage"
#define GRAPHICS_KEY	   "graphics"
#define TOTAL_GRAPHICS		10

/*
 * Array to tore Graphics data
 * First element will represent width of graphics
 */
uint16_t graphics_data[10][17];

/*
 * nvs storage handel
 */
nvs_handle_t grph_st_handle;

/*
 * Function to init Graphics memory or to load graphics
 */
esp_err_t init_graphics(void);

void save_updated_grapcics(void);


void print_graphics(void);


#endif /* MAIN_GRAPHICS_H_ */
