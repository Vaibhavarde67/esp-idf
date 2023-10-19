/*
 * dwinlcd_functions.c
 *
 *  Created on: 21-Sep-2023
 *      Author: Embin_RD
 */

#include "dwinlcd_functions.h"





esp_err_t DIWN_SLEEP()
{
	//uint8_t DIWN_Standby_buf[20];
	uint8_t frame_len = 8;									//as Tx frame will only have 8 bytes
	uint8_t *tx_frame = (uint8_t*) malloc(frame_len);

	tx_frame[0] = 0x5A;
	tx_frame[1] = 0xA5;

	tx_frame[2] = 0x05;
	tx_frame[3] = 0x82;

	tx_frame[4] = 0x00;
	tx_frame[5] = 0x82;

	tx_frame[6] = DIWN_MIN_BRIGHTNESS;
	tx_frame[7] = DIWN_MIN_BRIGHTNESS;

	if(uart_write_bytes(DWIN_LCD_UART, tx_frame, frame_len)<0)
	{
		free(tx_frame);
		return ESP_FAIL;
	}

	free(tx_frame);
	return ESP_OK;
}


/***********************************************************************/

esp_err_t DIWN_WAKEUP()
{

	uint8_t frame_len = 8;									//as Tx frame will only have 8 bytes
	uint8_t *tx_frame = (uint8_t*) malloc(frame_len);

	tx_frame[0] = 0x5A;
	tx_frame[1] = 0xA5;

	tx_frame[2] = 0x05;
	tx_frame[3] = 0x82;

	tx_frame[4] = 0x00;
	tx_frame[5] = 0x82;

	tx_frame[6] = DIWN_MAX_BRIGHTNESS;
	tx_frame[7] = DIWN_MAX_BRIGHTNESS;

	if(uart_write_bytes(DWIN_LCD_UART, tx_frame, frame_len)<0)
	{
		free(tx_frame);
		return ESP_FAIL;
	}

	free(tx_frame);
	return ESP_OK;
}




/***********************************************************************/
esp_err_t DWIN_Write(uint16_t addr, uint16_t data)
{
	//need to add delay of 10ms
	uint8_t nbyte=0;

	dwin_TxBuff[DWIN_FRAME_INDX_HDR_0] = DWIN_FRAME_HDR_0;					//nbyte 0 is for command Header
	dwin_TxBuff[DWIN_FRAME_INDX_HDR_1] = DWIN_FRAME_HDR_1;					//nbyte 1 is for command Header

	nbyte = DWIN_FRAME_INDX_CMD_TYPE; //because 3rd byte is length of further frame.
	dwin_TxBuff[nbyte++] = DWIN_CMD_WRITE;
	dwin_TxBuff[nbyte++] = addr >> 8;
	dwin_TxBuff[nbyte++] = addr & 0xFF;
	switch(addr)
	{
		case LCD_ADR_WRITE_PAGE:
		{
			dwin_TxBuff[nbyte++] = DWIN_SUBCMD_PAGE_SWITCH >> 8;
			dwin_TxBuff[nbyte++] = (uint8_t) DWIN_SUBCMD_PAGE_SWITCH & 0xFF;
		}
		break;


		default:
		{
		}
		break;
	}



	dwin_TxBuff[nbyte++] = data >> 8;
	dwin_TxBuff[nbyte++] = data & 0xFF;

	dwin_TxBuff[DWIN_FRAME_INDX_DATA_LEN] = nbyte - DWIN_FRAME_INDX_CMD_TYPE;

	uint8_t frameLen = nbyte;

	if(uart_write_bytes(DWIN_LCD_UART, dwin_TxBuff, frameLen)<0)
	{
		memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
		return ESP_FAIL;
	}

	memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
	return ESP_OK;
}



/***********************************************************************/


