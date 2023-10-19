/* PPPoS Client Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "esp_modem.h"
#include "esp_modem_netif.h"
#include "esp_log.h"
#include "sim800.h"
//#include "bg96.h"
//#include "sim7600.h"
#include "driver/gpio.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "esp_http_client.h"

#include "esp_efuse.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "lwip/err.h"

#include "esp_netif.h"
#include <stdbool.h>

#include "esp_modem_dce_service.h"


static const char *TAG = "FOTA_TRY_USING_PPOS";
static EventGroupHandle_t event_group = NULL;
static const int CONNECT_BIT = BIT0;
static const int STOP_BIT = BIT1;


#define OTA_URL_SIZE 256

#define MAX_SIM800_RETRY_COUNT	20
#define MAX_SIM800_RESET_COUNT	2

/*
 * If Http request Fails Continously for this time
 * We will restart the GSM Module
 */
#define MAX_HTTPS_RETRY_COUNT	20

#define MAX_FOTA_RETRY_COUNT	5
#define MAX_FOTA_PACKET_RETRY	60

#define MAX_HTTP_OUTPUT_BUFFER 2048
#define MAX_HTTP_TIMOUT 	   5000

#define	SIM800_RESET_PIN	22

extern const uint8_t server_cert_github_pem_start[] asm("_binary_github_pem_start");
extern const uint8_t server_cert_github_pem_end[] asm("_binary_github_pem_end");

//Variable for Http Project
modem_dte_t *dte;
modem_dce_t *dce;
esp_netif_t *esp_netif;
void *modem_netif_adapter;
esp_err_t err;
esp_http_client_handle_t client;
bool http_rq_fail,got_ip;
uint8_t http_rq_retry_count;

uint8_t fota_retry_count;



static void modem_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    switch (event_id) {
    case ESP_MODEM_EVENT_PPP_START:
        ESP_LOGI(TAG, "Modem PPP Started");
        break;
    case ESP_MODEM_EVENT_PPP_STOP:
        ESP_LOGI(TAG, "Modem PPP Stopped");
        xEventGroupSetBits(event_group, STOP_BIT);
        break;
    case ESP_MODEM_EVENT_UNKNOWN:
        ESP_LOGW(TAG, "Unknown line received: %s", (char *)event_data);
        break;
    default:
        break;
    }
}


