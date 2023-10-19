/*
 * dwinlcd_functions.h
 *
 *  Created on: 21-Sep-2023
 *      Author: Embin_RD
 */

#ifndef MAIN_DWINLCD_FUNCTIONS_H_
#define MAIN_DWINLCD_FUNCTIONS_H_

#include "lwip/err.h"
#include <string.h>

#include "dwinlcd.h"
#include "gui_vp_page_adr.h"
#include "critical_alarm_data_structure.h"



#define DIWN_MAX_BRIGHTNESS				0x64	//This byte is to set max brightness	0x00--0x64
#define DIWN_MIN_BRIGHTNESS				0x00	//This byte is to set min  brightness  0x00--0x64

#define DWIN_NXT_BYTE_WAIT_CNT 			1 //ms

#define DWIN_DELAY_FRAME_TX				10

#define DWIN_RX_QUEUE_SIZE   			5
#define DWIN_MAX_FRAME_LEN				DWIN_FRAME_LEN
#define DWIN_FRAME_LEN 					100
#define DWIN_MIN_FRAME_LEN 				6
#define DWIN_MAX_STR_LEN 				70

#define DWIN_FRAME_INDX_HDR_0			0
#define DWIN_FRAME_INDX_HDR_1   		1
#define DWIN_FRAME_INDX_DATA_LEN		2
#define DWIN_FRAME_INDX_CMD_TYPE		3 //command 82 or 83
#define DWIN_FRAME_INDX_REG_ADDRESS_H	4
#define DWIN_FRAME_INDX_REG_ADDRESS_L	5


#define DWIN_FRAME_HDR_0 				0x5A
#define DWIN_FRAME_HDR_1 				0xA5

#define DWIN_CMD_WRITE					0X82
#define DWIN_CMD_READ					0X83

#define		DWIN_CMD					1
#define		DWIN_DATA	    			2

#define DWIN_N_REG_READ_RTC				4
#define DWIN_N_REG_READ_PAGE			1
#define DWIN_N_REG_RKC					1


#define DWIN_CMD_LEN_PAGE_SWITCH 		7

#define DWIN_SUBCMD_PAGE_SWITCH 		0X5A01
#define DWIN_SUBCMD_SET_RTC				0x5AA5

#define DWIN_PSWD_INCOMPLETE			2

#define DWIN_TXT_ALIGN_CENTER 			1

#define LCD_ADR_INVALID					255






//main DWIN settings registers
#define LCD_ADR_WRITE_PAGE 				0X0084
#define LCD_ADR_WRITE_RTC			    0x009C

#define LCD_ADR_READ_PAGE 				0X0014
#define LCD_ADR_READ_RTC		        0x0010

#define LCD_QTY_RTC_READ_VP				0x04

#define LCD_STR_TXT_MAX_LEN				60//this is max lenth of all texts


uint16_t dwin_CrntPg;
uint8_t dwin_TxBuff[DWIN_FRAME_LEN];
uint8_t dwin_TxStrBuff[DWIN_MAX_STR_LEN];




esp_err_t DIWN_SLEEP();
esp_err_t DIWN_WAKEUP();
esp_err_t DWIN_GoToPg(uint16_t pgNo);
esp_err_t DWIN_Write(uint16_t addr, uint16_t data);
esp_err_t DWIN_Read(uint16_t addr, uint8_t numBytes);
esp_err_t DWIN_WriteMulti(uint16_t addr, uint8_t* strFrame, uint8_t strLength);
//esp_err_t DWIN_WriteStringAligned(uint16_t addr, char* str, uint8_t alignment);
esp_err_t DWIN_Set_String(uint16_t addr, char* str);

esp_err_t DWIN_Clear_String(uint16_t VP_addr, uint8_t len);


esp_err_t DWIN_Set_Int(uint16_t VP_addr,int value);

//esp_err_t DWIN_Set_Float(uint16_t VP_addr,float value);
//
//uint8_t DWIN_GetMaxTxtLen(uint16_t addr);
//
//uint8_t DWIN_GetIndxFromErrEventName(uint16_t addr);
//uint8_t DWIN_GetIndxFromErrDetail(uint16_t addr);
//uint8_t DWIN_GetIndxFromErrTime(uint16_t addr);
//uint8_t DWIN_GetIndxFromHomePgName(uint16_t addr);
//uint8_t DWIN_GetIndxFromOutgoing_SRVNamechange(uint16_t addr);
//uint8_t DWIN_GetIndxFromWifiandPanlelIDNamechange(uint16_t addr);
//uint8_t DWIN_GetIndxFromEmailIDPG1Namechange(uint16_t addr);
//uint8_t DWIN_GetIndxFromEmailIDPG2Namechange(uint16_t addr);




#endif /* MAIN_DWINLCD_FUNCTIONS_H_ */
