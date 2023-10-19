/*
 * ethrnet.h
 *
 *  Created on: Feb 9, 2023
 *      Author: Embin 7
 */



#ifndef MAIN_ETHRNET_H_
#define MAIN_ETHRNET_H_

#include "esp_netif.h"
#include "esp_eth.h"
#include "esp_eth_enc28j60.h"
#include "driver/spi_master.h"
#include "esp_tls.h"
#include "lwip/err.h"
#include "driver/gpio.h"

#include "esp_event.h"
#include "esp_log.h"

#include "define.h"

#define ENC28J60_SPI_HOST 2
#define ENC28J60_SCLK_GPIO 4//19
#define ENC28J60_MOSI_GPIO 15//23
#define ENC28J60_MISO_GPIO 5//25
#define ENC28J60_CS_GPIO 2//22
#define ENC28J60_SPI_CLOCK_MHZ 6
#define ENC28J60_INT_GPIO 18//4

#define MAX_ETHERNET_RETRY_COUNT	20


bool NET_CONNECTED;

esp_netif_t *eth_netif;

/*
 * Function to Initailize the Ethernet
 */
esp_err_t enc28j60_init();



#endif /* MAIN_ETHRNET_H_ */