/**Event handeler for HTTP **/
esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
	static char *output_buffer;  // Buffer to store response of http request from event handler
	static int output_len;       // Stores number of bytes read
	switch(evt->event_id)
	{
	 case HTTP_EVENT_ERROR:
		 ESP_LOGE(TAG, "HTTP_EVENT_ERROR");
		 http_rq_fail = true;
	     break;

	 case HTTP_EVENT_ON_CONNECTED:
		 ESP_LOGE(TAG, "HTTP_EVENT_ON_CONNECTED");
	     break;

	 case HTTP_EVENT_HEADER_SENT:
		 ESP_LOGE(TAG, "HTTP_EVENT_HEADER_SENT");
	     break;

	 case HTTP_EVENT_ON_HEADER:
		 ESP_LOGE(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
	     break;

	 case HTTP_EVENT_ON_DATA:
		 //ESP_LOGE(TAG, "HTTP_EVENT_ON_DATA = %.*s" ,evt->data_len, (char*) evt->data );
		 /*
		  * Check for chunk Encoding and according to read responce
		  * */
		 if (!esp_http_client_is_chunked_response(evt->client))
		 {
			 // If user_data buffer is configured, copy the response into the buffer
		     if (evt->user_data)
		     {
		    	 memcpy(evt->user_data + output_len, evt->data, evt->data_len);
		     }
		      else
		      {
		    	  if (output_buffer == NULL)
		    	  {
		    		  output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
		              output_len = 0;
		              if (output_buffer == NULL)
		              {
		            	  ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
		                  return ESP_FAIL;
		              }
		          }
		          memcpy(output_buffer + output_len, evt->data, evt->data_len);
		      }
		      output_len += evt->data_len;
		 }
		 break;

	 case HTTP_EVENT_ON_FINISH:
		 ESP_LOGE(TAG, "HTTP_EVENT_ON_FINISH");
	     if (output_buffer != NULL)
	     {
	    	 // Response is accumulated in output_buffer. the below line to print the accumulated response
	    	 //ESP_LOGE(TAG, output_buffer, output_len);
	    	 ESP_LOGE(TAG, "Received Responce = %.*s" ,output_len, (char*) output_buffer );
	         free(output_buffer);
	         output_buffer = NULL;
	     }
	     output_len = 0;
	     break;

	 case HTTP_EVENT_DISCONNECTED:
		 ESP_LOGE(TAG, "HTTP_EVENT_DISCONNECTED");
	     int mbedtls_err = 0;
	     esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
	     if (err != 0)
	     {
	    	 if (output_buffer != NULL)
	    	 {
	    		 free(output_buffer);
	             output_buffer = NULL;
	         }
	         output_len = 0;
	         ESP_LOGE(TAG, "Last esp error code: 0x%x", err);
	         ESP_LOGE(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
	     }
	     break;

	 default:
		 break;
	}
	return ESP_OK;
}




static void on_ip_event(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "IP event! %d", event_id);
    if (event_id == IP_EVENT_PPP_GOT_IP)
    {
    	got_ip= true;
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
        xEventGroupSetBits(event_group, CONNECT_BIT);

        ESP_LOGI(TAG, "GOT ip event!!!");
    } else if (event_id == IP_EVENT_PPP_LOST_IP) {
        ESP_LOGI(TAG, "Modem Disconnect from PPP Server");
    } else if (event_id == IP_EVENT_GOT_IP6) {
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
 * Function to Validate Image File
 */
static esp_err_t validate_image_header(esp_app_desc_t *new_app_info)
{
    if (new_app_info == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const esp_partition_t *running = esp_ota_get_running_partition();
    esp_app_desc_t running_app_info;
    if (esp_ota_get_partition_description(running, &running_app_info) == ESP_OK) {
        ESP_LOGE(TAG, "Running firmware version: %s", running_app_info.version);
    }

//#ifndef CONFIG_EXAMPLE_SKIP_VERSION_CHECK
    if (memcmp(new_app_info->version, running_app_info.version, sizeof(new_app_info->version)) == 0) {
    	ESP_LOGE(TAG, "Current running version is the same as a new. We will not continue the update.");
        return ESP_FAIL;
    }
//#endif

//#ifdef CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK
    /**
     * Secure version check from firmware image header prevents subsequent download and flash write of
     * entire firmware image. However this is optional because it is also taken care in API
     * esp_https_ota_finish at the end of OTA update procedure.
     */
    const uint32_t hw_sec_version = esp_efuse_read_secure_version();
    if (new_app_info->secure_version < hw_sec_version) {
        ESP_LOGW(TAG, "New firmware security version is less than eFuse programmed, %d < %d", new_app_info->secure_version, hw_sec_version);
        return ESP_FAIL;
    }
//#endif

    return ESP_OK;
}
static esp_err_t _http_client_init_cb(esp_http_client_handle_t http_client)
{
    esp_err_t err = ESP_OK;
    /* Uncomment to add custom headers to HTTP request */
    // err = esp_http_client_set_header(http_client, "Custom-Header", "Value");
    return err;
}

/*
 * Function Responsible for performing Update
 */
esp_err_t static inline get_update(void)
{
    ESP_LOGE(TAG, "Starting Advanced OTA example");
    esp_err_t ota_finish_err = ESP_OK;
    esp_http_client_config_t config = {
        .url = "https://github.com/Vaibhavarde67/esp32update/raw/main/ppos_fota_over_gsm.bin",
        .cert_pem = (char *)server_cert_github_pem_start,
        .timeout_ms = 10000,
        .keep_alive_enable = true,
    };

/*#ifdef CONFIG_EXAMPLE_FIRMWARE_UPGRADE_URL_FROM_STDIN
    char url_buf[OTA_URL_SIZE];
    if (strcmp(config.url, "FROM_STDIN") == 0) {
        example_configure_stdin_stdout();
        fgets(url_buf, OTA_URL_SIZE, stdin);
        int len = strlen(url_buf);
        url_buf[len - 1] = '\0';
        config.url = url_buf;
    } else {
        ESP_LOGE(TAG, "Configuration mismatch: wrong firmware upgrade image url");
        abort();
    }
#endif*/

//#ifdef CONFIG_EXAMPLE_SKIP_COMMON_NAME_CHECK
    config.skip_cert_common_name_check = true;
//#endif

    esp_https_ota_config_t ota_config =
    {
        .http_config = &config,
        .http_client_init_cb = _http_client_init_cb, // Register a callback to be invoked after esp_http_client is initialized
/*#ifdef CONFIG_EXAMPLE_ENABLE_PARTIAL_HTTP_DOWNLOAD
        .partial_http_download = true,
        .max_http_request_size = CONFIG_EXAMPLE_HTTP_REQUEST_SIZE,
#endif*/
    };

    esp_https_ota_handle_t https_ota_handle = NULL;
    esp_err_t err = esp_https_ota_begin(&ota_config, &https_ota_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "ESP HTTPS OTA Begin failed");
        return ESP_FAIL;
    }

    esp_app_desc_t app_desc;
    err = esp_https_ota_get_img_desc(https_ota_handle, &app_desc);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "esp_https_ota_read_img_desc failed");
        goto ota_end;
    }

    err = validate_image_header(&app_desc);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "image header verification failed");
        esp_https_ota_abort(https_ota_handle);
        return ESP_OK;
    }
    int previous_len = 0, pckt_retry_cnt = 0;
    while (1)
    {
        err = esp_https_ota_perform(https_ota_handle);
        if (err != ESP_ERR_HTTPS_OTA_IN_PROGRESS) {
            break;
        }
        // esp_https_ota_perform returns after every read operation which gives user the ability to
        // monitor the status of OTA upgrade by calling esp_https_ota_get_image_len_read, which gives length of image
        // data read so far.
        ESP_LOGE(TAG, "Image bytes read: %d", esp_https_ota_get_image_len_read(https_ota_handle));
        if(previous_len == esp_https_ota_get_image_len_read(https_ota_handle))
        	pckt_retry_cnt++;
        else
        	previous_len = esp_https_ota_get_image_len_read(https_ota_handle);
        if(pckt_retry_cnt >= MAX_FOTA_PACKET_RETRY)
        {
        	esp_https_ota_abort(https_ota_handle);
			ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed");
			return ESP_FAIL;
        }
    }

    if (esp_https_ota_is_complete_data_received(https_ota_handle) != true)
    {
        // the OTA image was not completely received and user can customise the response to this situation.
        ESP_LOGE(TAG, "Complete data was not received.");
    }
    else
    {
        ota_finish_err = esp_https_ota_finish(https_ota_handle);
        if ((err == ESP_OK) && (ota_finish_err == ESP_OK))
        {
            ESP_LOGI(TAG, "ESP_HTTPS_OTA upgrade successful. Rebooting ...");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            esp_restart();
        }
        else
        {
            if (ota_finish_err == ESP_ERR_OTA_VALIDATE_FAILED)
            {
                ESP_LOGE(TAG, "Image validation failed, image is corrupted");
            }
            ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed 0x%x", ota_finish_err);
            return ESP_FAIL;
        }
    }

