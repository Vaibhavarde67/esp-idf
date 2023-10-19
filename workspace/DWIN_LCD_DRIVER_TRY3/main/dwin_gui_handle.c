/*
 * dwin_gui_handle.c
 *
 *  Created on: 21-Sep-2023
 *      Author: Embin_RD
 */

#include "dwin_gui_handle.h"


static const char *TAG = "DWIN_GUI_HANDLE";



// Initializing various Array
const uint16_t rec_phone_num_vp[] = {REC1_PHONE_NUM_TXT_ENT_VP, REC2_PHONE_NUM_TXT_ENT_VP, REC3_PHONE_NUM_TXT_ENT_VP, REC4_PHONE_NUM_TXT_ENT_VP,
									 REC5_PHONE_NUM_TXT_ENT_VP, REC6_PHONE_NUM_TXT_ENT_VP, REC7_PHONE_NUM_TXT_ENT_VP, REC8_PHONE_NUM_TXT_ENT_VP,
									 REC9_PHONE_NUM_TXT_ENT_VP, REC10_PHONE_NUM_TXT_ENT_VP};

const uint16_t rec_email_id_vp[] = {REC1_EMAI_ID_TXT_ENT_VP, REC2_EMAI_ID_TXT_ENT_VP, REC3_EMAI_ID_TXT_ENT_VP, REC4_EMAI_ID_TXT_ENT_VP,
								 	REC5_EMAI_ID_TXT_ENT_VP, REC6_EMAI_ID_TXT_ENT_VP, REC7_EMAI_ID_TXT_ENT_VP, REC8_EMAI_ID_TXT_ENT_VP,
								    REC9_EMAI_ID_TXT_ENT_VP, REC10_EMAI_ID_TXT_ENT_VP};

const uint16_t digital_input_name_vp[] = {DIGITAL_IP1_NAME_TXT_ENT_VP, DIGITAL_IP2_NAME_TXT_ENT_VP, DIGITAL_IP3_NAME_TXT_ENT_VP, DIGITAL_IP4_NAME_TXT_ENT_VP,
										  DIGITAL_IP5_NAME_TXT_ENT_VP, DIGITAL_IP6_NAME_TXT_ENT_VP, DIGITAL_IP7_NAME_TXT_ENT_VP, DIGITAL_IP8_NAME_TXT_ENT_VP,
										  DIGITAL_IP9_NAME_TXT_ENT_VP, DIGITAL_IP10_NAME_TXT_ENT_VP, DIGITAL_IP11_NAME_TXT_ENT_VP, DIGITAL_IP12_NAME_TXT_ENT_VP,
										  DIGITAL_IP13_NAME_TXT_ENT_VP, DIGITAL_IP14_NAME_TXT_ENT_VP, DIGITAL_IP15_NAME_TXT_ENT_VP, DIGITAL_IP16_NAME_TXT_ENT_VP};

const uint16_t analog_input_name_vp[] = { ANALOG_IP1_NAME_TXT_ENT_VP, ANALOG_IP2_NAME_TXT_ENT_VP, ANALOG_IP3_NAME_TXT_ENT_VP, ANALOG_IP4_NAME_TXT_ENT_VP };

const uint16_t analog_input_limit_vp[] = { ANALOG_IP1_LIMIT_VER_VP, ANALOG_IP2_LIMIT_VER_VP, ANALOG_IP3_LIMIT_VER_VP, ANALOG_IP4_LIMIT_VER_VP};

const uint16_t log_ser_num_vp[] = { LG_SR_NO1__VER_VP, LG_SR_NO2__VER_VP, LG_SR_NO3__VER_VP, LG_SR_NO4__VER_VP};

const uint16_t log_event_name_vp[] = { LG_EVENT1_NAME_TXT_ENT_VP, LG_EVENT2_NAME_TXT_ENT_VP, LG_EVENT3_NAME_TXT_ENT_VP, LG_EVENT4_NAME_TXT_ENT_VP };

