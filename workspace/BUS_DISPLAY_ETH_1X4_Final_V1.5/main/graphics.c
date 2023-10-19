/*
 * graphics.c
 *
 *  Created on: Dec 6, 2022
 *      Author: Embin 7
 */

#include "graphics.h"

static const char *TAG = "Graphics";


esp_err_t load_default_graphics(void)
{
	esp_err_t err;
	uint16_t deflt_graph[10][17] = {
			/* 1 */			{14,0x0000,0x0000,0x0000,0x0280,0x0100,0x3018,0x3938,0x3d78,0x1d70,0x1d70,0x3d78,0x3938,0x3118,0x0000,0x0000,0x000},
			/* 2 */			{13,0x0000,0x0600,0x0f00,0x0f00,0x0f00,0x1f80,0x3fc0,0x7fe0,0xcf30,0x9f90,0x9f90,0x3fc0,0x0f00,0x0f00,0x0f00,0x0000},
			/* 3 */			{9, 0x0000,0x1800,0x3c00,0x3c00,0x1800,0x1800,0xff00,0xff00,0xff00,0xff00,0xff00,0x3c00,0x3c00,0x3c00,0x3c00,0x3c00},
			/* 4 */			{15,0x0000,0x0400,0x0e00,0x0e00,0x0800,0x0800,0x0400,0x3780,0x6400,0x47c0,0x4020,0x4060,0x4054,0x60d8,0x2080,0x1f00},
			/* 5 */			{13,0x0000,0x0000,0x0000,0x7000,0x7000,0x7000,0x7000,0x6000,0x2000,0x3000,0x1800,0x0cf0,0x07f0,0x01f0,0x0000,0x0000},
			/* 6 */			{13,0x0000,0x3fc0,0x7fe0,0xfff0,0xc030,0xc030,0xc030,0xc030,0xfff0,0xfff0,0xcf30,0xcf30,0x7fe0,0x70e0,0x6060,0x0000},
			/* 7 */			{9, 0x0000,0x0000,0x7f00,0x7f00,0x4100,0x4100,0x4100,0x4100,0x4100,0x4100,0x4100,0x7f00,0x7700,0x7f00,0x0000,0x0000},
			/* 8 */			{10,0x0000,0x0c00,0x1e00,0x3f00,0x7380,0x6180,0x7380,0x7f80,0x3f00,0x1e00,0x1e00,0x0c00,0x0c00,0x7f80,0x7f80,0x0000},
			/* 9 */			{13,0x0000,0x0000,0x0f80,0x18c0,0x2020,0x4d90,0x4d90,0x4010,0x4010,0x4890,0x4710,0x2020,0x18c0,0x0f80,0x0000,0x0000},
			/* 10 */		{14,0x0000,0x0200,0x0700,0x1fc0,0x3fe0,0x7ff0,0xfff8,0x1fc0,0x0200,0x0200,0x0200,0x0200,0x1200,0x0e00,0x0000,0x0000}
						};

	err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &grph_st_handle);
	if (err != ESP_OK)
			return err;
	//printf("Graphics are getting initialiced\n");
	size_t graphics_data_size = sizeof(deflt_graph);
	err = nvs_set_blob(grph_st_handle, GRAPHICS_KEY, deflt_graph, graphics_data_size);
	if (err != ESP_OK)
		return err;

	err = nvs_commit(grph_st_handle);
	if (err != ESP_OK)
		return err;

	nvs_close(grph_st_handle);
	return ESP_OK;

}


esp_err_t init_graphics(void)
{
	esp_err_t err;

	err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		// NVS partition was truncated and needs to be erased
		// Retry nvs_flash_init
		ESP_ERROR_CHECK(nvs_flash_erase());
		nvs_flash_init();
	}
	//nvs_flash_erase();

	//To detect empty NVS and store default graphics on it
	nvs_stats_t nvs_stats;
	nvs_get_stats("nvs", &nvs_stats);
	if(nvs_stats.used_entries==0)
	{
		ESP_LOGE(TAG, "Loading default graphics");
		err = load_default_graphics();
		if(err != ESP_OK)
			return err;
	}

	// Opening the Namespace where graphics data is stored
	err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &grph_st_handle);
	if (err != ESP_OK)
		return err;
	//ESP_LOGE(TAG, "Opened NVS Storage");
	// Reading the graphics data
	size_t graphics_data_size = sizeof(graphics_data);
	err = nvs_get_blob(grph_st_handle, GRAPHICS_KEY, graphics_data, &graphics_data_size);
	if (err != ESP_OK)
			return err;


	nvs_close(grph_st_handle);
	return ESP_OK;

}


void save_updated_grapcics(void)
{
	esp_err_t err;
	err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &grph_st_handle);
	if (err != ESP_OK)
		return;
	//ESP_LOGE(TAG, "Opened NVS Storage");
	// Reading the graphics data
	size_t graphics_data_size = sizeof(graphics_data);
	err = nvs_set_blob(grph_st_handle, GRAPHICS_KEY, graphics_data, graphics_data_size);
	if (err != ESP_OK)
			return;
//	ESP_LOGE(TAG, "Reached Here NVS Storage");
	err = nvs_commit(grph_st_handle);
	if (err != ESP_OK)
		return;

	nvs_close(grph_st_handle);
}

void print_graphics(void)
{
	for(uint8_t j = 0; j<10; j++)
	{
		printf("{");
		for(uint8_t i = 0; i<17; i++)
		{
			printf("%x,",graphics_data[j][i]);
		}
		printf("},\n");
	}
}



