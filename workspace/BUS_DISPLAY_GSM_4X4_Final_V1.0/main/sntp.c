/*
 * sntp.c
 *
 *  Created on: Feb 16, 2023
 *      Author: Embin 7
 */

#include "sntp.h"


static const char *TAG = "SNTP";

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
    sntp_init();
}


esp_err_t obtain_time(void)
{


    initialize_sntp();
    //setenv("IST", "UTC + 5:30", 253);

    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    //const int retry_count = 20;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < MAX_SNTP_RETRY_LIMIT)
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, MAX_SNTP_RETRY_LIMIT);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    if(sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && retry==MAX_SNTP_RETRY_LIMIT)
    	return ESP_FAIL;
    now += 19800;
    time(&now);
    localtime_r(&now, &timeinfo);
    return ESP_OK;

}

void print_current_time(void)
{
   char strftime_buf[64];
   time_t now = 0;
   struct tm timeinfo = { 0 };
   time(&now);
   localtime_r(&now, &timeinfo);
   strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);

   ESP_LOGI(TAG, "The current date/time in INDIA is: %s", strftime_buf);
}



