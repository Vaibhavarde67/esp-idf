#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/timers.h"
#include "esp_system.h"

#include "font.h"
#include "define.h"
#include "constTextDisp.h"
#include "display_function.h"
#include "graphics.h"
#include "ethrnet.h"
#include "server_comm.h"
#include "sntp.h"
#include "fota.h"
#include "ldr_driver.h"



char* TAG = "MainAppFile";

static uint8_t  disp_init = 0;
//static uint8_t RTC_NOINIT_ATTR time_init;
//static uint8_t RTC_NOINIT_ATTR fota_init;
//static uint8_t RTC_NOINIT_ATTR graph_init;


static void display_intencity(void *pvParameters)
{
	uint8_t intencity = MIN_INTENCITY;
	while(pdTRUE)
	{
		//ESP_LOGE(TAG,"In Task 2");
		intencity=get_intencity_from_ldr();
		if(intencity > MAX_INTENCITY)
			intencity = MAX_INTENCITY;
		else if(intencity < MIN_INTENCITY)
			intencity = MIN_INTENCITY;
		set_display_intencity(intencity);		// commented for video
		//printf("Intencity = %d",intencity);
		//Resetting Watchdog
		TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
		TIMERG0.wdt_feed=1;
		TIMERG0.wdt_wprotect=0;
		vTaskDelay(1000);
	}
}

void init_prg(void)
{


	if(display_prograns[0].prg_status == TRUE )
	{
	if(display_prograns[0].prg_exp_tm > 0)
	{
	int	counter = display_prograns[0].prg_exp_tm;
	counter--;
	if(counter == 0)
	{
	Cnstdata_writeProgramIntoDispMem(&display_prograns[current_program]);
	current_program++;
	}
	}
	}
	else if(display_prograns[1].prg_status == TRUE)
	{
		if(display_prograns[0].prg_exp_tm > 0)
		{
		int	counter = display_prograns[0].prg_exp_tm;
		counter--;
		if(counter == 0)
		{
		Cnstdata_writeProgramIntoDispMem(&display_prograns[current_program]);
		current_program++;
		}
        }
	}
}



