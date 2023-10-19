/*
 * modbus.h
 *
 *  Created on: 19-Aug-2022
 *      Author: Embin 7
 *      This Header file will contain basic function required for modbus operation
 *      This functions are used by modbus_master and modbus_slave software to impliment their functionality
 */
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "lwip/err.h"
#include "freertos/semphr.h"
#include "soc/soc.h"

#include "driver/gpio.h"
#include "sdkconfig.h"

#include "driver/uart.h"
#include "driver/uart_select.h"
#include "hal/uart_hal.h"


#ifndef MAIN_MODBUSRD_H_
#define MAIN_MODBUSRD_H_

/**
 * Defining the UART PORT used for MODBUS Parameter
 */
#define IO_BOARD_MOD_UART 		    UART_NUM_0
#define IO_BOARD_MODBUS_BAUD		115200
#define IO_BOARD_MODBUS_UART_No 	UART0
#define IO_BOARD_MODBUS_TIMOUT	    0.5*APB_CLK_FREQ	//Required ticks for 0.5Sec

#define MOD_UART_INTR_CONFIG_FLAG ((UART_INTR_RXFIFO_FULL)\
								| (UART_INTR_RXFIFO_OVF))
/*
 * MODBUS TX PIN and RX PIN
 */
#define IO_BOARD_TX_PIN      23
#define IO_BOARD_RX_PIN      32


//max485 control pin.

#define SET               1
#define RESET             0
#define IO_BOARD_COM_CONTROL_PIN   4



//#define COM_SEND_ENB()     {gpio_set_level(COM_CONTROL_PIN, 1);}
//#define COM_RECIEVE_ENB()  {gpio_set_level(COM_CONTROL_PIN, 0);}

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

/**
 * Defining Timer Related parameters
 */
#define TIMER_DIVIDER	80
#define	TIMER_GRP	TIMER_GROUP_1
#define TIMER_ID	TIMER_0

/*
 * Defining buffer to collect received frame
 * length of received frame
 */
uint8_t rx_frame[256];
uint8_t rx_frame_len;


/*
 * Type Def to Indicate Error
 */
typedef int modb_err_t;

/* Definitions for error constants. */
#define MODB_OK          0       /*!< modb_err_t value indicating success (no error) */
#define MODB_FAIL        -1      /*!< Generic modb_err_t code indicating failure */

#define MODB_ERR_TIMEOUT            0x01	/*!< Operation timed out */
#define MODB_ERR_INVALID_CRC		0X02    /*!< Invalid Frame CRC Received */
#define MODB_ERR_INVALID_DATA		0X03	/*!< Invalid Data received*/
#define MODB_ERR_ERROR_CODE_REC		0X04	/*!< Error code received from Slave*/


/**
 * @brief MODBUS Request Message parameters
 */
typedef struct {
    uint8_t slave_address;                      /*!< Slave Address*/
    uint8_t function_code;       				/*!< Function Code*/
    uint16_t address;							/*!< Address Field of Message*/
    uint8_t data[252];              			/*!< Data to be sent*/
    uint8_t len;								/*!<Length of data field*/
} modbus_req_t;


/*
 * @brief MODBUS Response Message parameters
 */
typedef struct {
    uint8_t slave_address;                      /*!< Slave Address*/
    uint8_t function_code;       				/*!< Function Code*/
    uint8_t data[254];              			/*!< Data to be sent*/
    uint8_t len;								/*!<Length of data field*/
    uint8_t error;								/*!< To inform about Error*/
} modbus_resp_t;

/**
 * @brief
 * --Function used to initialize the Modbus related peripherals like:
 * --UART Port and its configuration
 * --Timer required etc
 * @param
 * --Semaphore handle require for frame detection
 * @return
 * -- ESP_OK on Success
 */
esp_err_t modbus_init(void);


/**
 * MODBUS Master Functions
 */

/**
 * @brief
 * --0X04
 * --Function to Read Input Registers of Slave
 * --This function will be used to read I/O board status like digital I/P, analog i/p
 * and rs485 i/p
 * @param
 * --slave address of I/O board from which we want to read (uint8_t)
 * --Starting Address of registers to read (uint16_t)
 * --Quantity Register to read (uint16_t)
 * @return
 * --Return Error code if any error occurs or it will send MODB_OK
 */