esp_err_t DWIN_Read(uint16_t addr, uint8_t numBytes)
{
	//need to add delay of 10ms
	uint8_t nbyte = 0;

	dwin_TxBuff[DWIN_FRAME_INDX_HDR_0] = DWIN_FRAME_HDR_0;
	dwin_TxBuff[DWIN_FRAME_INDX_HDR_1] = DWIN_FRAME_HDR_1;

	nbyte = DWIN_FRAME_INDX_CMD_TYPE;

	dwin_TxBuff[nbyte++] = DWIN_CMD_READ;
	dwin_TxBuff[nbyte++] = addr >> 8;
	dwin_TxBuff[nbyte++] = addr & 0xFF;
	dwin_TxBuff[nbyte++] = numBytes;

	dwin_TxBuff[DWIN_FRAME_INDX_DATA_LEN] = nbyte - DWIN_FRAME_INDX_CMD_TYPE;

	uint8_t frameLen = nbyte;

	if(uart_write_bytes(DWIN_LCD_UART, dwin_TxBuff, frameLen)<0)
	{
		memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
		return ESP_FAIL;
	}

	memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
	return ESP_OK;

}


/***********************************************************************/
esp_err_t DWIN_GoToPg(uint16_t pgNo)
{
	dwin_CrntPg = pgNo;
	return(DWIN_Write(LCD_ADR_WRITE_PAGE, pgNo));
}

/***********************************************************************/

