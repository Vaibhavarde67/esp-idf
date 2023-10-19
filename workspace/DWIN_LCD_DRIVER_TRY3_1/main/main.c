
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
	uart_event_t event;
	size_t buffered_size;
	//uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);

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

    	if(xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY))
    	{
    		ets_printf("InEvent\n");
    		if(event.type == UART_DATA)
    		{
    			uint16_t rx_fifo_len;
    			rx_fifo_len = event.size;
    			while(rx_fifo_len)
				{
    				uart_read_bytes(DWIN_LCD_UART, &rx_frame[rx_frame_len], 1, portMAX_DELAY);
					//ets_printf("0x%02X",rx_frame[rx_frame_len]);
					rx_fifo_len--;

					if((!frame_start)&&(rx_frame_len==1))
					{
						if((rx_frame[0]!=0x5A)&&(rx_frame[1]!=0xA5))
						{
							rx_frame_len = 0;
							bzero(rx_frame, MAX_FRAM_SIZE);
						}
						else
							frame_start = 1;
					}
					if((frame_start)&&(rx_frame[2]==rx_frame_len-2)&&(rx_frame_len>4))
					{
						frame_start = 0;
						struct dwin_frame recevied_dwin_frametemp;
						//recevied_dwin_frametemp = (void *)malloc(sizeof(struct dwin_frame));

						ets_printf("\nDwin Frame received\n");
						//recevied_dwin_frametemp.ptr_addr = recevied_dwin_frametemp;
						recevied_dwin_frametemp.data_length = rx_frame[2];
						recevied_dwin_frametemp.instruction = rx_frame[3];
						recevied_dwin_frametemp.VP_addr = (rx_frame[4]<<8) | rx_frame[5];
						for(uint i = 0; i<recevied_dwin_frametemp.data_length-3; i++)
						{
							recevied_dwin_frametemp.data[i] = rx_frame[i+6];
							//ets_printf("%x",recevied_dwin_frametemp.data[i]);
						}
						//Extracting RKC
						if((recevied_dwin_frametemp.VP_addr>0x1000)&&(recevied_dwin_frametemp.VP_addr<0x1050))
						{
							//ets_printf("Extracting RKC\n");
							recevied_dwin_frametemp.RKC = (rx_frame[7]<<8) | rx_frame[8];
						}

						ets_printf("\n Received Data Length = %d\n",recevied_dwin_frametemp.data_length);
						ets_printf("Received Instruction = %x\n",recevied_dwin_frametemp.instruction);
						ets_printf("Received VP Address = %x\n",recevied_dwin_frametemp.VP_addr);
						dwin_process_frame(&recevied_dwin_frametemp);
						rx_frame_len = 0;
						bzero(rx_frame, MAX_FRAM_SIZE);
					}
					rx_frame_len++;

				}
    		}
    	}
    }
    vTaskDelete(NULL);
}




void app_main(void)
{

	dwin_CrntPg = 0;
	//struct dwin_frame *recevied_dwin_frame1 = NULL;
	dwin_lcd_init();

	//Create Task to Handel MODBUS Related Operations
	xTaskCreate(dwin_lcd_task, "dwin_lcd_task", 4096, NULL, 6, &dwin_lcd_task_task_handel);
	while(1)
	{
		vTaskDelay(100);
	}
}


