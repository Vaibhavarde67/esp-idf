/*
 * fota.h
 *
 *  Created on: Feb 17, 2023
 *      Author: Embin 7
 */


#ifndef MAIN_FOTA_H_
#define MAIN_FOTA_H_

#include "esp_efuse.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_ota_ops.h"
#include "esp_https_ota.h"
#include "lwip/err.h"
#include "esp_log.h"

#include "define.h"
#include "constTextDisp.h"

#define OTA_URL_SIZE 256

#define MAX_FOTA_RETRY_COUNT	5
#define MAX_FOTA_PACKET_RETRY	60

/*
 * Function Responsible for performing Update
 */
esp_err_t get_update(void);


#endif /* MAIN_FOTA_H_ */