const uint16_t log_event_detail_vp[] = { LG_EVENT1_DETAILS_TXT_ENT_VP, LG_EVENT2_DETAILS_TXT_ENT_VP, LG_EVENT3_DETAILS_TXT_ENT_VP, LG_EVENT4_DETAILS_TXT_ENT_VP };

const uint16_t log_event_timestamp_vp[] = { LG_EVENT1_TIMSTMP_TXT_ENT_VP, LG_EVENT2_TIMSTMP_TXT_ENT_VP, LG_EVENT3_TIMSTMP_TXT_ENT_VP, LG_EVENT4_TIMSTMP_TXT_ENT_VP };



esp_err_t dwin_process_frame(struct dwin_frame *dwin_frame)
{

	switch(dwin_frame->VP_addr)
	{
	case 0x1001:
	{
		if(dwin_frame->RKC == GO_TO_HOME_PAGE_RKC)
			DWIN_GoToPg(HOME_PAGE);

		break;
	}

	case 0x1002:
	{
		DWIN_GoToPg(HOME_PAGE);
		break;
	}

	case HOME_PAGE_VP:
	{
		home_page_handel(dwin_frame);
		break;
	}

	case EVENT_LOG_PAGE_VP:
	{
		event_log_page_handel(dwin_frame);
		break;
	}

	case SETTING_PAGE_VP:
	{
		setting_page_handel(dwin_frame);
		break;
	}

	case NO_OF_IO_BORD_SET_PAGE_VP:
	{
		no_of_io_board_page_handel(dwin_frame);
		break;
	}

	case IO_BOARD_SETTING_SEL_PAGE_VP:
	{
		io_bord_setting_select_page_handle(dwin_frame);
		break;
	}

	case IO_BOARD_SELECT_PAGE_VP:
	{
		io_board_select_page_handel(dwin_frame);
		break;
	}

	case IO_BOARD_NAME_CANGE_PAGE_VP:
	{
		io_board_name_change_page_handel(dwin_frame);
		break;
	}

	case IO_BOARD_IP_SETTING_PAGE_VP:
	{
		io_board_input_setting_page_handel(dwin_frame);
		break;
	}

	case RS485_IP_SETTING_PAGE_VP:
	{
		rs485_input_setting_page_handel(dwin_frame);
		break;
	}

	case RS485_IP_NAME_CHNGE_PAGE_VP:
	{
		rs485_input_name_change_page_handle(dwin_frame);
		break;
	}

	case RS485_IP_LIMIT_SET_PAGE_VP:
	{
		rs485_limit_control_page_handel(dwin_frame);
		break;
	}

	case ANALOG_IP_SET_SEL_PAGE_VP:
	{
		analog_input_setting_select_page_handel(dwin_frame);
		break;
	}

	case ANALOG_IP_NAME_CHANGE_PAGE_VP:
	{
		analog_input_name_change_page_handel(dwin_frame);
		break;
	}

	case ANALOG_IP_LIMIT_SET_PAGE_VP:
	{
		analog_input_limit_set_page_handel(dwin_frame);
		break;
	}

	case DIGITAL_IP_NAME1_PAGE_VP:
	{
		digital_input_setting1_handel(dwin_frame);
		break;
	}

	case DIGITAL_IP_NAME2_PAGE_VP:
	{
		digital_input_setting2_handel(dwin_frame);
		break;
	}

	case ESCALATION_MATRIX_SETTING_PAGE_VP:
	{
		escalation_matrix_setting_page_handel(dwin_frame);
		break;
	}

	case OUTGOING_EMAIL_SRV_SET_PAGE_VP:
	{
		outgoing_email_srv_set_page_handel(dwin_frame);
		break;
	}

	case WIFI_PANEL_ID_SETTING_PAGE_VP:
	{
		wifi_panel_id_setting_page_handel(dwin_frame);
		break;
	}

	case REC_EMAIL_ID_SET1_PAGE_VP:
	{
		rec_email_id_set1_page_handel(dwin_frame);
		break;
	}

	case REC_EMAIL_ID_SET2_PAGE_VP:
	{
		rec_email_id_set2_page_handel(dwin_frame);
		break;
	}

	case PHONE_NUM_SET1_PAGE_VP:
	{
		phone_num_set1_page_handel(dwin_frame);
		break;
	}

	case PHONE_NUM_SET2_PAGE_VP:
	{
		phone_num_set2_page_handel(dwin_frame);
		break;
	}

	case SCHEDULING_AND_ESC_TIMING_PAGE_VP:
	{
		scheduling_and_esc_timing_page_handel(dwin_frame);
		break;
	}


	}



	return ESP_OK;
}


