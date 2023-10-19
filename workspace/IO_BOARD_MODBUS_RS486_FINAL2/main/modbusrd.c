/*
 * modbus.c
 *
 *  Created on: 19-Aug-2022
 *      Author: Embin 7
 */
#include <stdbool.h>
#include <unistd.h>
#include <string.h>


#include "driver/uart.h"
#include "soc/uart_reg.h"
#include "soc/uart_struct.h"
#include "esp_intr_alloc.h"

#include "freertos/semphr.h"
#include "freertos/task.h"


#include "driver/timer.h"
#include "lwip/err.h"

#include "esp_log.h"

#include "modbusrd.h"

static const char *TAG = "IO_BOARD_MODBUS";

/*
 * static uart_isr_handle_t *handle_console;
 */
static intr_handle_t handle_console;

/*
 * Defining Semaphore for detecting wether frame received or not
 */
SemaphoreHandle_t raw_frame_received = NULL;


//Flag to detect start of frame
bool frame_start;

/*
void modbus_req_frame(uint8_t slaveId)
{

	uint8_t frame_make[10];
    NRxData1=0;
	memset(RxBuff1, 0, MODBUS_FRAME_LEN);
	//NRxData1=0;
	Delay_ms(3);

	frame_make[0] = slaveId;//MODBUS_DEFAULT_ADDRESS;
	frame_make[1] = 0x03;
	frame_make[2] = 0x00;
	frame_make[3] = 0x00;
	frame_make[4] = 0x00;
	frame_make[5] = 0x10;

	switch(DEVICE_BAUDRATE)
	{
		case MODBUS_BAUDRATE_0:
			Delay_ms(5);
		break;
		case MODBUS_BAUDRATE_1:
			Delay_ms(4);
		break;

		case MODBUS_BAUDRATE_2:
			Delay_ms(3);
		break;

 		case MODBUS_BAUDRATE_3:
			Delay_ms(1);
		break;

		case MODBUS_BAUDRATE_6:
			Delay_ms(6);
     break;
	}

	modbus_send_frame(frame_make,8);
	Delay_ms(8);//5


}
*/

void MasterSendEnable(void)
{
	gpio_set_level(IO_BOARD_COM_CONTROL_PIN, SET);
}


void MasterRecevdEnable(void)
{
	gpio_set_level(IO_BOARD_COM_CONTROL_PIN, RESET);
}

void ConfigControlPin(void)
{
	gpio_reset_pin(IO_BOARD_COM_CONTROL_PIN);
	gpio_set_direction(IO_BOARD_COM_CONTROL_PIN, GPIO_MODE_OUTPUT);
}

modb_err_t modb_read_input_register(uint8_t slave_adr, uint16_t reg_addr, uint16_t quant)
{
	uint8_t frame_len = 8;									//as Tx frame will only have 8 bytes
uint8_t *tx_frame = (uint8_t*) malloc(frame_len);
	tx_frame[0] = slave_adr;						//Slave Address
	tx_frame[1]	= 0x04;								//Function code
	tx_frame[2] = (uint8_t)(reg_addr>>8);			//Starting Register address higher byte
	tx_frame[3] = (uint8_t)(reg_addr & 0X00FF);		//Starting Register address lower byte
	tx_frame[4] = (uint8_t)(quant>>8);				//Quantity higher byte
	tx_frame[5] = (uint8_t)(quant & 0X00FF);		//Quantity lower byte
	add_crc_to_buff(tx_frame, frame_len);			//Add Crc to frame
	//Now Send frame and analyse responce
	bzero(rx_frame, MAX_FRAM_SIZE);					//Clear Rx frame to receive responce
	//Activate Transsmission then transmit packet
	MasterSendEnable();
	if(uart_write_bytes(IO_BOARD_MOD_UART, tx_frame, frame_len)<0)
		return MODB_FAIL;

	ets_delay_us(frame_len*86.8);
	//Free the Tx Buffer space
	free(tx_frame);
	MasterRecevdEnable();
	//gpio_reset_pin(COM_CONTROL_PIN);
	return(wait_and_validate_responce(slave_adr, 0X04));
}

