/*
 * modbus.h
 *
 *  Created on: 19-Aug-2022
 *      Author: Embin 7
 *      This Header file will contain basic function required for modbus operation
 *      This functions are used by modbus_master and modbus_slave software to impliment their functionality
 */

#ifndef MAIN_DWINLCD_H_
#define MAIN_DWINLCD_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "lwip/err.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "soc/soc.h"

#include "driver/gpio.h"

#include "driver/uart.h"
#include "driver/uart_select.h"
#include "hal/uart_hal.h"
#include "esp_intr_alloc.h"

#include "driver/timer.h"
#include "lwip/err.h"

#include "esp_log.h"




/**
 * Defining the UART PORT used for MODBUS Parameter
 */
#define DWIN_LCD_UART 		    UART_NUM_2
#define DWIN_LCD_BAUD			115200
#define DWIN_LCD_UART_No 		UART2
#define DWIN_LCD_TIMOUT	    	0.5*APB_CLK_FREQ	//Required ticks for 0.5Sec

#define DWIN_LCD_UART_INTR_CONFIG_FLAG ((UART_INTR_RXFIFO_FULL)\
										| (UART_INTR_RXFIFO_OVF) | (ESP_INTR_FLAG_IRAM))

#define DWIN_INTR_UART_CONFIG_FLAG 	((UART_INTR_RXFIFO_FULL) | (UART_INTR_TXFIFO_EMPTY) | (UART_INTR_PARITY_ERR) | (UART_INTR_FRAM_ERR)\
		| (UART_INTR_RXFIFO_OVF) | (UART_INTR_DSR_CHG) | (UART_INTR_CTS_CHG) | (UART_INTR_BRK_DET) |  (UART_INTR_RXFIFO_TOUT) | (UART_INTR_SW_XON)\
    	| (UART_INTR_SW_XOFF) | (UART_INTR_GLITCH_DET) | (UART_INTR_TX_BRK_DONE) | (UART_INTR_TX_BRK_IDLE) | (UART_INTR_TX_DONE)\
		| (UART_INTR_RS485_PARITY_ERR) | (UART_INTR_RS485_FRM_ERR) | (UART_INTR_RS485_CLASH) | (UART_INTR_CMD_CHAR_DET))




/*
 * MODBUS TX PIN and RX PIN
 */
#define DWIN_LCD_TX_PIN      17
#define DWIN_LCD_RX_PIN      16


//max485 control pin.

#define SET               1
#define RESET             0



/**
 * Defining Buffer Size For different bufeer required for MODBUS operation through UART
 */
#define BUF_SIZE (512)
#define RX_BUF_SIZE (BUF_SIZE)
#define TX_BUF_SIZE (BUF_SIZE)

/**
 * Defining Parameters related MODBUS Frame Structure
 */
#define FRAME_END_TM	(int)(40*1000000/IO_BOARD_MODBUS_BAUD)//347	//calculated using baudrate and 3.5char time
#define MAX_FRAM_SIZE	256


/*
 * Defining buffer to collect received frame
 * length of received frame
 */
uint8_t rx_frame[256];
uint8_t rx_frame_len;

QueueHandle_t dwin_frame_queu;

//Flag to detect start of frame
bool frame_start;

/*
 * static uart_isr_handle_t *handle_console;
 */
static intr_handle_t handle_console;


/*
 * Type Def to Indicate Error
 */
typedef int modb_err_t;

/* Definitions for error constants. */
#define DWIN_LCD_OK          0       /*!< modb_err_t value indicating success (no error) */
#define DWIN_LCD_FAIL        -1      /*!< Generic modb_err_t code indicating failure */

#define DWIN_LCD_ERR_TIMEOUT            0x01	/*!< Operation timed out */
#define DWIN_LCD_INVALID_CRC		0X02    /*!< Invalid Frame CRC Received */
#define DWIN_LCD_INVALID_DATA		0X03	/*!< Invalid Data received*/
#define DWIN_LCD_ERROR_CODE_REC		0X04	/*!< Error code received from Slave*/


/**
 * @brief MODBUS Request Message parameters
 */
//typedef struct {
//    uint8_t slave_address;                      /*!< Slave Address*/
//    uint8_t function_code;       				/*!< Function Code*/
//    uint16_t address;							/*!< Address Field of Message*/
//    uint8_t data[252];              			/*!< Data to be sent*/
//    uint8_t len;								/*!<Length of data field*/
//} modbus_req_t;


/*
 * @brief MODBUS Response Message parameters
 */
struct dwin_frame{
    uint16_t VP_addr;                    	    /*!< VP Address*/
    uint16_t RKC;								/*!< RKC */
    uint8_t data_length;       				    /*!< Function Code*/
    uint8_t instruction;              			/*!< Data to be sent*/
    uint8_t data[254];							/*!<Length of data field*/
    void * ptr_addr;							/*To store the malloc address */
};

struct dwin_frame recevied_dwin_frame;

/**
 * @brief
 * --Function used to initialize the Dwin LCD related peripherals like:
 * --UART Port and its configuration
 * --Timer required etc
 * @param
 * --None
 * @return
 * -- ESP_OK on Success
 */
esp_err_t dwin_lcd_init(void);




#endif /* MAIN_DWINLCD_H_ */