void home_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case SILENCE_BUTTON_RKC:
	{
		ESP_LOGI(TAG, "Silence Button Pressed");
		break;
	}
	case GO_TO_LOG_PAGE_RKC:
	{
		//Load the first page Log page by reading from NVS
		current_log_pg_cn = 1;
		DWIN_GoToPg(EVENT_LOG_PAGE);
		break;
	}
	case GO_TO_SETTING_PAGE_RKC:
	{

		DWIN_GoToPg(SETTING_PAGE);
		break;
	}

	case GO_TO_HELP_PAGE_RKC:
	{
		//Load the Help Page

		break;
	}

	}
}


void event_log_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_4_RKC:
	{
		current_log_pg_cn--;
		if(current_log_pg_cn==0)
		{
			DWIN_GoToPg(HOME_PAGE);
		}
		else
		{
			DWIN_GoToPg(DUMMY_PAGE);
			//according to counter current_log_pg_cn display LOG
			DWIN_GoToPg(EVENT_LOG_PAGE);
		}
		break;
	}
	case NEXT_BUTTON_4_RKC:
	{
		current_log_pg_cn++;
		if(current_log_pg_cn >= MAX_LOG_PAGES)
		{
			current_log_pg_cn = MAX_LOG_PAGES;
			return;
		}
		DWIN_GoToPg(DUMMY_PAGE);
		//Load the first page Log page by reading from NVS
		DWIN_GoToPg(EVENT_LOG_PAGE);
		break;
	}

	}
}


void setting_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_5_RKC:
	{
		DWIN_GoToPg(HOME_PAGE);
		break;
	}
	case ESCALATION_MATRIX_SET_BUT_RKC:
	{
		DWIN_GoToPg(ESCALATION_MATRIX_SETTING_PAGE);
		break;
	}
	case IO_BOARD_SET_BUTTON_RKC:
	{
		//Set the IO_BOARD count according to saved setting and then show the page
		DWIN_GoToPg(NO_OF_IO_BORD_SET_PAGE);
		break;
	}


	}

}

void no_of_io_board_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_6_RKC:
	{
		DWIN_GoToPg(SETTING_PAGE);
		break;
	}
	case SAVE_And_NEXT_BUTTON_6_RKC:
	{
		//First read the Variable indicating No of IO Board connected
		DWIN_GoToPg(IO_BOARD_SETTING_SEL_PAGE);
		break;
	}

	}
}

void io_bord_setting_select_page_handle(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_7_RKC:
	{
		//Set the IO_BOARD Count before displaying
		DWIN_GoToPg(NO_OF_IO_BORD_SET_PAGE);
		break;
	}
	case IO_BOARD_NAME_CHANGE_BUTTON_RKC:
	{
		//First according to no of IO board present and saved name display the buttons and then display page
		DWIN_GoToPg(IO_BOARD_NAME_CANGE_PAGE);
		break;
	}

	case IO_BOARD_IP_SETTING_BUTTON_RKC:
	{
		//According to No of IO Board show the buttons
		selected_io_board = 0;
		DWIN_GoToPg(IO_BOARD_SELECT_PAGE);
		break;
	}

	}
}