modb_err_t modb_read_input_register(uint8_t slave_adr, uint16_t reg_addr, uint16_t quant);

/**
 * @brief
 * --0X03
 * --Function to Read Holding Registers of Slave
 * --This function will be used to read I/O board status like digital I/P, analog i/p
 * and rs485 i/p
 * @param
 * --slave address of I/O board from which we want to read (uint8_t)
 * --Starting Address of registers to read (uint16_t)
 * --Quantity Register to read (uint16_t)
 * @return
 * --Return Error code if any error occurs or it will send MODB_OK
 */
modb_err_t modb_read_holding_register(uint8_t slave_adr, uint16_t reg_addr, uint16_t quant);

/**
 * @brief
 * --0X05
 * --Function to Write Single Coils
 * --This function will be used to control Relay O/P on I/O Board according to
 * input read from I/O board
 * @param
 * --slave address of I/O board to which we want to write (uint8_t)
 * --coil address to which we want to write (uint16_t)
 * --True False accoding to our need
 * --Coil ON --> True --> 0XFF00
 * --Coil OFF --> False --> 0X0000
 * @return
 * --Return Error code if any error occurs or it will send MODB_OK
 */
modb_err_t modb_write_single_coil(uint8_t slave_adr, uint16_t coil_addr, bool on_off);

/**
 * @brief
 * --0X06
 * --Function to Write Single Register
 * --This function will be used to write configuration parameter to I/O board
 * @param
 * --slave address of I/O board to which we want to write (uint8_t)
 * --reg address to which we want to write (uint16_t)
 * --Data which will be used to write in reg(uint16_t)
 * @return
 * --Return Error code if any error occurs or it will send MODB_OK
 */
modb_err_t modb_write_single_reg(uint8_t slave_adr, uint16_t reg_addr, uint16_t data);

/**
 * @brief
 * --0X15
 * --Function to Write Multiple Coils
 * --This function will be used to control Relays O/P on I/O Board according to
 * input read from I/O board
 * @param
 * --slave address of I/O board to which we want to write (uint8_t)
 * --starting coil address to which we want to write (uint16_t)
 * --Quantity of Coils to write (uint16_t)
 * --Pointer to Data which will be used to write in coils(uint8_t)
 * @return
 * --Return Error code if any error occurs or it will send MODB_OK
 */
modb_err_t modb_write_multiple_coils(uint8_t slave_adr, uint16_t coil_addr, uint16_t quant, uint8_t *data);


/**
 * @brief
 * --0X16
 * --Function to Write Multiple Registers
 * --This function will be used to write configuration parameter to I/O board
 * @param
 * --slave address of I/O board to which we want to write (uint8_t)
 * --starting Register address to which we want to write (uint16_t)
 * --Quantity of registers to write (uint16_t)
 * --Pointer to Data which will be used to write in coils(uint8_t)
 * @return
 * --Return Error code if any error occurs or it will send MODB_OK
 */
modb_err_t modb_write_multiple_regs(uint8_t slave_adr, uint16_t reg_addr, uint16_t quant, uint8_t *data);


/**
 *
 * @brief
 *-- Function to calculate CRC16 before sending packet or after receiving frame
 * @param
 * --Data is pointer to starting location where frame data is stored
 * --Length is length of modbus frame for which we are calculating CRC
 * @return
 * --Returns the 16-bit calculated CRC16
 */

uint16_t cal_crc16 (const uint8_t *Data, uint16_t Length);

/**
 * @brief
 * --Function to calculate and add CRC to buffer according to Modbus protocol
 * @param
 * --Pointer to Starting of buffer to which we have to add CRC
 * -- length of buffer with CRC byte
 */
void add_crc_to_buff(uint8_t *buff, uint8_t len);

/**
 * @brief
 * --This function will Wait for responce from respective slave device for predefined time
 * --Function to validate Modbus frame received using CRCfield of Modbus Frame
 * @param
 * --Slave address from whome we expected responce
 * --Message function code for which we are expecting responce
 * @return
 * --Return modb_err_t type value according to responce
 */
modb_err_t wait_and_validate_responce(uint8_t slave_adr, uint8_t fn_code);



//void modbus_req_frame(uint8_t slaveId);

uint8_t modbus_CRC_Check(const uint8_t *buff, uint8_t len);
void ConfigControlPin(void);


#endif /* MAIN_MODBUSRD_H_ */
