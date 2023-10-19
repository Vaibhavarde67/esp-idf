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
	switch(pgNo)
	{
	case NO_OF_IO_BORD_SET_PAGE:
	{
		DWIN_Set_Int(NO_OF_IO_BRD_VER_VP, present_io_board_count);
		break;
	}

	case IO_BOARD_SELECT_PAGE:
	{
		DWIN_Clear_String(IO_BRD1_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(IO_BRD2_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(IO_BRD3_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(IO_BRD4_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);

		DWIN_Set_String(IO_BRD1_NAME_TXT_ENT_VP, io_board_input_data[0].io_board_name);
		DWIN_Set_String(IO_BRD2_NAME_TXT_ENT_VP, io_board_input_data[1].io_board_name);
		DWIN_Set_String(IO_BRD3_NAME_TXT_ENT_VP, io_board_input_data[2].io_board_name);
		DWIN_Set_String(IO_BRD4_NAME_TXT_ENT_VP, io_board_input_data[3].io_board_name);

		DWIN_Set_Int(IO_BRD1_ICON_VP, 0x01);
		DWIN_Set_Int(IO_BRD2_ICON_VP, 0x01);
		DWIN_Set_Int(IO_BRD3_ICON_VP, 0x01);
		DWIN_Set_Int(IO_BRD4_ICON_VP, 0x01);
		break;
	}

	case IO_BOARD_NAME_CANGE_PAGE:
	{
		DWIN_Clear_String(IO_BRD1_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(IO_BRD2_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(IO_BRD3_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(IO_BRD4_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);

		DWIN_Set_String(IO_BRD1_NAME_TXT_ENT_VP, io_board_input_data[0].io_board_name);
		DWIN_Set_String(IO_BRD2_NAME_TXT_ENT_VP, io_board_input_data[1].io_board_name);
		DWIN_Set_String(IO_BRD3_NAME_TXT_ENT_VP, io_board_input_data[2].io_board_name);
		DWIN_Set_String(IO_BRD4_NAME_TXT_ENT_VP, io_board_input_data[3].io_board_name);

		DWIN_Set_Int(IO_BRD1_ICON_VP, 0x01);
		DWIN_Set_Int(IO_BRD2_ICON_VP, 0x01);
		DWIN_Set_Int(IO_BRD3_ICON_VP, 0x01);
		DWIN_Set_Int(IO_BRD4_ICON_VP, 0x01);
		break;
	}

	case IO_BOARD_IP_SETTING_PAGE:
	{
		DWIN_Clear_String(SELECTED_IO_BRD_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Set_String(SELECTED_IO_BRD_TXT_ENT_VP, io_board_input_data[selected_io_board].io_board_name);

		break;
	}

	case RS485_IP_NAME_CHNGE_PAGE:
	{
		DWIN_Clear_String(SENSOR1_TEMP_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(SENSOR1_HUM_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(SENSOR2_TEMP_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(SENSOR2_HUM_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);

		DWIN_Set_String(SENSOR1_TEMP_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].rs485_input[0].temperature_sensor.temperature_name);
		DWIN_Set_String(SENSOR1_HUM_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].rs485_input[0].humidity_sensor.humidity_name);
		DWIN_Set_String(SENSOR2_TEMP_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].rs485_input[1].temperature_sensor.temperature_name);
		DWIN_Set_String(SENSOR2_HUM_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].rs485_input[1].humidity_sensor.humidity_name);

		break;
	}

	case RS485_IP_LIMIT_SET_PAGE:
	{
		DWIN_Set_Int(SENSOR1_HUM_LIMIT_VER_VP, io_board_input_data[selected_io_board].rs485_input[0].humidity_sensor.limit);
		DWIN_Set_Int(SENSOR2_HUM_LIMIT_VER_VP, io_board_input_data[selected_io_board].rs485_input[1].humidity_sensor.limit);
		DWIN_Set_Int(SENSOR1_TEMP_LIMIT_VER_VP, io_board_input_data[selected_io_board].rs485_input[0].temperature_sensor.limit);
		DWIN_Set_Int(SENSOR2_TEMP_LIMIT_VER_VP, io_board_input_data[selected_io_board].rs485_input[1].temperature_sensor.limit);
		break;
	}

	case ANALOG_IP_NAME_CHANGE_PAGE:
	{
		DWIN_Clear_String(ANALOG_IP1_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(ANALOG_IP2_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(ANALOG_IP3_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(ANALOG_IP4_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);

		DWIN_Set_String(ANALOG_IP1_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].analog_input[0].analog_ip_name);
		DWIN_Set_String(ANALOG_IP2_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].analog_input[1].analog_ip_name);
		DWIN_Set_String(ANALOG_IP3_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].analog_input[2].analog_ip_name);
		DWIN_Set_String(ANALOG_IP4_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].analog_input[3].analog_ip_name);
		break;
	}

	case ANALOG_IP_LIMIT_SET_PAGE:
	{
		DWIN_Set_Int(ANALOG_IP1_LIMIT_VER_VP, io_board_input_data[selected_io_board].analog_input[0].limit);
		DWIN_Set_Int(ANALOG_IP2_LIMIT_VER_VP, io_board_input_data[selected_io_board].analog_input[1].limit);
		DWIN_Set_Int(ANALOG_IP3_LIMIT_VER_VP, io_board_input_data[selected_io_board].analog_input[2].limit);
		DWIN_Set_Int(ANALOG_IP4_LIMIT_VER_VP, io_board_input_data[selected_io_board].analog_input[3].limit);
		break;
	}

	case DIGITAL_IP_NAME1_PAGE:
	{
		DWIN_Clear_String(DIGITAL_IP1_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP2_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP3_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP4_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP5_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP6_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP7_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP8_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);

		DWIN_Set_String(DIGITAL_IP1_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[0].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP2_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[1].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP3_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[2].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP4_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[3].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP5_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[4].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP6_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[5].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP7_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[6].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP8_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[7].digital_ip_name);
		break;
	}

	case DIGITAL_IP_NAME2_PAGE:
	{
		DWIN_Clear_String(DIGITAL_IP9_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP10_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP11_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP12_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP13_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP14_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP15_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);
		DWIN_Clear_String(DIGITAL_IP16_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);

		DWIN_Set_String(DIGITAL_IP9_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[8].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP10_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[9].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP11_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[10].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP12_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[11].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP13_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[12].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP14_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[13].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP15_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[14].digital_ip_name);
		DWIN_Set_String(DIGITAL_IP16_NAME_TXT_ENT_VP, io_board_input_data[selected_io_board].digital_input[15].digital_ip_name);
		break;
	}

	case OUTGOING_EMAIL_SRV_SET_PAGE:
	{
		DWIN_Clear_String(EMAIL_SERV_TXT_ENT_VP, MAX_EMAIL_ID_LENGTH);
		DWIN_Clear_String(SENDER_EMAIL_ID_TXT_ENT_VP, MAX_EMAIL_ID_LENGTH);
		DWIN_Clear_String(SENDER_PASSWORD_TXT_ENT_VP, 30);
		DWIN_Clear_String(SENDER_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);

		DWIN_Set_Int(EMAIL_PORT_NUM_VER_VP, io_board_comm_info.email_srv_info.port_number);
		DWIN_Set_String(EMAIL_SERV_TXT_ENT_VP, io_board_comm_info.email_srv_info.sender_email_server);
		DWIN_Set_String(SENDER_EMAIL_ID_TXT_ENT_VP, io_board_comm_info.email_srv_info.sender_email_id);
		DWIN_Set_String(SENDER_PASSWORD_TXT_ENT_VP, io_board_comm_info.email_srv_info.sender_email_password);
		DWIN_Set_String(SENDER_NAME_TXT_ENT_VP, io_board_comm_info.email_srv_info.sender_name);
		break;
	}

	case WIFI_PANEL_ID_SETTING_PAGE:
	{
		DWIN_Clear_String(WIFI_SSID_TXT_ENT_VP, 30);
		DWIN_Clear_String(WIFI_PASSWORD_TXT_ENT_VP, 30);
		DWIN_Clear_String(DEVICE_ID_NAME_TXT_ENT_VP, MAX_NAME_LENGTH_ALLOWED);

		DWIN_Set_String(WIFI_SSID_TXT_ENT_VP, wifi_data.wifi_ssid);
		DWIN_Set_String(WIFI_PASSWORD_TXT_ENT_VP, wifi_data.wifi_pswd);
		DWIN_Set_String(DEVICE_ID_NAME_TXT_ENT_VP, device_name);
		break;
	}

	case REC_EMAIL_ID_SET1_PAGE:
	{
		DWIN_Clear_String(REC1_EMAI_ID_TXT_ENT_VP, MAX_EMAIL_ID_LENGTH);
		DWIN_Clear_String(REC2_EMAI_ID_TXT_ENT_VP, MAX_EMAIL_ID_LENGTH);
		DWIN_Clear_String(REC3_EMAI_ID_TXT_ENT_VP, MAX_EMAIL_ID_LENGTH);
		DWIN_Clear_String(REC4_EMAI_ID_TXT_ENT_VP, MAX_EMAIL_ID_LENGTH);
		DWIN_Clear_String(REC5_EMAI_ID_TXT_ENT_VP, MAX_EMAIL_ID_LENGTH);

		DWIN_Set_String(REC1_EMAI_ID_TXT_ENT_VP, io_board_comm_info.rec_email_info.receiver_email[0]);
		DWIN_Set_String(REC2_EMAI_ID_TXT_ENT_VP, io_board_comm_info.rec_email_info.receiver_email[1]);
		DWIN_Set_String(REC3_EMAI_ID_TXT_ENT_VP, io_board_comm_info.rec_email_info.receiver_email[2]);
		DWIN_Set_String(REC4_EMAI_ID_TXT_ENT_VP, io_board_comm_info.rec_email_info.receiver_email[3]);
		DWIN_Set_String(REC5_EMAI_ID_TXT_ENT_VP, io_board_comm_info.rec_email_info.receiver_email[4]);
		break;
	}

	case REC_EMAIL_ID_SET2_PAGE:
	{
		DWIN_Clear_String(REC6_EMAI_ID_TXT_ENT_VP, MAX_EMAIL_ID_LENGTH);
		DWIN_Clear_String(REC7_EMAI_ID_TXT_ENT_VP, MAX_EMAIL_ID_LENGTH);
		DWIN_Clear_String(REC8_EMAI_ID_TXT_ENT_VP, MAX_EMAIL_ID_LENGTH);
		DWIN_Clear_String(REC9_EMAI_ID_TXT_ENT_VP, MAX_EMAIL_ID_LENGTH);
		DWIN_Clear_String(REC10_EMAI_ID_TXT_ENT_VP, MAX_EMAIL_ID_LENGTH);

		DWIN_Set_String(REC6_EMAI_ID_TXT_ENT_VP, io_board_comm_info.rec_email_info.receiver_email[5]);
		DWIN_Set_String(REC7_EMAI_ID_TXT_ENT_VP, io_board_comm_info.rec_email_info.receiver_email[6]);
		DWIN_Set_String(REC8_EMAI_ID_TXT_ENT_VP, io_board_comm_info.rec_email_info.receiver_email[7]);
		DWIN_Set_String(REC9_EMAI_ID_TXT_ENT_VP, io_board_comm_info.rec_email_info.receiver_email[8]);
		DWIN_Set_String(REC10_EMAI_ID_TXT_ENT_VP, io_board_comm_info.rec_email_info.receiver_email[9]);
		break;
	}

	case PHONE_NUM_SET1_PAGE:
	{
		DWIN_Clear_String(REC1_PHONE_NUM_TXT_ENT_VP, MAX_PHONE_NUM_LENGTH);
		DWIN_Clear_String(REC2_PHONE_NUM_TXT_ENT_VP, MAX_PHONE_NUM_LENGTH);
		DWIN_Clear_String(REC3_PHONE_NUM_TXT_ENT_VP, MAX_PHONE_NUM_LENGTH);
		DWIN_Clear_String(REC4_PHONE_NUM_TXT_ENT_VP, MAX_PHONE_NUM_LENGTH);
		DWIN_Clear_String(REC5_PHONE_NUM_TXT_ENT_VP, MAX_PHONE_NUM_LENGTH);
		DWIN_Clear_String(REC6_PHONE_NUM_TXT_ENT_VP, MAX_PHONE_NUM_LENGTH);

		DWIN_Set_String(REC1_PHONE_NUM_TXT_ENT_VP, io_board_comm_info.mobile_no_info.level1_mobiles[0]);
		DWIN_Set_String(REC2_PHONE_NUM_TXT_ENT_VP, io_board_comm_info.mobile_no_info.level1_mobiles[1]);
		DWIN_Set_String(REC3_PHONE_NUM_TXT_ENT_VP, io_board_comm_info.mobile_no_info.level1_mobiles[2]);

		DWIN_Set_String(REC4_PHONE_NUM_TXT_ENT_VP, io_board_comm_info.mobile_no_info.level2_mobiles[0]);
		DWIN_Set_String(REC5_PHONE_NUM_TXT_ENT_VP, io_board_comm_info.mobile_no_info.level2_mobiles[1]);
		DWIN_Set_String(REC6_PHONE_NUM_TXT_ENT_VP, io_board_comm_info.mobile_no_info.level2_mobiles[2]);
		break;
	}

	case PHONE_NUM_SET2_PAGE:
	{
		DWIN_Clear_String(REC7_PHONE_NUM_TXT_ENT_VP, MAX_PHONE_NUM_LENGTH);
		DWIN_Clear_String(REC8_PHONE_NUM_TXT_ENT_VP, MAX_PHONE_NUM_LENGTH);
		DWIN_Clear_String(REC9_PHONE_NUM_TXT_ENT_VP, MAX_PHONE_NUM_LENGTH);
		DWIN_Clear_String(REC10_PHONE_NUM_TXT_ENT_VP, MAX_PHONE_NUM_LENGTH);

		DWIN_Set_String(REC7_PHONE_NUM_TXT_ENT_VP, io_board_comm_info.mobile_no_info.level3_mobiles[0]);
		DWIN_Set_String(REC8_PHONE_NUM_TXT_ENT_VP, io_board_comm_info.mobile_no_info.level3_mobiles[1]);
		DWIN_Set_String(REC9_PHONE_NUM_TXT_ENT_VP, io_board_comm_info.mobile_no_info.level3_mobiles[2]);
		DWIN_Set_String(REC10_PHONE_NUM_TXT_ENT_VP, io_board_comm_info.mobile_no_info.level3_mobiles[3]);
		break;
	}

	case SCHEDULING_AND_ESC_TIMING_PAGE:
	{
		//DWIN_Set_Int()
		break;
	}

	}


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


//	for(uint8_t i = 0; i<frameLen; i++)
//	{
//		printf("0x%02X\n",dwin_TxBuff[i]);
//
//	}


	if(uart_write_bytes(DWIN_LCD_UART, (const char *)dwin_TxBuff, frameLen)<0)
	{
		memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
		memset(strFrame, 0, strLength);//DWIN_MAX_STR_LEN);
		return ESP_FAIL;
	}

	memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
	memset(strFrame, 0, strLength);//DWIN_MAX_STR_LEN);
	return ESP_OK;

}

esp_err_t DWIN_Clear_String(uint16_t VP_addr, uint8_t len)
{
	uint8_t nbyte = 0;

	dwin_TxBuff[DWIN_FRAME_INDX_HDR_0] = DWIN_FRAME_HDR_0;
	dwin_TxBuff[DWIN_FRAME_INDX_HDR_1] = DWIN_FRAME_HDR_1;

	nbyte = DWIN_FRAME_INDX_CMD_TYPE;

	dwin_TxBuff[nbyte++] = DWIN_CMD_WRITE;
	dwin_TxBuff[nbyte++] = VP_addr >> 8;
	dwin_TxBuff[nbyte++] = VP_addr & 0xFF;

	for(uint8_t i =0; i<len; i++)
	{
		dwin_TxBuff[nbyte++] = 0x20;
	}

	dwin_TxBuff[DWIN_FRAME_INDX_DATA_LEN] = nbyte - DWIN_FRAME_INDX_CMD_TYPE-1;

	uint8_t frameLen = nbyte;

	if(uart_write_bytes(DWIN_LCD_UART, (const char *)dwin_TxBuff, frameLen)<0)
	{
		memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
		return ESP_FAIL;
	}

	memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
	return ESP_OK;

}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
esp_err_t DWIN_Set_String(uint16_t VP_addr, char* str)
{
	uint8_t strRealLen = strlen(str);
	//printf("Length = %d\n", strRealLen);
	memset(dwin_TxStrBuff, 0, DWIN_MAX_STR_LEN);
	memcpy(dwin_TxStrBuff, str, strRealLen);
	dwin_TxStrBuff[strRealLen++] = 0xFF;
	dwin_TxStrBuff[strRealLen++] = 0xFF;

	if(strRealLen > LCD_STR_TXT_MAX_LEN)
		return ESP_FAIL;
	return DWIN_WriteMulti(VP_addr, dwin_TxStrBuff, strRealLen);


}


esp_err_t DWIN_Set_Int(uint16_t VP_addr,int value)
{
	uint8_t nbyte = 0;

	dwin_TxBuff[DWIN_FRAME_INDX_HDR_0] = DWIN_FRAME_HDR_0;
	dwin_TxBuff[DWIN_FRAME_INDX_HDR_1] = DWIN_FRAME_HDR_1;

	dwin_TxBuff[DWIN_FRAME_INDX_DATA_LEN] = 0x05;

	nbyte = DWIN_FRAME_INDX_CMD_TYPE;

	dwin_TxBuff[nbyte++] = DWIN_CMD_WRITE;
	dwin_TxBuff[nbyte++] = VP_addr >> 8;
	dwin_TxBuff[nbyte++] = VP_addr & 0xFF;

	dwin_TxBuff[nbyte++] = value >> 8;
	dwin_TxBuff[nbyte++] = value & 0xFF;

	uint8_t frameLen = nbyte;

	if(uart_write_bytes(DWIN_LCD_UART, (const char *)dwin_TxBuff, frameLen)<0)
	{
		memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
		return ESP_FAIL;
	}

	memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
	return ESP_OK;
}



//esp_err_t DWIN_Set_Float(uint16_t VP_addr,float value)
//{
//	uint8_t nbyte = 0;
//
//	dwin_TxBuff[DWIN_FRAME_INDX_HDR_0] = DWIN_FRAME_HDR_0;
//	dwin_TxBuff[DWIN_FRAME_INDX_HDR_1] = DWIN_FRAME_HDR_1;
//
//	dwin_TxBuff[DWIN_FRAME_INDX_DATA_LEN] = 0x05;
//
//	nbyte = DWIN_FRAME_INDX_CMD_TYPE;
//
//	dwin_TxBuff[nbyte++] = DWIN_CMD_WRITE;
//	dwin_TxBuff[nbyte++] = VP_addr >> 8;
//	dwin_TxBuff[nbyte++] = VP_addr & 0xFF;
//
//	dwin_TxBuff[nbyte++] = (uint8_t)(value >> 24) & 0xFF;
//	dwin_TxBuff[nbyte++] = (value >> 16) & 0xFF;
//	dwin_TxBuff[nbyte++] = (value >> 8) & 0xFF;
//	dwin_TxBuff[nbyte++] = value & 0xFF;
//
//
//	uint8_t frameLen = nbyte;
//
//	if(uart_write_bytes(DWIN_LCD_UART, (const char *)dwin_TxBuff, frameLen)<0)
//	{
//		memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
//		return ESP_FAIL;
//	}
//
//	memset(dwin_TxBuff, 0, DWIN_FRAME_LEN);
//	return ESP_OK;
//}



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