void io_board_select_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_8_RKC:
	{
		DWIN_GoToPg(SETTING_PAGE);
		break;
	}
	case IO_BOARD1_BUTTON_RKC:
	{
		selected_io_board = 0;
		DWIN_GoToPg(IO_BOARD_IP_SETTING_PAGE);
		break;
	}

	case IO_BOARD2_BUTTON_RKC:
	{
		selected_io_board = 1;
		DWIN_GoToPg(IO_BOARD_IP_SETTING_PAGE);
		break;
	}

	case IO_BOARD3_BUTTON_RKC:
	{
		selected_io_board = 2;
		DWIN_GoToPg(IO_BOARD_IP_SETTING_PAGE);
		break;
	}

	case IO_BOARD4_BUTTON_RKC:
	{
		selected_io_board = 3;
		DWIN_GoToPg(IO_BOARD_IP_SETTING_PAGE);
		break;
	}

	}
}


void io_board_name_change_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_9_RKC:
	{
		DWIN_GoToPg(SETTING_PAGE);
		break;
	}
	case SAVE_BUTTON_9_RKC:
	{
		//First read all IO board name and saved in nvs
		DWIN_GoToPg(SETTING_PAGE);
		break;
	}


	}
}

void io_board_input_setting_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_A_RKC:
	{
		//According to No of IO Board show the buttons
		selected_io_board = 0;
		DWIN_GoToPg(IO_BOARD_SELECT_PAGE);
		break;
	}
	case RS485_IP_SET_PAGE_BUTTON:
	{
		DWIN_GoToPg(RS485_IP_SETTING_PAGE);
		break;
	}
	case ANALOG_IP_SET_PAGE_BUTTON:
	{
		DWIN_GoToPg(ANALOG_IP_SET_SEL_PAGE);
		break;
	}

	case DIGITAL_IP_SET_PAGE_BUTTON:
	{
		DWIN_GoToPg(DIGITAL_IP_NAME1_PAGE);
		break;
	}


	}
}


void rs485_input_setting_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_B_RKC:
	{
		DWIN_GoToPg(IO_BOARD_IP_SETTING_PAGE);
		break;
	}
	case RS485_IP_NAME_CHANGE_BUTTON_RKC:
	{
		//First load all rs485 input names saved in nvs and display it on screen and then display page
		DWIN_GoToPg(RS485_IP_NAME_CHNGE_PAGE);
		break;
	}
	case RS485_IP_LIMIT_BUTTON_RKC:
	{
		//First load all rs485 inputs limits saved in nvs and display it on screen and then display on page
		DWIN_GoToPg(RS485_IP_LIMIT_SET_PAGE);
		break;
	}

	}
}

void rs485_input_name_change_page_handle(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_C_RKC:
	{
		DWIN_GoToPg(RS485_IP_SETTING_PAGE);
		break;
	}
	case ANALOG_IP_SET_BUTTON_C_RKC:
	{
		DWIN_GoToPg(ANALOG_IP_SET_SEL_PAGE);
		break;
	}
	case DIGITAL_IP_SET_BUTTON_C_RKC:
	{
		//Set the digital inputs saved names
		DWIN_GoToPg(DIGITAL_IP_NAME1_PAGE);
		break;
	}

	case SAVE_BUTTON_C_RKC:
	{
		//Readd all rs485 names from page and store it in nvs storage and accordingly update the structure
		DWIN_GoToPg(RS485_IP_SETTING_PAGE);
		break;
	}

	}
}

void rs485_limit_control_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_D_RKC:
	{
		DWIN_GoToPg(RS485_IP_SETTING_PAGE);
		break;
	}
	case ANALOG_IP_SET_BUTTON_D_RKC:
	{
		DWIN_GoToPg(ANALOG_IP_SET_SEL_PAGE);
		break;
	}
	case DIGITAL_IP_SET_BUTTON_D_RKC:
	{
		DWIN_GoToPg(DIGITAL_IP_NAME1_PAGE);
		break;
	}

	case SAVE_BUTTON_D_RKC:
	{
		//Readd all rs485 inputs higher limit from page and store it in nvs storage and accordingly update the structure
		DWIN_GoToPg(RS485_IP_SETTING_PAGE);
		break;
	}

	}
}


