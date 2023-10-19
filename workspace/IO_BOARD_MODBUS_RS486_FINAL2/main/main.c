
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

#include "system.h"
#include "modbusrd.h"

static const char *TAG = "IO_BOARD_MODBUS";

//Creating Task Handel
TaskHandle_t modbus_master_task_handel = NULL;


static void modbus_master_task(void *pvParameters)
{
	uint8_t* data;
    for(;;)
    {
    	if(modb_read_holding_register(1, 0, 4)==MODB_OK)
    	{
    		//printing res value
    		uint16_t resp;
    		for(uint8_t i =0 ;i <4; i++)
    		{
    			resp = rx_frame[3+i*2];
    			resp = (resp<<8)| rx_frame[3+i*2+1];
    			ets_printf("Value of Reg %d = %d\n",i,resp);
    		}

    	}

//    	modb_read_input_register(1, 0, 4);
//    	modb_write_single_coil(1, 0, false);
    	modb_write_single_reg(1, 0, 6767);

//    	data = (uint8_t*) malloc(2);
//    	data[0]=0XFF;
//    	data[1]=0X03;
//    	modb_write_multiple_coils(1,0,10,&data[0]);
    	data = (uint8_t*) malloc(20);
    	for(uint8_t i = 0;i<10;i++)
    	{
    		data[i*2] = 0X56;
    		data[i*2+1] =0X6F;
    	}
    	modb_write_multiple_regs(1, 0, 10, &data[0]);
    	//ESP_LOGI(TAG," Single Cycle completed\n");
    	//uart_write_bytes(MOD_UART, (const char *) tx_frame, frame_len);
    	//printf(&tx_frame);
    	vTaskDelay(100);
    }
    vTaskDelete(NULL);
}


void app_main(void)
{
//	gpio_reset_pin(COM_CONTROL_PIN);
//	gpio_set_direction(COM_CONTROL_PIN, GPIO_MODE_OUTPUT);
//
//    printf("inside while\n");
//    while(1){
//	gpio_set_level(COM_CONTROL_PIN, RESET);
//    vTaskDelay(100);
//
//	gpio_set_level(COM_CONTROL_PIN, SET);
//	vTaskDelay(100);}
	//Initalize MODBUS
	modbus_init();

	//Create Task to Handel MODBUS Related Operations
	xTaskCreate(modbus_master_task, "modbus_master_task", 4096, NULL, 6, &modbus_master_task_handel);
}