modb_err_t modb_read_holding_register(uint8_t slave_adr, uint16_t reg_addr, uint16_t quant)
{
	uint8_t frame_len = 8;									//as Tx frame will only have 8 bytes
	uint8_t *tx_frame = (uint8_t*) malloc(frame_len);
	tx_frame[0] = slave_adr;						//Slave Address
	tx_frame[1]	= 0x03;								//Function code
	tx_frame[2] = (uint8_t)(reg_addr>>8);			//Starting Register address higher byte
	tx_frame[3] = (uint8_t)(reg_addr & 0X00FF);		//Starting Register address lower byte
	tx_frame[4] = (uint8_t)(quant>>8);				//Quantity higher byte
	tx_frame[5] = (uint8_t)(quant & 0X00FF);		//Quantity lower byte
	add_crc_to_buff(tx_frame, frame_len);			//Add Crc to frame
	//Now Send frame and analyse responce
	bzero(rx_frame, MAX_FRAM_SIZE);					//Clear Rx frame to receive responce
	MasterSendEnable();
	//Activate Transsmission then transmit packet
	if(uart_write_bytes(IO_BOARD_MOD_UART, tx_frame, frame_len)<0)
		return MODB_FAIL;
	ets_delay_us(frame_len*86.8);
	//Free the Tx Buffer space
	free(tx_frame);
	MasterRecevdEnable();
	return(wait_and_validate_responce(slave_adr, 0X03));
}


modb_err_t modb_write_single_coil(uint8_t slave_adr, uint16_t coil_addr, bool on_off)
{
	uint8_t frame_len = 8;									//as Tx frame will only have 8 bytes
	uint8_t* tx_frame = (uint8_t*) malloc(frame_len);
	tx_frame[0] = slave_adr;							//Slave Address
	tx_frame[1]	= 0x05;									//Function code
	tx_frame[2] = (uint8_t)(coil_addr>>8);				//Coil address higher byte
	tx_frame[3] = (uint8_t)(coil_addr & 0X00FF);		//Coil address lower byte
	if(on_off)			//ON
	{
		tx_frame[4] = 0XFF;
		tx_frame[5] = 0X00;
	}
	else				//OFF
	{
		tx_frame[4] = 0X00;
		tx_frame[5] = 0X00;
	}
	add_crc_to_buff(tx_frame, frame_len);					//Add Crc to frame
	MasterSendEnable();
	//Now send frame and analyse responce
	if(uart_write_bytes(IO_BOARD_MOD_UART, tx_frame, frame_len)<0)
			return MODB_FAIL;
	ets_delay_us(frame_len*86.8);
	//Free the Tx Buffer spae
	free(tx_frame);
	MasterRecevdEnable();
	return(wait_and_validate_responce(slave_adr, 0X05));
}


modb_err_t modb_write_single_reg(uint8_t slave_adr, uint16_t reg_addr, uint16_t data)
{
	uint8_t frame_len = 8;									//as Tx frame will only have 8 bytes
	uint8_t* tx_frame = (uint8_t*) malloc(frame_len);
	tx_frame[0] = slave_adr;							//Slave Address
	tx_frame[1]	= 0x06;									//Function code
	tx_frame[2] = (uint8_t)(reg_addr>>8);					//Register address higher byte
	tx_frame[3] = (uint8_t)(reg_addr & 0X00FF);			//Register address lower byte
	tx_frame[4] = (uint8_t)(data>>8);						//Register Data higher byte
	tx_frame[5] = (uint8_t)(data & 0X00FF);				//Register Data lower byte
	add_crc_to_buff(tx_frame, frame_len);						//Add CRC to frame
	MasterSendEnable();
	//Now send frame and analyse responce
	if(uart_write_bytes(IO_BOARD_MOD_UART, tx_frame, frame_len)<0)
			return MODB_FAIL;
	ets_delay_us(frame_len*86.8);
	//Free the Tx Buffer space
	free(tx_frame);
	MasterRecevdEnable();
	return(wait_and_validate_responce(slave_adr, 0X06));

}


