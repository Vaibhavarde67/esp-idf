/*
 * ethrnet.c
 *
 *  Created on: Feb 9, 2023
 *      Author: Embin 7
 */


#include "ethrnet.h"

static const char *TAG = "Ethernet";



static esp_err_t example_set_dns_server(esp_netif_t *netif, uint32_t addr, esp_netif_dns_type_t type)
{
    if (addr && (addr != IPADDR_NONE)) {
        esp_netif_dns_info_t dns;
        dns.ip.u_addr.ip4.addr = addr;
        dns.ip.type = IPADDR_TYPE_V4;
        ESP_ERROR_CHECK(esp_netif_set_dns_info(netif, type, &dns));
    }
    return ESP_OK;
}


static void set_static_ip(esp_netif_t *netif)
{
    if (esp_netif_dhcpc_stop(netif) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop dhcp client");
        return;
    }
    esp_netif_ip_info_t ip;
    memset(&ip, 0 , sizeof(esp_netif_ip_info_t));
    ip.ip.addr = ipaddr_addr(EXAMPLE_STATIC_IP_ADDR);
    ip.netmask.addr = ipaddr_addr(EXAMPLE_STATIC_NETMASK_ADDR);
    ip.gw.addr = ipaddr_addr(EXAMPLE_STATIC_GW_ADDR);
    if (esp_netif_set_ip_info(netif, &ip) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set ip info");
        return;
    }
    ESP_LOGD(TAG, "Success to set static ip: %s, netmask: %s, gw: %s", EXAMPLE_STATIC_IP_ADDR, EXAMPLE_STATIC_NETMASK_ADDR, EXAMPLE_STATIC_GW_ADDR);
    ESP_ERROR_CHECK(example_set_dns_server(netif, ipaddr_addr(EXAMPLE_MAIN_DNS_SERVER), ESP_NETIF_DNS_MAIN));
    ESP_ERROR_CHECK(example_set_dns_server(netif, ipaddr_addr(EXAMPLE_BACKUP_DNS_SERVER), ESP_NETIF_DNS_BACKUP));
}


/** Event handler for Ethernet events */
static void eth_event_handler(void *arg, esp_event_base_t event_base,
                              int32_t event_id, void *event_data)
{
    uint8_t mac_addr[6] = {0};
    /* we can get the ethernet driver handle from event data */
    esp_eth_handle_t eth_handle = *(esp_eth_handle_t *)event_data;

    switch (event_id) {
    case ETHERNET_EVENT_CONNECTED:
        esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
        ESP_LOGI(TAG, "Ethernet Link Up");
        ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
        set_static_ip(eth_netif);
        break;
    case ETHERNET_EVENT_DISCONNECTED:
    	gpio_set_level(STS_LED1_PIN, 1);
        ESP_LOGI(TAG, "Ethernet Link Down");
        NET_CONNECTED = FALSE;
        break;
    case ETHERNET_EVENT_START:
        ESP_LOGI(TAG, "Ethernet Started");
        break;
    case ETHERNET_EVENT_STOP:
    	gpio_set_level(STS_LED1_PIN, 1);
        ESP_LOGI(TAG, "Ethernet Stopped");
        NET_CONNECTED = FALSE;
        break;
    default:
        break;
    }
}


/** Event handler for IP_EVENT_ETH_GOT_IP */

static void got_ip_event_handler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data)
{
    ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
    const esp_netif_ip_info_t *ip_info = &event->ip_info;

    gpio_set_level(STS_LED1_PIN, 0);
    ESP_LOGI(TAG, "Ethernet Got IP Address");
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    ESP_LOGI(TAG, "ETHIP:" IPSTR, IP2STR(&ip_info->ip));
    ESP_LOGI(TAG, "ETHMASK:" IPSTR, IP2STR(&ip_info->netmask));
    ESP_LOGI(TAG, "ETHGW:" IPSTR, IP2STR(&ip_info->gw));
    ESP_LOGI(TAG, "~~~~~~~~~~~");
    NET_CONNECTED = TRUE;
}



/*
 * Function to Initialize Ethernet Module
 */
esp_err_t enc28j60_init()
{
	esp_err_t err;
	NET_CONNECTED = FALSE;
	// Initialize TCP/IP network interface (should be called only once in application)
	err = esp_netif_init();
	if(err != ESP_OK)
		return err;

	esp_netif_config_t netif_cfg = ESP_NETIF_DEFAULT_ETH();
	//esp_netif_t *eth_netif = esp_netif_new(&netif_cfg);
	eth_netif = esp_netif_new(&netif_cfg);
	// Set default handlers to process TCP/IP stuffs
	err = esp_eth_set_default_handlers(eth_netif);
	if(err != ESP_OK)
			return err;
	// Register user defined event handers
	esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL);
	esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL);

	spi_bus_config_t buscfg = {
		.miso_io_num = ENC28J60_MISO_GPIO,
		.mosi_io_num = ENC28J60_MOSI_GPIO,
		.sclk_io_num = ENC28J60_SCLK_GPIO,
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
	};

	spi_bus_initialize(ENC28J60_SPI_HOST, &buscfg, 1);
	/* ENC28J60 ethernet driver is based on spi driver */
	spi_device_interface_config_t devcfg = {
		.command_bits = 3,
		.address_bits = 5,
		.mode = 0,
		.clock_speed_hz = ENC28J60_SPI_CLOCK_MHZ * 1000 * 1000,
		.spics_io_num = ENC28J60_CS_GPIO,
		.queue_size = 20
	};
	spi_device_handle_t spi_handle = NULL;
	spi_bus_add_device(ENC28J60_SPI_HOST, &devcfg, &spi_handle);

	eth_enc28j60_config_t enc28j60_config = ETH_ENC28J60_DEFAULT_CONFIG(spi_handle);
	enc28j60_config.int_gpio_num = ENC28J60_INT_GPIO;

	eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
	mac_config.smi_mdc_gpio_num = -1;  // ENC28J60 doesn't have SMI interface
	mac_config.smi_mdio_gpio_num = -1;
	esp_eth_mac_t *mac = esp_eth_mac_new_enc28j60(&enc28j60_config, &mac_config);

	eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
	phy_config.autonego_timeout_ms = 0; // ENC28J60 doesn't support auto-negotiation
	phy_config.reset_gpio_num = -1; // ENC28J60 doesn't have a pin to reset internal PHY
	esp_eth_phy_t *phy = esp_eth_phy_new_enc28j60(&phy_config);

	esp_eth_config_t eth_config = ETH_DEFAULT_CONFIG(mac, phy);
	esp_eth_handle_t eth_handle = NULL;
	//printf("Here2\n");
	ESP_ERROR_CHECK(esp_eth_driver_install(&eth_config, &eth_handle));
	if(err != ESP_OK)
			return err;
	/* ENC28J60 doesn't burn any factory MAC address, we need to set it manually.
	   02:00:00 is a Locally Administered OUI range so should not be used except when testing on a LAN under your control.
	*/
	mac->set_addr(mac, (uint8_t[]) {
		0x02, 0x00, 0x00, 0x12, 0x34, 0x56
	});

	/* attach Ethernet driver to TCP/IP stack */
	esp_netif_attach(eth_netif, esp_eth_new_netif_glue(eth_handle));
	/* start Ethernet driver state machine */
	esp_eth_start(eth_handle);
	return ESP_OK;
}
