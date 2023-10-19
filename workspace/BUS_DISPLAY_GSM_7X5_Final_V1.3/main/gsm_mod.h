/*
 * gsm_mod.h
 *
 *  Created on: Feb 27, 2023
 *      Author: Embin 7
 */

#ifndef MAIN_GSM_MOD_H_
#define MAIN_GSM_MOD_H_

#include "esp_netif.h"
#include "esp_netif_ppp.h"
#include "esp_modem.h"
#include "esp_modem_netif.h"
#include "esp_log.h"
#include "sim800.h"
#include "driver/gpio.h"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "lwip/err.h"

#include "esp_modem_dce_service.h"
#include "define.h"


#define MAX_SIM800_RETRY_COUNT	8
#define MAX_SIM800_RESET_COUNT	0


#define	SIM800_RESET_PIN	22
#define SIM800_RX_PIN	17
#define SIM800_TX_PIN	16

//Variable for GSM Module
modem_dte_t *dte;
modem_dce_t *dce;
esp_netif_t *esp_netif;
void *modem_netif_adapter;


bool NET_CONNECTED;

/*
 * Function to initialize the Pins for GSM module
 */
void setup_gsm_module(void);

/*
 * Function to Reset the SIM800 module
 */
void reset_sim800();

/*
 * Function to Initiate SIM800
 */
esp_err_t initiate_gsm_module();



#endif /* MAIN_GSM_MOD_H_ */