ota_end:
    esp_https_ota_abort(https_ota_handle);
    ESP_LOGE(TAG, "ESP_HTTPS_OTA upgrade failed");
    return ESP_FAIL;
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
 * Function to Initiate SIM800
 */
esp_err_t initiate_gsm_module()
{
	uint8_t retry_count = 0, reset_count = 0;
start:	ESP_LOGI(TAG, "Initialising the GSM Module ");
	got_ip = false;
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
	ESP_ERROR_CHECK(dce->set_flow_ctrl(dce, MODEM_FLOW_CONTROL_NONE));
	ESP_ERROR_CHECK(dce->store_profile(dce));
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

		/* Get battery voltage */
//        uint32_t voltage = 0, bcs = 0, bcl = 0;
//        ESP_ERROR_CHECK(dce->get_battery_status(dce, &bcs, &bcl, &voltage));
//        ESP_LOGI(TAG, "Battery voltage: %d mV", voltage);


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
	EventBits_t bits = 0;
//	esp_netif_ip_info_t *ip_infor;
	while(!got_ip)
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
//		bits = xEventGroupWaitBits(event_group, CONNECT_BIT, pdTRUE, pdTRUE, portMAX_DELAY);
//		//ESP_LOGI(TAG, "Not got IP Address");
//		vTaskDelay(1000/portTICK_PERIOD_MS);

	return ESP_OK;

}

/*
 * Function to Initialize  http client
 */

void initalise_http_client(void)
{
	esp_http_client_config_t http_config = {
											 //.url = "https://dl.espressif.com/dl/package_esp32_index.json",
											 //.url = "https://httpbin.org/ip",
											 .url = "http://sandbox.itspe.co.in:9000/api/GetUpdateProgramData",
											 .method = HTTP_METHOD_GET,
											 .cert_pem = NULL,//(char *)server_cert_httpbin_pem_start,
											 .buffer_size= MAX_HTTP_OUTPUT_BUFFER,
											 .timeout_ms = MAX_HTTP_TIMOUT,
											 .event_handler = http_event_handler};

	client = esp_http_client_init(&http_config);
	esp_http_client_set_header(client, "device_id", "24:0A:C4:80:1F:68");
//	esp_http_client_handle_t client = esp_http_client_init(&http_config);
//	esp_http_client_perform(client);
	//esp_http_client_cleanup(client);
}