modb_err_t modb_write_multiple_coils(uint8_t slave_adr, uint16_t coil_addr, uint16_t quant, uint8_t *data)
{
	uint8_t byte_cn = quant/8;
	if(quant%8>0)
		byte_cn++;
	uint8_t frame_len = 9 + byte_cn;						//as Tx frame will only have 8 bytes
	uint8_t* tx_frame = (uint8_t*) malloc(frame_len);
	tx_frame[0] = slave_adr;								//Slave Address
	tx_frame[1]	= 0x0F;										//Function code
	tx_frame[2] = (uint8_t)(coil_addr>>8);					//Starting Coil address higher byte
	tx_frame[3] = (uint8_t)(coil_addr & 0X00FF);			//Starting Coil address lower byte
	tx_frame[4] = (uint8_t)(quant>>8);						//Quantity Data higher byte
	tx_frame[5] = (uint8_t)(quant & 0X00FF);				//Quantity Data lower byte
	tx_frame[6] = byte_cn;									//Byte Count
	for(uint8_t i = 0; i<byte_cn;i++)
	{
		tx_frame[7+i] = data[i];
	}
	add_crc_to_buff(tx_frame, frame_len);						//Add CRC to frame
	MasterSendEnable();
	//Now send frame and analyse responce
	if(uart_write_bytes(IO_BOARD_MOD_UART, tx_frame, frame_len)<0)
			return MODB_FAIL;
	ets_delay_us(frame_len*86.8);
	//Free the Tx Buffer space
	free(tx_frame);
	MasterRecevdEnable();
	return(wait_and_validate_responce(slave_adr, 0X0F));
}


modb_err_t modb_write_multiple_regs(uint8_t slave_adr, uint16_t reg_addr, uint16_t quant, uint8_t *data)
{
	uint8_t byte_cn = quant*2;								//byte count field
	uint8_t frame_len = 9 + byte_cn;					//as Tx frame will only have 8 bytes
	uint8_t* tx_frame = (uint8_t*) malloc(frame_len);
	tx_frame[0] = slave_adr;								//Slave Address
	tx_frame[1]	= 0x10;										//Function code
	tx_frame[2] = (uint8_t)(reg_addr>>8);						//Starting Register address higher byte
	tx_frame[3] = (uint8_t)(reg_addr & 0X00FF);				//Starting Register address lower byte
	tx_frame[4] = (uint8_t)(quant>>8);						//Quantity Data higher byte
	tx_frame[5] = (uint8_t)(quant & 0X00FF);				//Quantity Data lower byte
	tx_frame[6] = byte_cn;									//Byte Count
	for(uint8_t i = 0; i<byte_cn;i++)
	{
		tx_frame[7+i] = data[i];
	}
	add_crc_to_buff(tx_frame, frame_len);					//Add CRC to frame
	MasterSendEnable();
	//Now send frame and analyse responce
	if(uart_write_bytes(IO_BOARD_MOD_UART, tx_frame, frame_len)<0)
			return MODB_FAIL;

	ets_delay_us(frame_len*86.8);
//	if(uart_wait_tx_done(MOD_UART, 500)<0)
//	{
//		ets_printf("Error while waiting for transfer completer\n");
//		return MODB_FAIL;
//	}
	//Free the Tx Buffer space
	free(tx_frame);
	MasterRecevdEnable();
	return(wait_and_validate_responce(slave_adr, 0X10));
}



/*
 * Function to validate frame received
 */
