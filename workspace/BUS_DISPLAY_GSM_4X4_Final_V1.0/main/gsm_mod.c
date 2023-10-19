/*
 * gsm_mod.c
 *
 *  Created on: Feb 27, 2023
 *      Author: Embin 7
 */

#include "gsm_mod.h"


static const char *TAG = "GSM Module";


static void modem_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id) {
    case ESP_MODEM_EVENT_PPP_START:
        ESP_LOGI(TAG, "Modem PPP Started");
        break;
    case ESP_MODEM_EVENT_PPP_STOP:
        ESP_LOGI(TAG, "Modem PPP Stopped");
        break;
    case ESP_MODEM_EVENT_UNKNOWN:
        ESP_LOGW(TAG, "Unknown line received: %s", (char *)event_data);
        break;
    default:
        break;
    }
}



static void on_ip_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "IP event! %d", event_id);
    if (event_id == IP_EVENT_PPP_GOT_IP)
    {
    	NET_CONNECTED = TRUE;
    	gpio_set_level(STS_LED1_PIN, 0);
        esp_netif_dns_info_t dns_info;

        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        esp_netif_t *netif = event->esp_netif;

        ESP_LOGI(TAG, "Modem Connect to PPP Server");
        ESP_LOGI(TAG, "~~~~~~~~~~~~~~");
        ESP_LOGI(TAG, "IP          : " IPSTR, IP2STR(&event->ip_info.ip));
        ESP_LOGI(TAG, "Netmask     : " IPSTR, IP2STR(&event->ip_info.netmask));
        ESP_LOGI(TAG, "Gateway     : " IPSTR, IP2STR(&event->ip_info.gw));
        esp_netif_get_dns_info(netif, 0, &dns_info);
        ESP_LOGI(TAG, "Name Server1: " IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
        esp_netif_get_dns_info(netif, 1, &dns_info);
        ESP_LOGI(TAG, "Name Server2: " IPSTR, IP2STR(&dns_info.ip.u_addr.ip4));
        ESP_LOGI(TAG, "~~~~~~~~~~~~~~");

        ESP_LOGI(TAG, "GOT ip event!!!");
    }
    else if (event_id == IP_EVENT_PPP_LOST_IP)
    {
    	NET_CONNECTED = FALSE;
    	gpio_set_level(STS_LED1_PIN, 1);
        ESP_LOGI(TAG, "Modem Disconnect from PPP Server");
    }
    else if (event_id == IP_EVENT_GOT_IP6)
    {
        ESP_LOGI(TAG, "GOT IPv6 event!");
        ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
        ESP_LOGI(TAG, "Got IPv6 address " IPV6STR, IPV62STR(event->ip6_info.ip));
    }
}



static void on_ppp_changed(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "PPP state changed event %d", event_id);
    if (event_id == NETIF_PPP_ERRORUSER)
    {
        /* User interrupted event from esp-netif */
        esp_netif_t *netif = *(esp_netif_t**)event_data;
        ESP_LOGI(TAG, "User interrupted event from netif:%p", netif);
    }
}



/*
 * Function to Reset the SIM800 module
 */
void reset_sim800()
{
	ESP_LOGI(TAG, "Reseting SIM800 Module");
	gpio_set_level(SIM800_RESET_PIN, 0);
	vTaskDelay(200 / portTICK_PERIOD_MS);
	gpio_set_level(SIM800_RESET_PIN, 1);
	vTaskDelay(500);
}

/*
 * Function to initialize the Pins for GSM module
 */
void setup_gsm_module(void)
{
	/*
	 * Setup the GPIO pins used by RESET Pin of SIM800
	 */
	gpio_reset_pin(SIM800_RESET_PIN);
	// Set the GPIO as a push/pull output

	gpio_set_direction(SIM800_RESET_PIN, GPIO_MODE_OUTPUT);
	gpio_set_level(SIM800_RESET_PIN, 1);
	reset_sim800();

	esp_netif_init();
	esp_event_loop_create_default();
	esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &on_ip_event, NULL);
	esp_event_handler_register(NETIF_PPP_STATUS, ESP_EVENT_ANY_ID, &on_ppp_changed, NULL);

	 /* create dte object */
	esp_modem_dte_config_t config = ESP_MODEM_DTE_DEFAULT_CONFIG();
	/* setup UART specific configuration based on kconfig options */
	config.tx_io_num = SIM800_RX_PIN;
	config.rx_io_num = SIM800_TX_PIN;
	config.rts_io_num = -1;
	config.cts_io_num = -1;
	config.rx_buffer_size = 1024;
	config.tx_buffer_size = 512;
	config.event_queue_size = 30;
	config.event_task_stack_size = 3072;
	config.event_task_priority = 5;
	config.dte_buffer_size = 1024 / 2;

	//modem_dte_t *dte = esp_modem_dte_init(&config);
	dte = esp_modem_dte_init(&config);
	/* Register event handler */
	esp_modem_set_event_handler(dte, modem_event_handler, ESP_EVENT_ANY_ID, NULL);

	// Init netif object
	esp_netif_config_t cfg = ESP_NETIF_DEFAULT_PPP();
	esp_netif = esp_netif_new(&cfg);
	assert(esp_netif);

	modem_netif_adapter = esp_modem_netif_setup(dte);
	esp_modem_netif_set_default_handlers(modem_netif_adapter, esp_netif);
}

