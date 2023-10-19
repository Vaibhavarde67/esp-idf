
//Standard Library
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/param.h>

//Required Free RTOS Libraries
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#include "dwin_gui_handle.h"
//#include "dwinlcd.h"
//#include "dwinlcd_functions.h"


static const char *TAG = "DWIN_LCD_PRG";

//const uint16_t rec_phone_num[] = {1,2,3,4,5,6,7,8,9,10 };

//Creating Task Handel
TaskHandle_t dwin_lcd_task_task_handel = NULL;


static void dwin_lcd_task(void *pvParameters)
{
	dwin_CrntPg = 0;
	//struct dwin_frame *recevied_dwin_frame1 = NULL;
	dwin_lcd_init();

	//Display Starting PAge
	DWIN_GoToPg(WELLCOME1_PAGE);
	vTaskDelay(200);
	DWIN_GoToPg(WELLCOME2_PAGE);
	vTaskDelay(200);
	DWIN_GoToPg(HOME_PAGE);
	DWIN_Set_String(0x2010, "Vaibhav Krishna Arde");
	DWIN_Set_String(0x2020, "Vaibhav Krishna Arde");

	DWIN_Set_String(0x2040, "Vaibhav Krishna Arde");
	DWIN_Set_String(0x2050, "Vaibhav Krishna Arde");
	DWIN_Set_String(0x2060, "Vaibhav Krishna Arde");
	DWIN_Set_String(0x2070, "Vaibhav Krishna Arde");

	//uint8_t* data;
    for(;;)
    {
    	if(xQueueReceive(dwin_frame_queu, &recevied_dwin_frame, portMAX_DELAY) == pdPASS)
    	{
    		//ets_printf("\n Received Data Length = %d\n",recevied_dwin_frame.data_length);
			//ets_printf("Received Instruction = %x\n",recevied_dwin_frame.instruction);
			//ets_printf("Received VP Address = %x\n",recevied_dwin_frame.VP_addr);
			//ets_printf("Received RKC Address = %x\n",recevied_dwin_frame.RKC);
			dwin_process_frame(&recevied_dwin_frame);
			//free(recevied_dwin_frame1);
    	}
    	ets_printf("Available free Size = %d",esp_get_free_internal_heap_size());
    	vTaskDelay(10);


    }
    vTaskDelete(NULL);
}


void app_main(void)
{

	//Create Task to Handel MODBUS Related Operations
	xTaskCreate(dwin_lcd_task, "dwin_lcd_task", 4096, NULL, 6, &dwin_lcd_task_task_handel);
	while(1)
	{
		vTaskDelay(100);
	}
}