void app_main(void)
{
//#if CONFIG_LWIP_PPP_PAP_SUPPORT
//    esp_netif_auth_type_t auth_type = NETIF_PPP_AUTHTYPE_PAP;
//#elif CONFIG_LWIP_PPP_CHAP_SUPPORT
//    esp_netif_auth_type_t auth_type = NETIF_PPP_AUTHTYPE_CHAP;
//#elif !defined(CONFIG_EXAMPLE_MODEM_PPP_AUTH_NONE)
//#error "Unsupported AUTH Negotiation"
//#endif
	 //esp_err_t err;
	/*
	 * Setup the GPIO pins used by RESET Pin of SIM800
	 */
	gpio_reset_pin(SIM800_RESET_PIN);
	/* Set the GPIO as a push/pull output */
	gpio_set_direction(SIM800_RESET_PIN, GPIO_MODE_OUTPUT);
	gpio_set_level(SIM800_RESET_PIN, 1);
	reset_sim800();

	// Initialize NVS.
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		// 1.OTA app partition table has a smaller NVS partition size than the non-OTA
		// partition table. This size mismatch may cause NVS initialization to fail.
		// 2.NVS partition contains data in new format and cannot be recognized by this version of code.
		// If this happens, we erase NVS partition and initialize NVS again.
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}

    esp_netif_init();
    esp_event_loop_create_default();
    esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &on_ip_event, NULL);
    esp_event_handler_register(NETIF_PPP_STATUS, ESP_EVENT_ANY_ID, &on_ppp_changed, NULL);

    event_group = xEventGroupCreate();

    /* create dte object */
    esp_modem_dte_config_t config = ESP_MODEM_DTE_DEFAULT_CONFIG();
    /* setup UART specific configuration based on kconfig options */
    config.tx_io_num = 17;
    config.rx_io_num = 16;
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
    ESP_ERROR_CHECK(esp_modem_set_event_handler(dte, modem_event_handler, ESP_EVENT_ANY_ID, NULL));

    // Init netif object
    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_PPP();
    esp_netif = esp_netif_new(&cfg);
    assert(esp_netif);

    modem_netif_adapter = esp_modem_netif_setup(dte);
    esp_modem_netif_set_default_handlers(modem_netif_adapter, esp_netif);


	if(initiate_gsm_module()==ESP_FAIL)
	{
		ESP_LOGI(TAG, "Reseting ESP-32");
		esp_restart();
	}

	fota_retry_count = 0;
	while((get_update()!=ESP_OK)&&(fota_retry_count<MAX_FOTA_RETRY_COUNT))
	{
		fota_retry_count++;
		 vTaskDelay(500);
	}
	initalise_http_client();
	http_rq_retry_count= 0;
	while(1)
	{
		http_rq_fail = false;
		err = esp_http_client_perform(client);
		if(http_rq_fail)
		{
			http_rq_retry_count++;
//			if(esp_netif_is_netif_up(esp_netif)==true)
//			{
//				 ESP_LOGI(TAG, "Network Interface is Up");
//			}
			if(http_rq_retry_count>=MAX_HTTPS_RETRY_COUNT)
			{
				ESP_LOGI(TAG, "Maximum Time Failed Reseting SIM800");
				reset_sim800();
				if(initiate_gsm_module()==ESP_FAIL)
				{

					ESP_LOGI(TAG, "Reseting ESP-32");
					esp_restart();
				}
				esp_modem_netif_clear_default_handlers(modem_netif_adapter);
				initalise_http_client();
			}
			ESP_LOGI(TAG, "Failed to get Responce");
		}
		else
		{
			http_rq_retry_count = 0;
		}
	   esp_http_client_close(client);
	   vTaskDelay(2000/portTICK_PERIOD_MS);
	}

    //Clear Http Client
    esp_http_client_cleanup(client);

    /* Unregister events, destroy the netif adapter and destroy its esp-netif instance */
    esp_modem_netif_clear_default_handlers(modem_netif_adapter);
    esp_modem_netif_teardown(modem_netif_adapter);
    esp_netif_destroy(esp_netif);

    ESP_ERROR_CHECK(dte->deinit(dte));
}
