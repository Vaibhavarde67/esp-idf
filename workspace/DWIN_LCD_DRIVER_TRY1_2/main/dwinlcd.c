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



/*
 * Function to handel UART Interrupt for MODBUS Port
 */
static void IRAM_ATTR mod_uart_intr_handle(void *arg)
{
	//ESP_LOGI(TAG," IN UART Interrupt\n");
	uint16_t rx_fifo_len;

	// read number of bytes in UART buffer
	rx_fifo_len = DWIN_LCD_UART_No.status.rxfifo_cnt;

	while(rx_fifo_len)
	{
		rx_frame[rx_frame_len] = DWIN_LCD_UART_No.fifo.rw_byte; // read all bytes
		//ets_printf("0x%02X",rx_frame[rx_frame_len]);
		rx_fifo_len--;

		if((!frame_start)&&(rx_frame_len==1))
		{
			if((rx_frame[0]!=0x5A)&&(rx_frame[1]!=0xA5))
			{
				rx_frame_len = 0;
				bzero(rx_frame, MAX_FRAM_SIZE);
			}
			else
				frame_start = 1;
		}
		if((frame_start)&&(rx_frame[2]==rx_frame_len-2)&&(rx_frame_len>4))
		{
			frame_start = 0;
			struct dwin_frame recevied_dwin_frametemp;
			//recevied_dwin_frametemp = (void *)malloc(sizeof(struct dwin_frame));

			//ets_printf("\nDwin Frame received\n");
			//recevied_dwin_frametemp.ptr_addr = recevied_dwin_frametemp;
			recevied_dwin_frametemp.data_length = rx_frame[2];
			recevied_dwin_frametemp.instruction = rx_frame[3];
			recevied_dwin_frametemp.VP_addr = (rx_frame[4]<<8) | rx_frame[5];
			for(uint i = 0; i<recevied_dwin_frametemp.data_length-3; i++)
			{
				recevied_dwin_frametemp.data[i] = rx_frame[i+6];
				//ets_printf("%x",recevied_dwin_frametemp.data[i]);
			}
			//Extracting RKC
			if((recevied_dwin_frametemp.VP_addr>0x1000)&&(recevied_dwin_frametemp.VP_addr<0x1050))
			{
				//ets_printf("Extracting RKC\n");
				recevied_dwin_frametemp.RKC = (rx_frame[7]<<8) | rx_frame[8];
			}

//			ets_printf("\n Received Data Length = %d\n",recevied_dwin_frametemp->data_length);
//			ets_printf("Received Instruction = %x\n",recevied_dwin_frametemp->instruction);
//			ets_printf("Received VP Address = %x\n",recevied_dwin_frametemp->VP_addr);

			xQueueSend(dwin_frame_queu, &recevied_dwin_frametemp, portMAX_DELAY);

			rx_frame_len = 0;
			bzero(rx_frame, MAX_FRAM_SIZE);
		}
		rx_frame_len++;

	}




	// after reading bytes from buffer clear UART interrupt status
	//uart_clear_intr_status(MOD_UART, UART_RXFIFO_FULL_INT_CLR|UART_RXFIFO_TOUT_INT_CLR);
	uart_clear_intr_status(DWIN_LCD_UART, UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_OVF_INT_CLR);
}


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

	//Install UART Deriver
	uart_driver_install(DWIN_LCD_UART, RX_BUF_SIZE * 2, 0 , 0, NULL, 0);

	//Assigne Configuration parameter to UART
	uart_param_config(DWIN_LCD_UART, &uart_config);

	//Assigne Pin for UART Port that we are using for MODBUS
	uart_set_pin(DWIN_LCD_UART, DWIN_LCD_TX_PIN, DWIN_LCD_RX_PIN, -1, -1);

	/* RX interrupt Configuration
	* We will have interrupt for each character so we kept threshold 1
	*/
	//ESP_INTR_FLAG_IRAM
	//MOD_UART_INTR_CONFIG_FLAG
	uart_intr_config_t uart_intr = {
									.intr_enable_mask = DWIN_LCD_UART_INTR_CONFIG_FLAG,
									.rxfifo_full_thresh = 1,
									.rx_timeout_thresh = 0,
									.txfifo_empty_intr_thresh = 0,
								   };
	uart_intr_config(DWIN_LCD_UART,&uart_intr);

	// release the pre registered UART handler/subroutine
	uart_isr_free(DWIN_LCD_UART);

	// register new UART subroutine
	uart_isr_register(DWIN_LCD_UART,mod_uart_intr_handle, 0, 0, &handle_console);


	//enable RX interrupt
	uart_enable_rx_intr(DWIN_LCD_UART);



	return ESP_OK;

}