modb_err_t wait_and_validate_responce(uint8_t slave_adr, uint8_t fn_code)
{
	uint16_t rec_crc = 0, cal_crc = 0;
	if((xSemaphoreTake(raw_frame_received, FRAME_END_TM)))
	{
		rec_crc = rx_frame[rx_frame_len-1];		//as index start from 0
		rec_crc = rec_crc<<8 | rx_frame[rx_frame_len-2];
		cal_crc = cal_crc16(&rx_frame[0], rx_frame_len-2);
		if(rec_crc == cal_crc)
		{
			if(slave_adr != rx_frame[0])
				return MODB_ERR_INVALID_DATA;
			if(rx_frame[1] == fn_code)
			{
				return MODB_OK;
			}
			else
				return MODB_ERR_ERROR_CODE_REC;


		}
		else
			return MODB_ERR_INVALID_CRC;
	}
	else
		return MODB_ERR_TIMEOUT;
}


/*
 * Function to Handel Timer Interrupt for MODBUS Frame
 */
static void IRAM_ATTR mod_frame_timer_callback(void *args)
{
	//ESP_DRAM_LOGI(TAG, "In Timer Interrupt ");		//ESP_DRAM_LOGx uae to print logging information from ISR
	timer_pause(TIMER_GRP, TIMER_ID);
	//Clear the Timer Interrupt Status and Flag bits
	timer_group_clr_intr_status_in_isr(TIMER_GRP, TIMER_ID);
	//Disable the Timer interrupt
	timer_disable_intr(TIMER_GRP, TIMER_ID);
	xSemaphoreGiveFromISR(raw_frame_received,pdFALSE);
	frame_start = true;

}

/*
 * Function to handel UART Interrupt for MODBUS Port
 */
static void IRAM_ATTR mod_uart_intr_handle(void *arg)
{
	//ESP_LOGI(TAG," IN UART Interrupt\n");
	uint16_t rx_fifo_len;
	//Reset Timer
	timer_set_counter_value(TIMER_GRP, TIMER_ID, 0);
	//If it is frame Start then start Timer
	if(frame_start)
	{
		//Enable timer Interrupt
		timer_enable_intr(TIMER_GRP, TIMER_ID);
		//Set Timer Counter Value to 0
		timer_set_counter_value(TIMER_GRP, TIMER_ID, 0);
		//Start Timer
		timer_start(TIMER_GRP, TIMER_ID);
		frame_start = false;
		rx_frame_len = 0;
	}

	// read number of bytes in UART buffer
	rx_fifo_len = IO_BOARD_MODBUS_UART_No.status.rxfifo_cnt;
	while(rx_fifo_len)
	{
		rx_frame[rx_frame_len++] = IO_BOARD_MODBUS_UART_No.fifo.rw_byte; // read all bytes
		rx_fifo_len--;
	}

	// after reading bytes from buffer clear UART interrupt status
	//uart_clear_intr_status(MOD_UART, UART_RXFIFO_FULL_INT_CLR|UART_RXFIFO_TOUT_INT_CLR);
	uart_clear_intr_status(IO_BOARD_MOD_UART, UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_OVF_INT_CLR);
}


