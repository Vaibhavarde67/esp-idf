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


#define EXAMPLE_STATIC_IP_ADDR        "172.20.23.215"
#define EXAMPLE_STATIC_NETMASK_ADDR   "255.255.248.0"
#define EXAMPLE_STATIC_GW_ADDR        "172.20.16.1"
#define EXAMPLE_MAIN_DNS_SERVER       "8.8.8.8"
#define EXAMPLE_BACKUP_DNS_SERVER     "172.20.1.11"

//#define EXAMPLE_STATIC_IP_ADDR        "192.168.1.67"
//#define EXAMPLE_STATIC_NETMASK_ADDR   "255.255.255.0"
//#define EXAMPLE_STATIC_GW_ADDR        "192.168.1.1"
//#define EXAMPLE_MAIN_DNS_SERVER       "8.8.8.8"
//#define EXAMPLE_BACKUP_DNS_SERVER     "192.168.1.1"


bool NET_CONNECTED;

esp_netif_t *eth_netif;

/*
 * Function to Initailize the Ethernet
 */
esp_err_t enc28j60_init();



#endif /* MAIN_ETHRNET_H_ */