void app_main(void)
{

	//Initialize the graphics
	esp_err_t err;
	bool disp_init = FALSE;
	// Create default event loop that running in background
	esp_event_loop_create_default();
	//Installing GPIO ISR Service
	gpio_install_isr_service(0);

	//Get MAC Address for device Identification
	esp_read_mac(mac,ESP_MAC_WIFI_STA);
	sprintf(mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	err = init_graphics();
	if(err != ESP_OK)
		ESP_LOGI(TAG,"Graphics not Initialiced\n");

//	p6_display_init(1, 4);

	//Setup Status Led
	gpio_set_direction(STS_LED1_PIN, GPIO_MODE_OUTPUT);
	gpio_set_level(STS_LED1_PIN, 1);

	gpio_set_direction(STS_LED2_PIN, GPIO_MODE_OUTPUT);
	gpio_set_level(STS_LED2_PIN, 1);

	p6_display_init(1, 4);
	clear_p6_display_data();

	selected_program = 0;
	load_dummy_data();
	err = load_initial_program();
	if(err == ESP_OK)
	{
		current_program  = selected_program;
		nothing_to_disp = FALSE;
		timer_pause(DIS_TIMER_GRP, DIS_TIMER_ID);
		Cnstdata_writeProgramIntoDispMem(&display_prograns[current_program]);
		timer_start(DIS_TIMER_GRP, DIS_TIMER_ID);
		current_program_cnt = display_prograns[current_program].prg_sec;
	}
	else
	{
		//clear_p6_display_data();
		nothing_to_disp = TRUE;
		current_program  = selected_program;
		timer_pause(DIS_TIMER_GRP, DIS_TIMER_ID);
		Cnstdata_writeProgramIntoDispMem(&display_prograns[selected_program]);
		timer_start(DIS_TIMER_GRP, DIS_TIMER_ID);
		ESP_LOGI(TAG, "There is No any Active Program");
	}


strt: err = enc28j60_init();
//	if(err == DID_NOT_GET_IP_ADDRESS)
//	{
//		ESP_LOGI(TAG,"Not Got IP Address\n");
//	}
	if(err != ESP_OK)
	{
//		if(!disp_init)
//		{
//			p6_display_init(7, 5);
//			disp_init = TRUE;
//		}
		ESP_LOGI(TAG,"Error = %d while Initating Ethernet Module\n",err);
		//display_Message(ETHERNET_MODULE_ERROR);
		vTaskDelay(10000);
		ESP_LOGI(TAG,"Reinitalizing Ethernet Module");
		goto strt;
	}
	int retry_count = 0;
	while(!NET_CONNECTED)
	{
		retry_count++;
		if(retry_count >= MAX_ETHERNET_RETRY_COUNT)
		{
			//display_Message(DID_NOT_GET_IP_ADDRESS);
			//Now wait untill we get connect to Internet
			while(!NET_CONNECTED)
			{
				//Resetting Watchdog
				TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
				TIMERG0.wdt_feed=1;
				TIMERG0.wdt_wprotect=0;
				vTaskDelay(1000);
			}
		}
		vTaskDelay(100);
	}

	//Check For Update Available or not
	uint8_t fota_retry_count = 0;
	//timer_pause(DIS_TIMER_GRP, DIS_TIMER_ID);

	while((get_update()!=ESP_OK)&&(fota_retry_count<MAX_FOTA_RETRY_COUNT))
	{
		fota_retry_count++;
		 vTaskDelay(500);
	}

	timer_start(DIS_TIMER_GRP, DIS_TIMER_ID);



	if(update_graphics()==ESP_OK)
		save_updated_grapcics();


	//Getting Time Synchronised

	time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    // Is time set? If not, tm_year will be (1970 - 1900).
    if (timeinfo.tm_year < (2023 - 1900))
    {
	   ESP_LOGI(TAG, "Time is not set yet. Getting time over NTP.");
str2:	err = obtain_time();
	   if(err!=ESP_OK)
	   {
		   ESP_LOGI(TAG, "Unable to get Time Synchronized");
		   vTaskDelay(1000);
		   goto str2;
	   }
	   // update 'now' variable with current time
	   time(&now);
    }
    setenv("TZ", "IST-5:30", 1);
    tzset();


	//p6_display_init(7, 5);

	initialize_program_refresh_routine();

	ldr_adc_init();
	xTaskCreatePinnedToCore(display_intencity, "display_Refresh_task2", 8192, NULL, 3, NULL, 0);

	int http_req_fl_cn = 0;
	bool svr_err = FALSE;
	server_comm_init();
	while(1)
	{
		ESP_LOGI(TAG,"Available free Size = %d",esp_get_free_internal_heap_size());
		ESP_LOGI(TAG,"Sending Request");
		err = send_https_request();
		if((err != ESP_OK))
		{
			gpio_set_level(STS_LED2_PIN, 1);
			if(!svr_err)
			{
				http_req_fl_cn++;
				if(http_req_fl_cn >= MAX_HTTPS_RETRY_COUNT)
				{
					terminate_program_refresh_routine();
					display_Message(SERVER_ERROR);
					svr_err = TRUE;
					clear_p6_display_data();
					vTaskDelay(500);
					esp_restart();

	//				esp_netif_destroy(eth_netif);
	//				terminate_program_refresh_routine();
	//				goto strt;
				}
			}
		}
		else
		{
			http_req_fl_cn = 0;
			gpio_set_level(STS_LED2_PIN, 0);
			if(svr_err)
			{
				svr_err = FALSE;
				restart_program_refresh_routine();
			}
		}
		//Resetting Watchdog
		TIMERG0.wdt_wprotect=TIMG_WDT_WKEY_VALUE;
		TIMERG0.wdt_feed=1;
		TIMERG0.wdt_wprotect=0;
		vTaskDelay(1000);
	}

}