void analog_input_setting_select_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_E_RKC:
	{
		DWIN_GoToPg(IO_BOARD_IP_SETTING_PAGE);
		break;
	}
	case ANALOG_IP_NAME_SET_BUTTON_RKC:
	{
		DWIN_GoToPg(ANALOG_IP_NAME_CHANGE_PAGE);
		break;
	}
	case ANALOG_IP_LIMIT_SET_BUTTON_RKC:
	{
		DWIN_GoToPg(ANALOG_IP_LIMIT_SET_PAGE);
		break;
	}


	}
}


void analog_input_name_change_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_F_RKC:
	{
		DWIN_GoToPg(ANALOG_IP_SET_SEL_PAGE);
		break;
	}
	case DIGITAL_IP_SET_BUTTON_F_RKC:
	{
		//Set the digital inputs saved names
		DWIN_GoToPg(DIGITAL_IP_NAME1_PAGE);
		break;
	}
	case RS485_IP_SET_BUTTON_F_RKC:
	{
		DWIN_GoToPg(RS485_IP_SETTING_PAGE);
		break;
	}

	case SAVE_BUTTON_F_RKC:
	{
		//Readd all analog input names from page and store it in nvs storage and accordingly update the structure
		DWIN_GoToPg(ANALOG_IP_SET_SEL_PAGE);
		break;
	}

	}
}


void analog_input_limit_set_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_10_RKC:
	{
		DWIN_GoToPg(ANALOG_IP_SET_SEL_PAGE);
		break;
	}
	case DIGITAL_IP_SET_BUTTON_10_RKC:
	{
		//Set the digital inputs saved names
		DWIN_GoToPg(DIGITAL_IP_NAME1_PAGE);
		break;
	}
	case RS485_IP_SET_BUTTON_10_RKC:
	{
		DWIN_GoToPg(RS485_IP_SETTING_PAGE);
		break;
	}

	case SAVE_BUTTON_10_RKC:
	{
		//Readd all analog input higher limit from page and store it in nvs storage and accordingly update the structure
		DWIN_GoToPg(ANALOG_IP_SET_SEL_PAGE);
		break;
	}

	}
}


void digital_input_setting1_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_11_RKC:
	{
		DWIN_GoToPg(IO_BOARD_IP_SETTING_PAGE);
		break;
	}
	case ANALOG_IP_BUTTON_11_RKC:
	{
		DWIN_GoToPg(ANALOG_IP_SET_SEL_PAGE);
		break;
	}
	case RS485_IP_SET_BUTTON_11_RKC:
	{
		DWIN_GoToPg(RS485_IP_SETTING_PAGE);
		break;
	}

	case SAVE_AND_NEXT_BUTTON_11_RKC:
	{
		//Readd all digital input name from page and store it in nvs storage and accordingly update the structure
		//Also load the next page with digital input names
		DWIN_GoToPg(DIGITAL_IP_NAME2_PAGE);
		break;
	}

	}
}



void digital_input_setting2_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_12_RKC:
	{
		//Set the digital inputs saved names
		DWIN_GoToPg(DIGITAL_IP_NAME1_PAGE);
		break;
	}
	case ANALOG_IP_BUTTON_11_RKC:
	{
		DWIN_GoToPg(ANALOG_IP_SET_SEL_PAGE);
		break;
	}
	case RS485_IP_SET_BUTTON_11_RKC:
	{
		DWIN_GoToPg(RS485_IP_SETTING_PAGE);
		break;
	}

	case SAVE_AND_NEXT_BUTTON_11_RKC:
	{
		//Readd all digital input name from page and store it in nvs storage and accordingly update the structure
		DWIN_GoToPg(IO_BOARD_IP_SETTING_PAGE);
		break;
	}

	}
}


