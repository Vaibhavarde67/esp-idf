#include "log_management.h"

void app_main(void)
{

	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		// NVS partition was truncated and needs to be erased
		// Retry nvs_flash_init
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}

	log_data_struct str, rd;
	str.epoch_time = 12356789;
	str.device_id = 1;
	str.type= 2;
	str.ip_id = 3;
	str.action = 4;
	str.ackt = 5;

	//staore_log_entry(6, &str);

	if(read_log_entry(6, &rd)==ESP_OK)
	{
		printf("Read Epoch time = %d\n", rd.epoch_time);
		printf("Read device Id = %d\n", rd.device_id);
		printf("Read typr = %d\n", rd.type);
		printf("Read Input Id = %d\n",rd.ip_id);
		printf("Read Action = %d\n",rd.action);
		printf("Read Action type = %d\n",rd.ackt);
		//printf("Read ");
	}


    while(1)
    {
    	sleep(100);
    }
}