esp_err_t DWIN_WriteMulti(uint16_t addr, uint8_t* strFrame, uint8_t strLength)
{
		//need to add delay of 10ms
	uint8_t nbyte = 0;

	dwin_TxBuff[DWIN_FRAME_INDX_HDR_0] = DWIN_FRAME_HDR_0;
	dwin_TxBuff[DWIN_FRAME_INDX_HDR_1] = DWIN_FRAME_HDR_1;

	nbyte = DWIN_FRAME_INDX_CMD_TYPE;

	dwin_TxBuff[nbyte++] = DWIN_CMD_WRITE;
	dwin_TxBuff[nbyte++] = addr >> 8;
	dwin_TxBuff[nbyte++] = addr & 0xFF;

//	for(uint8_t i = 0; i < strLength; i++)
//	{
//		dwin_TxBuff[nbyte++] = strFrame[i];
//	}
	memcpy(dwin_TxBuff + nbyte, strFrame, strLength);

	nbyte += strLength;

	dwin_TxBuff[DWIN_FRAME_INDX_DATA_LEN] = nbyte - DWIN_FRAME_INDX_CMD_TYPE;

	uint8_t frameLen = nbyte;



	if(uart_write_bytes(DWIN_LCD_UART, dwin_TxBuff, frameLen)<0)
	{
		memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
		memset(strFrame, 0, strLength);//DWIN_MAX_STR_LEN);
		return ESP_FAIL;
	}

	memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
	memset(strFrame, 0, strLength);//DWIN_MAX_STR_LEN);
	return ESP_OK;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
esp_err_t DWIN_Set_String(uint16_t VP_addr, char* str)
{
	uint8_t strRealLen = strlen(str);

	memcpy(dwin_TxStrBuff, str, strRealLen);

	if(strRealLen > LCD_STR_TXT_MAX_LEN)
		return ESP_FAIL;
	return DWIN_WriteMulti(VP_addr, dwin_TxStrBuff, strRealLen);


}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void DWIN_WriteStringAligned(uint16_t addr, char* str, uint8_t alignment)
//{
//	uint8_t maxTxtLen = DWIN_GetMaxTxtLen(addr);
//	uint8_t fullStr[LCD_TXT_MAX_LEN] = {0};
//	memset(fullStr, 0, LCD_TXT_MAX_LEN);
//	uint8_t strRealLen = strlen(str);
//
//	if(strRealLen > maxTxtLen)
//	{
//		memcpy(fullStr, str, maxTxtLen);
//		memset(fullStr + maxTxtLen, '\0', 1);
//	}
//	else
//	{
//		uint8_t strtPos = (maxTxtLen - strRealLen)/2;
//		memset(fullStr, ' ', strtPos);
//		memcpy(fullStr + strtPos, str, strRealLen);
//		memset(fullStr + strtPos + strRealLen, '\0', 1);
//	}
//
//	if(DWIN_WriteMulti(addr, fullStr, maxTxtLen))
//	{
//		memset(fullStr, 0, LCD_TXT_MAX_LEN);
//		return ESP_OK;
//	}
//	else
//	{
//		memset(fullStr, 0, LCD_TXT_MAX_LEN);
//		return ESP_FAIL;
//	}
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint8_t DWIN_GetMaxTxtLen(uint16_t addr)
//{
//
//	if(DWIN_GetIndxFromErrEventName(addr) != LCD_ADR_INVALID){
//		return LCD_TXTLEN_ERR_EVENT_NAME;
//	}
//	else if(DWIN_GetIndxFromErrDetail(addr) != LCD_ADR_INVALID){
//		return LCD_TXTLEN_ERR_DETAIL;
//	}
//	else if(DWIN_GetIndxFromErrTime(addr) != LCD_ADR_INVALID){
//		return LCD_TXTLEN_ERR_TIME;
//	}
//	else if(DWIN_GetIndxFromHomePgName(addr) != LCD_ADR_INVALID){
//		return LCD_TXTLEN_HOME_PG_NAME;
//	}
//
//	else if(DWIN_GetIndxFromOutgoing_SRVNamechange(addr) != LCD_ADR_INVALID){
//		return LCD_TXTLEN_OUTSRV_PG_NAME;
//	}
//else if(DWIN_GetIndxFromWifiandPanlelIDNamechange(addr) != LCD_ADR_INVALID){
//		return LCD_TXTLEN_EMAIL_PG_NAME;
//	}
//
//	else if(DWIN_GetIndxFromEmailIDPG1Namechange(addr) != LCD_ADR_INVALID){
//		return LCD_TXTLEN_EMAIL_PG_NAME;
//	}
//
//		else if(DWIN_GetIndxFromEmailIDPG2Namechange(addr) != LCD_ADR_INVALID){
//		return LCD_TXTLEN_OUTSRV_PG_NAME;
//	}
//
//
//}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//uint8_t DWIN_GetIndxFromHomePgName(uint16_t addr)
//{
//	uint16_t lastAddr = LCD_ADR_TD_HOME_PG_NAME_STRT + ((LCD_QTY_HOME_PG_NAMES-1) * LCD_VP_OFFSET_NAME);
//	if(addr >= LCD_ADR_TD_HOME_PG_NAME_STRT && addr <= lastAddr)
//	{
//		if(!((addr - LCD_ADR_TD_HOME_PG_NAME_STRT) % LCD_VP_OFFSET_NAME)){
//			return ((addr - LCD_ADR_TD_HOME_PG_NAME_STRT)/LCD_VP_OFFSET_NAME);
//		}
//	}
//	return LCD_ADR_INVALID;
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint8_t DWIN_GetIndxFromErrEventName(uint16_t addr)
//{
//	uint16_t lastAddr = LCD_ADR_TD_ERR_EVENT_NAME_STRT + ((LCD_QTY_ERR-1) * LCD_VP_OFFSET_ERR_EVENT_NAME);
//	if(addr >= LCD_ADR_TD_ERR_EVENT_NAME_STRT && addr <= lastAddr)
//	{
//		if(!((addr - LCD_ADR_TD_ERR_EVENT_NAME_STRT) % LCD_VP_OFFSET_ERR_EVENT_NAME)){
//			return ((addr - LCD_ADR_TD_ERR_EVENT_NAME_STRT)/LCD_VP_OFFSET_ERR_EVENT_NAME);
//		}
//	}
//	return LCD_ADR_INVALID;
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint8_t DWIN_GetIndxFromErrDetail(uint16_t addr)
//{
//	uint16_t lastAddr = LCD_ADR_TD_ERR_DETAIL_STRT + ((LCD_QTY_ERR-1) * LCD_VP_OFFSET_ERR_DETAIL);
//	if(addr >= LCD_ADR_TD_ERR_DETAIL_STRT && addr <= lastAddr)
//	{
//		if(!((addr - LCD_ADR_TD_ERR_DETAIL_STRT) % LCD_VP_OFFSET_ERR_DETAIL)){
//			return ((addr - LCD_ADR_TD_ERR_DETAIL_STRT)/LCD_VP_OFFSET_ERR_DETAIL);
//		}
//	}
//	return LCD_ADR_INVALID;
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint8_t DWIN_GetIndxFromErrTime(uint16_t addr)
//{
//	uint16_t lastAddr = LCD_ADR_TD_ERR_TIME_STRT + ((LCD_QTY_ERR-1) * LCD_VP_OFFSET_ERR_TIME);
//	if(addr >= LCD_ADR_TD_ERR_TIME_STRT && addr <= lastAddr)
//	{
//		if(!((addr - LCD_ADR_TD_ERR_TIME_STRT) % LCD_VP_OFFSET_ERR_TIME)){
//			return ((addr - LCD_ADR_TD_ERR_TIME_STRT)/LCD_VP_OFFSET_ERR_TIME);
//		}
//	}
//	return LCD_ADR_INVALID;
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//uint8_t DWIN_GetIndxFromOutgoing_SRVNamechange(uint16_t addr)
//{
//	uint16_t lastAddr = LCD_ADR_TD_SRV_STRT_CHANGE + ((LCD_QTY_OUTGOING_SRV_PG_NAMES-1) * LCD_VP_OFFSET_NAME);
//	if(addr >= LCD_ADR_TD_SRV_STRT_CHANGE && addr <= lastAddr)
//	{
//		if(!((addr - LCD_ADR_TD_SRV_STRT_CHANGE) % LCD_VP_OFFSET_NAME)){
//			return ((addr - LCD_ADR_TD_SRV_STRT_CHANGE)/LCD_VP_OFFSET_NAME);
//		}
//	}
//	return LCD_ADR_INVALID;
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//uint8_t DWIN_GetIndxFromWifiandPanlelIDNamechange(uint16_t addr)
//{
//	uint16_t lastAddr = LCD_ADR_TD_Wifi_SSID_NAME_CHANGE + ((LCD_QTY_Wifi_PanelID_PG_NAMES-1) * LCD_VP_OFFSET_NAME);
//	if(addr >= LCD_ADR_TD_Wifi_SSID_NAME_CHANGE && addr <= lastAddr)
//	{
//		if(!((addr - LCD_ADR_TD_Wifi_SSID_NAME_CHANGE) % LCD_VP_OFFSET_NAME)){
//			return ((addr - LCD_ADR_TD_Wifi_SSID_NAME_CHANGE)/LCD_VP_OFFSET_NAME);
//		}
//	}
//	return LCD_ADR_INVALID;
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//uint8_t DWIN_GetIndxFromEmailIDPG1Namechange(uint16_t addr)
//{
//	uint16_t lastAddr = LCD_ADR_TD_Email_ID_PG1_NAME_CHANGE + ((LCD_QTY_EmailID_PG_NAMES-1) * LCD_VP_OFFSET_NAME);
//	if(addr >= LCD_ADR_TD_Email_ID_PG1_NAME_CHANGE && addr <= lastAddr)
//	{
//		if(!((addr - LCD_ADR_TD_Email_ID_PG1_NAME_CHANGE) % LCD_VP_OFFSET_NAME)){
//			return ((addr - LCD_ADR_TD_Email_ID_PG1_NAME_CHANGE)/LCD_VP_OFFSET_NAME);
//		}
//	}
//	return LCD_ADR_INVALID;
//}
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//uint8_t DWIN_GetIndxFromEmailIDPG2Namechange(uint16_t addr)
//{
//	uint16_t lastAddr = LCD_ADR_TD_Email_ID_PG2_NAME_CHANGE + ((LCD_QTY_EmailID_PG_NAMES-1) * LCD_VP_OFFSET_NAME);
//	if(addr >= LCD_ADR_TD_Email_ID_PG2_NAME_CHANGE && addr <= lastAddr)
//	{
//		if(!((addr - LCD_ADR_TD_Email_ID_PG2_NAME_CHANGE) % LCD_VP_OFFSET_NAME)){
//			return ((addr - LCD_ADR_TD_Email_ID_PG2_NAME_CHANGE)/LCD_VP_OFFSET_NAME);
//		}
//	}
//	return LCD_ADR_INVALID;
//}