/*
 * Function to Initiate SIM800
 */
esp_err_t initiate_gsm_module()
{
	uint8_t retry_count = 0, reset_count = 0;
	esp_err_t err;
start:ESP_LOGI(TAG, "Initialising the GSM Module ");
	NET_CONNECTED = FALSE;
	dce = NULL;

	/*
	 * Trying To get connection with SIM800 module
	 */
	retry_count = 0;
	do
	{
		dce = sim800_init(dte);
		if(dce == NULL)
		{
			ESP_LOGI(TAG, "Retrying to connect with SIM800");
			retry_count++;
			if(retry_count >= MAX_SIM800_RETRY_COUNT)
			{
				retry_count = 0;
				if(reset_count< MAX_SIM800_RESET_COUNT)
				{
					reset_sim800();
					reset_count++;
					goto start;
				}
				else
					return ESP_FAIL;

			}
			vTaskDelay(1000/portTICK_PERIOD_MS);
		}
	}while(dce == NULL);
	/*
	 * Once connection with SIM800 is Successful then
	 * Wait until we get Working Network
	 */
	//assert(dce != NULL);
	dce->set_flow_ctrl(dce, MODEM_FLOW_CONTROL_NONE);
	dce->store_profile(dce);
	/* Print Module ID, Operator, IMEI, IMSI */
	ESP_LOGI(TAG, "Module: %s", dce->name);
	ESP_LOGI(TAG, "Operator: %s", dce->oper);
	ESP_LOGI(TAG, "IMEI: %s", dce->imei);
	ESP_LOGI(TAG, "IMSI: %s", dce->imsi);

	/*
	 * Wait for Network Connection
	 */
	retry_count = 0;
	uint32_t rssi = 0, ber = 0;
	while((rssi == 0)||(rssi > 99))
	{
		err = dce->get_signal_quality(dce, &rssi, &ber);
		if(err!= ESP_OK)
		{
			retry_count++;
			if(retry_count >= MAX_SIM800_RETRY_COUNT)
			{
				retry_count = 0;
				if(reset_count< MAX_SIM800_RESET_COUNT)
				{
					reset_sim800();
					reset_count++;
					goto start;
				}
				else
					return ESP_FAIL;
			}
		}
		else if((rssi == 0)||(rssi > 99))
		{
			ESP_LOGI(TAG, "Retrying to Connect to Network");
			vTaskDelay(1000/portTICK_PERIOD_MS);
		}
	}
	ESP_LOGI(TAG, "rssi: %d, ber: %d", rssi, ber);


	/*
	 * attach the modem to the network interface
	 */
	retry_count = 0;
	do
	{
		ESP_LOGI(TAG, "Attaching the modem to the network interface");
		err = esp_netif_attach(esp_netif, modem_netif_adapter);
		if(err!= ESP_OK)
		{
			retry_count++;
			if(retry_count >= MAX_SIM800_RETRY_COUNT)
			{
				retry_count = 0;
				if(reset_count< MAX_SIM800_RESET_COUNT)
				{
					reset_sim800();
					reset_count++;
					goto start;
				}
				else
					return ESP_FAIL;
			}
		}
		if(err == ESP_FAIL)
			vTaskDelay(1000/portTICK_PERIOD_MS);
	}while(err == ESP_FAIL );

	vTaskDelay(1000/portTICK_PERIOD_MS);
	/*
	 * Wait for IP address
	 */
	retry_count = 0;
//	esp_netif_ip_info_t *ip_infor;
	while(!NET_CONNECTED)
	{
		retry_count++;
		ESP_LOGI(TAG, "Waiting For get IP Address");
		if(retry_count >= MAX_SIM800_RETRY_COUNT)
		{
			retry_count = 0;
			if(reset_count< MAX_SIM800_RESET_COUNT)
			{
				reset_sim800();
//				esp_modem_dce_connect_gprs(dce);
//				vTaskDelay(2000/portTICK_PERIOD_MS);
//				esp_modem_dce_disconnect_gprs(dce);
//				vTaskDelay(2000/portTICK_PERIOD_MS);
				reset_count++;
				goto start;
			}
			else
				return ESP_FAIL;
		}
//		if(esp_netif_is_netif_up(esp_netif)==true)
//		{
//			 ESP_LOGI(TAG, "Network Interface is Up");
//		}
		vTaskDelay(5000/portTICK_PERIOD_MS);
	}


	return ESP_OK;

}
