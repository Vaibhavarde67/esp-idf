/*
 * fota.c
 *
 *  Created on: Feb 17, 2023
 *      Author: Embin 7
 */

#include "fota.h"

static const char *TAG = "FOTA";

uint8_t fota_retry_count;

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
    double crnt_frm_ver = strtod_l(running_app_info.version, NULL,NULL);
    double new_frm_ver = strtod_l(new_app_info->version, NULL,NULL);

    if (new_frm_ver < crnt_frm_ver)
    {
        ESP_LOGW(TAG, "New firmware Version is less than Current Firmware Version, %f < %f", new_frm_ver, crnt_frm_ver);
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
esp_err_t get_update(void)
{
    ESP_LOGE(TAG, "Starting Advanced OTA example");
    esp_err_t ota_finish_err = ESP_OK;
    esp_http_client_config_t config = {
        .url = "https://itms.smarttvm.in:3035/pis/firmware/SBSD-1715/v1.0.0/SBSD-1715.bin",
        .cert_pem = NULL,
        .timeout_ms = 10000,
        .keep_alive_enable = true,
    };


    config.skip_cert_common_name_check = true;

    esp_https_ota_config_t ota_config =
    {
        .http_config = &config,
        .http_client_init_cb = _http_client_init_cb, // Register a callback to be invoked after esp_http_client is initialized
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
    //Added on 22/08/2023
    timer_pause(DIS_TIMER_GRP, DIS_TIMER_ID);
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
        	display_Message(FOTA_SUCESS);
			ESP_LOGI(TAG, "ESP_HTTPS_OTA upgrade successful. Rebooting ...");
			vTaskDelay(5000 / portTICK_PERIOD_MS);
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


