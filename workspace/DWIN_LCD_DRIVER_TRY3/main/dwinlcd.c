/*
 * modbus.c
 *
 *  Created on: 19-Aug-2022
 *      Author: Embin 7
 */
#include "dwinlcd.h"


static const char *TAG = "DWIN_UART_DRIVER";


//
//modb_err_t modb_read_holding_register(uint8_t slave_adr, uint16_t reg_addr, uint16_t quant)
//{
//	uint8_t frame_len = 8;									//as Tx frame will only have 8 bytes
//	uint8_t *tx_frame = (uint8_t*) malloc(frame_len);
//	tx_frame[0] = slave_adr;						//Slave Address
//	tx_frame[1]	= 0x03;								//Function code
//	tx_frame[2] = (uint8_t)(reg_addr>>8);			//Starting Register address higher byte
//	tx_frame[3] = (uint8_t)(reg_addr & 0X00FF);		//Starting Register address lower byte
//	tx_frame[4] = (uint8_t)(quant>>8);				//Quantity higher byte
//	tx_frame[5] = (uint8_t)(quant & 0X00FF);		//Quantity lower byte
//	add_crc_to_buff(tx_frame, frame_len);			//Add Crc to frame
//	//Now Send frame and analyse responce
//	bzero(rx_frame, MAX_FRAM_SIZE);					//Clear Rx frame to receive responce
//	MasterSendEnable();
//	//Activate Transsmission then transmit packet
//	if(uart_write_bytes(IO_BOARD_MOD_UART, tx_frame, frame_len)<0)
//		return MODB_FAIL;
//	ets_delay_us(frame_len*86.8);
//	//Free the Tx Buffer space
//	free(tx_frame);
//	MasterRecevdEnable();
//	return(wait_and_validate_responce(slave_adr, 0X03));
//}




/*
 * Function to validate frame received
 */
//modb_err_t wait_and_validate_responce(uint8_t slave_adr, uint8_t fn_code)
//{
//	uint16_t rec_crc = 0, cal_crc = 0;
//	if((xSemaphoreTake(raw_frame_received, FRAME_END_TM)))
//	{
//		rec_crc = rx_frame[rx_frame_len-1];		//as index start from 0
//		rec_crc = rec_crc<<8 | rx_frame[rx_frame_len-2];
//		cal_crc = cal_crc16(&rx_frame[0], rx_frame_len-2);
//		if(rec_crc == cal_crc)
//		{
//			if(slave_adr != rx_frame[0])
//				return MODB_ERR_INVALID_DATA;
//			if(rx_frame[1] == fn_code)
//			{
//				return MODB_OK;
//			}
//			else
//				return MODB_ERR_ERROR_CODE_REC;
//
//
//		}
//		else
//			return MODB_ERR_INVALID_CRC;
//	}
//	else
//		return MODB_ERR_TIMEOUT;
//}




esp_err_t dwin_lcd_init()
{
	frame_start = 0;
	rx_frame_len = 0;
	bzero(rx_frame, MAX_FRAM_SIZE);		//Clear the RX

	//Creating Queue for Dwin Frame
	dwin_frame_queu = xQueueCreate(5, sizeof(recevied_dwin_frame));
	//ets_printf("%d\n\n",sizeof(recevied_dwin_frame));


	/*
	* initializing UART required for MODBUS
	*/
	uart_config_t uart_config = {
				       	   	   	 .baud_rate = DWIN_LCD_BAUD,
								 .data_bits = UART_DATA_8_BITS,
								 .parity = UART_PARITY_DISABLE,
								 .stop_bits = UART_STOP_BITS_1,
								 .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
								 .source_clk = UART_SCLK_APB,
				    			};

	//Assigne Configuration parameter to UART
	uart_param_config(DWIN_LCD_UART, &uart_config);

	//Assigne Pin for UART Port that we are using for MODBUS
	uart_set_pin(DWIN_LCD_UART, DWIN_LCD_TX_PIN, DWIN_LCD_RX_PIN, -1, -1);

	//Install UART Deriver
	uart_driver_install(DWIN_LCD_UART, RX_BUF_SIZE * 2, 0 , 20, &uart0_queue, 0);

	return ESP_OK;

}