void escalation_matrix_setting_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_13_RKC:
	{
		DWIN_GoToPg(SETTING_PAGE);
		break;
	}
	case SMS_SCHEDULE_BUTTON_13_RKC:
	{
		DWIN_GoToPg(SCHEDULING_AND_ESC_TIMING_PAGE);
		break;
	}
	case MOBILE_NUM_SET_BUTTON_13_RKC:
	{
		//first set the saved phone number on page
		DWIN_GoToPg(PHONE_NUM_SET1_PAGE);
		break;
	}

	case REC_EMAILID_SET_BUTTON_13_RKC:
	{
		//first set the saved e-mail ids on page
		DWIN_GoToPg(REC_EMAIL_ID_SET1_PAGE);
		break;
	}

	case SEND_EMAILID_SET_BUTTON_13_RKC:
	{
		//first set the page with the saved contents
		DWIN_GoToPg(OUTGOING_EMAIL_SRV_SET_PAGE);
		break;
	}

	case WIFI_AND_PANEL_ID_SET_BUTTON_13_RKC:
	{
		//first set the page with the saved contents
		DWIN_GoToPg(WIFI_PANEL_ID_SETTING_PAGE);
		break;
	}

	}
}


void outgoing_email_srv_set_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_14_RKC:
	{
		DWIN_GoToPg(ESCALATION_MATRIX_SETTING_PAGE);
		break;
	}
	case SAVE_BUTTON_14_RKC:
	{
		//read the all data entered on page and save it
		DWIN_GoToPg(ESCALATION_MATRIX_SETTING_PAGE);
		break;
	}

	}
}


void wifi_panel_id_setting_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_15_RKC:
	{
		DWIN_GoToPg(ESCALATION_MATRIX_SETTING_PAGE);
		break;
	}
	case SAVE_BUTTON_15_RKC:
	{
		//read the all data entered on page and save it
		DWIN_GoToPg(ESCALATION_MATRIX_SETTING_PAGE);
		break;
	}

	}
}

void rec_email_id_set1_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_16_RKC:
	{
		DWIN_GoToPg(ESCALATION_MATRIX_SETTING_PAGE);
		break;
	}
	case SAVE_AND_NEXT_BUTTON_16_RKC:
	{
		//read the all data entered on page and save it
		DWIN_GoToPg(REC_EMAIL_ID_SET2_PAGE);
		break;
	}
	}
}


void rec_email_id_set2_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_17_RKC:
	{
		DWIN_GoToPg(REC_EMAIL_ID_SET1_PAGE);
		break;
	}
	case SAVE_BUTTON_17_RKC:
	{
		//read the all data entered on page and save it
		DWIN_GoToPg(ESCALATION_MATRIX_SETTING_PAGE);
		break;
	}
	}
}


void phone_num_set1_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_18_RKC:
	{
		DWIN_GoToPg(ESCALATION_MATRIX_SETTING_PAGE);
		break;
	}
	case SAVE_AND_NEXT_BUTTON_18_RKC:
	{
		//read the all data entered on page and save it
		DWIN_GoToPg(PHONE_NUM_SET2_PAGE);
		break;
	}
	}
}


void phone_num_set2_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_19_RKC:
	{
		DWIN_GoToPg(PHONE_NUM_SET1_PAGE);
		break;
	}
	case SAVE_BUTTON_19_RKC:
	{
		//read the all data entered on page and save it
		DWIN_GoToPg(ESCALATION_MATRIX_SETTING_PAGE);
		break;
	}
	}
}


void scheduling_and_esc_timing_page_handel(struct dwin_frame *dwin_frame)
{
	switch(dwin_frame->RKC)
	{
	case BACK_BUTTON_1A_RKC:
	{
		DWIN_GoToPg(ESCALATION_MATRIX_SETTING_PAGE);
		break;
	}
	case SAVE_BUTTON_1A_RKC:
	{
		//read the all data entered on page and save it
		DWIN_GoToPg(ESCALATION_MATRIX_SETTING_PAGE);
		break;
	}
	}
}