//MODBUS Initailization
esp_err_t modbus_init()
{
	frame_start = true;
	rx_frame_len = 0;
	bzero(rx_frame, MAX_FRAM_SIZE);		//Clear the RX Buffer

	//Creating Semaphore for detecting frame
	raw_frame_received = xSemaphoreCreateBinary();

	//Initialize the Timer Required for Modbus operation (to detect end of frame)

	//Select and initialize basic parameters of the timer
	//We will be not using auto reload facility
	timer_config_t config = {
		        				.divider = TIMER_DIVIDER,
								.counter_dir = TIMER_COUNT_UP,
								.counter_en = TIMER_PAUSE,
								.alarm_en = TIMER_ALARM_EN,
								.auto_reload = TIMER_AUTORELOAD_DIS,
							}; // default clock source is APB

	// Initaliaze Timer required for frame end detection in MODBUS
	timer_init(TIMER_GRP, TIMER_ID, &config);

	// Timer's counter will initially start from value below.
	timer_set_counter_value(TIMER_GRP, TIMER_ID, 0);

	// Configure the alarm value and the interrupt on alarm.
	timer_set_alarm_value(TIMER_GRP, TIMER_ID, FRAME_END_TM);
	//timer_enable_intr(TIMER_GRP, TIMER_ID);

	//Adding Callback function for Timer interrupt
	timer_isr_callback_add(TIMER_GRP, TIMER_ID, mod_frame_timer_callback, NULL, 0);

	/*
	* initializing UART required for MODBUS
	*/
	uart_config_t uart_config = {
				       	   	   	 .baud_rate = 115200,
								 .data_bits = UART_DATA_8_BITS,
								 .parity = UART_PARITY_DISABLE,
								 .stop_bits = UART_STOP_BITS_1,
								 .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
								 .source_clk = UART_SCLK_APB,
				    			};

	//Install UART Deriver
	uart_driver_install(IO_BOARD_MOD_UART, RX_BUF_SIZE * 2, 0 , 0, NULL, 0);

	//Assigne Configuration parameter to UART
	uart_param_config(IO_BOARD_MOD_UART, &uart_config);

	//Assigne Pin for UART Port that we are using for MODBUS
	uart_set_pin(IO_BOARD_MOD_UART, IO_BOARD_TX_PIN, IO_BOARD_RX_PIN, -1, -1);

	/* RX interrupt Configuration
	* We will have interrupt for each character so we kept threshold 1
	*/
	//ESP_INTR_FLAG_IRAM
	//MOD_UART_INTR_CONFIG_FLAG
	uart_intr_config_t uart_intr = {
									.intr_enable_mask = MOD_UART_INTR_CONFIG_FLAG,
									.rxfifo_full_thresh = 1,
									.rx_timeout_thresh = 0,
									.txfifo_empty_intr_thresh = 0,
								   };
	uart_intr_config(IO_BOARD_MOD_UART,&uart_intr);

	// release the pre registered UART handler/subroutine
	uart_isr_free(IO_BOARD_MOD_UART);

	// register new UART subroutine
	uart_isr_register(IO_BOARD_MOD_UART,mod_uart_intr_handle, 0, 0, &handle_console);


	//enable RX interrupt
	uart_enable_rx_intr(IO_BOARD_MOD_UART);

	ConfigControlPin();
	MasterRecevdEnable();
	//uart_disable_tx_intr(MOD_UART);
	return ESP_OK;

}


//CRC calculation
uint16_t cal_crc16 (const uint8_t *Data, uint16_t Length)
{
	static const uint16_t CRCTable[] = { 0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
									 	 0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
										 0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
										 0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
										 0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
										 0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
										 0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
										 0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
										 0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
										 0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
										 0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
										 0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
										 0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
										 0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
										 0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
										 0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
										 0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
										 0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
										 0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
										 0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
										 0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
										 0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
										 0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
										 0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
										 0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
										 0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
										 0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
										 0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
										 0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
										 0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
										 0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
										 0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 };
	uint8_t Temp;
	uint16_t CRCWord = 0xFFFF;
	while (Length--)
	{
		Temp = *Data++ ^ CRCWord;
		CRCWord >>= 8;
		CRCWord ^= CRCTable[Temp];
	}
	return CRCWord;
}



/*
 * Function to add CRC at end of Buffer
 */
void add_crc_to_buff(uint8_t *buff, uint8_t len)
{
	uint16_t cal_crc = cal_crc16(buff, len-2);
	//ESP_LOGI(TAG,"Calculated CRC=%x", cal_crc);
	buff[len-2] = (uint8_t)(cal_crc & 0X00FF);		//CRC lower byte
	buff[len-1] = (uint8_t)(cal_crc>>8);				//CRC higher byte
	//ESP_LOGI(TAG,"CRC=%x", buff[len-1]);
	//ESP_LOGI(TAG,"CRC=%x", buff[len-2]);

}

//////////////////////////////////////////////////////////////////
uint8_t modbus_CRC_Check(const uint8_t *buff, uint8_t len)
{
	uint16_t cal_CRC =(cal_crc16 (buff,len-2));
	uint16_t rec_crc=(buff[len-1]<<8 | buff[len-2]);
	if(cal_CRC == rec_crc)
	{
		return 1;
	}
	return 0;


}

