/*
 * dwin_gui_handle.h
 *
 *  Created on: 21-Sep-2023
 *      Author: Embin_RD
 */

#ifndef MAIN_DWIN_GUI_HANDLE_H_
#define MAIN_DWIN_GUI_HANDLE_H_

#include "dwinlcd_functions.h"



//Pages Numbers
#define WELLCOME1_PAGE					0
#define WELLCOME2_PAGE					1
#define HOME_PAGE 						2
#define EVENT_LOG_PAGE					3
#define SETTING_PAGE					4
#define NO_OF_IO_BORD_SET_PAGE			5
#define IO_BOARD_SETTING_SEL_PAGE		6
#define IO_BOARD_SELECT_PAGE			7
#define IO_BOARD_NAME_CANGE_PAGE		8
#define IO_BOARD_IP_SETTING_PAGE		9
#define RS485_IP_SETTING_PAGE			10
#define DUMMY_PAGE						11
#define RS485_IP_NAME_CHNGE_PAGE		12
#define RS485_IP_LIMIT_SET_PAGE			13
#define	ANALOG_IP_SET_SEL_PAGE			14
#define ANALOG_IP_NAME_CHANGE_PAGE		15
#define ANALOG_IP_LIMIT_SET_PAGE		16
#define DIGITAL_IP_NAME1_PAGE			17
#define DIGITAL_IP_NAME2_PAGE			22
#define ESCALATION_MATRIX_SETTING_PAGE	23
#define OUTGOING_EMAIL_SRV_SET_PAGE		24
#define WIFI_PANEL_ID_SETTING_PAGE		25
#define REC_EMAIL_ID_SET1_PAGE			26
#define REC_EMAIL_ID_SET2_PAGE			27
#define PHONE_NUM_SET1_PAGE				28
#define PHONE_NUM_SET2_PAGE				29
#define SCHEDULING_AND_ESC_TIMING_PAGE	30


#define GO_TO_HOME_PAGE_RKC				0x0001

//Home Page VP & RKC
#define HOME_PAGE_VP					0x1003
#define SILENCE_BUTTON_RKC				0x0001
#define GO_TO_LOG_PAGE_RKC				0x0002
#define GO_TO_SETTING_PAGE_RKC			0x0003
#define GO_TO_HELP_PAGE_RKC				0x0004


//Event Log Page VP & RKC
#define EVENT_LOG_PAGE_VP				0x1004
#define BACK_BUTTON_4_RKC				0x0001
#define NEXT_BUTTON_4_RKC				0x0002
#define MAX_LOG_PAGES					100
uint8_t current_log_pg_cn;


// SETTING PAGE VP & RKC
#define SETTING_PAGE_VP					0x1005
#define BACK_BUTTON_5_RKC				0x0001
#define ESCALATION_MATRIX_SET_BUT_RKC	0x0002
#define IO_BOARD_SET_BUTTON_RKC			0x0003


//NO_OF_IO_BORD_SET_PAGE VP & RKC
#define NO_OF_IO_BORD_SET_PAGE_VP		0x1006
#define BACK_BUTTON_6_RKC				0x0001
#define SAVE_And_NEXT_BUTTON_6_RKC		0x0002

//IO_BOARD_SETTING_SEL_PAGE VP & RKC
#define IO_BOARD_SETTING_SEL_PAGE_VP	0x1007
#define BACK_BUTTON_7_RKC				0x0001
#define IO_BOARD_NAME_CHANGE_BUTTON_RKC	0x0002
#define IO_BOARD_IP_SETTING_BUTTON_RKC	0x0003

//IO_BOARD_SELECT_PAGE VP & RKC
#define IO_BOARD_SELECT_PAGE_VP			0x1008
#define BACK_BUTTON_8_RKC				0x0001
#define IO_BOARD1_BUTTON_RKC			0x0002
#define IO_BOARD2_BUTTON_RKC			0x0003
#define IO_BOARD3_BUTTON_RKC			0x0004
#define IO_BOARD4_BUTTON_RKC			0x0005
uint8_t selected_io_board;

//IO_BOARD_NAME_CANGE_PAGE VP & RKC
#define IO_BOARD_NAME_CANGE_PAGE_VP		0x1009
#define BACK_BUTTON_9_RKC				0x0001
#define SAVE_BUTTON_9_RKC				0x0002

//IO_BOARD_IP_SETTING_PAGE VP & RKC
#define IO_BOARD_IP_SETTING_PAGE_VP		0x100A
#define BACK_BUTTON_A_RKC				0x0001
#define RS485_IP_SET_PAGE_BUTTON		0x0002
#define ANALOG_IP_SET_PAGE_BUTTON		0x0003
#define DIGITAL_IP_SET_PAGE_BUTTON		0x0004

//RS485_IP_SETTING_PAGE VP & RKC
#define RS485_IP_SETTING_PAGE_VP		0x100B
#define BACK_BUTTON_B_RKC				0x0001
#define RS485_IP_NAME_CHANGE_BUTTON_RKC	0x0002
#define RS485_IP_LIMIT_BUTTON_RKC		0x0003


//RS485_IP_NAME_CHNGE_PAGE VP & RKC
#define RS485_IP_NAME_CHNGE_PAGE_VP		0x100C
#define BACK_BUTTON_C_RKC				0x0001
#define ANALOG_IP_SET_BUTTON_C_RKC		0x0002
#define DIGITAL_IP_SET_BUTTON_C_RKC		0x0003
#define SAVE_BUTTON_C_RKC				0x0004

//RS485_IP_LIMIT_SET_PAGE VP & RKC
#define RS485_IP_LIMIT_SET_PAGE_VP		0x100D
#define BACK_BUTTON_D_RKC				0x0001
#define ANALOG_IP_SET_BUTTON_D_RKC		0x0002
#define DIGITAL_IP_SET_BUTTON_D_RKC		0x0003
#define SAVE_BUTTON_D_RKC				0x0004

//ANALOG_IP_SET_SEL_PAGE VP & RKC
#define ANALOG_IP_SET_SEL_PAGE_VP		0x100E
#define BACK_BUTTON_E_RKC				0x0001
#define ANALOG_IP_NAME_SET_BUTTON_RKC	0x0002
#define ANALOG_IP_LIMIT_SET_BUTTON_RKC	0x0003

//ANALOG_IP_NAME_CHANGE_PAGE VP & RKC
#define ANALOG_IP_NAME_CHANGE_PAGE_VP	0x100F
#define BACK_BUTTON_F_RKC				0x0001
#define DIGITAL_IP_SET_BUTTON_F_RKC		0x0002
#define RS485_IP_SET_BUTTON_F_RKC		0x0003
#define SAVE_BUTTON_F_RKC				0x0004

//ANALOG_IP_LIMIT_SET_PAGE VP & RKC
#define ANALOG_IP_LIMIT_SET_PAGE_VP		0x1010
#define BACK_BUTTON_10_RKC				0x0001
#define DIGITAL_IP_SET_BUTTON_10_RKC	0x0002
#define RS485_IP_SET_BUTTON_10_RKC		0x0003
#define SAVE_BUTTON_10_RKC				0x0004

//DIGITAL_IP_NAME1_PAGE VP & RKC
#define DIGITAL_IP_NAME1_PAGE_VP		0x1011
#define BACK_BUTTON_11_RKC				0x0001
#define ANALOG_IP_BUTTON_11_RKC			0x0002
#define RS485_IP_SET_BUTTON_11_RKC		0x0003
#define SAVE_AND_NEXT_BUTTON_11_RKC		0x0004

//DIGITAL_IP_NAME2_PAGE VP & RKC
#define DIGITAL_IP_NAME2_PAGE_VP	 	0x1012
#define BACK_BUTTON_12_RKC				0x0001
#define ANALOG_IP_BUTTON_12_RKC			0x0002
#define RS485_IP_SET_BUTTON_12_RKC		0x0003
#define SAVE_AND_NEXT_BUTTON_12_RKC		0x0004

//ESCALATION_MATRIX_SETTING_PAGE VP & RKC	23
#define ESCALATION_MATRIX_SETTING_PAGE_VP	0x1013
#define BACK_BUTTON_13_RKC					0x0001
#define SMS_SCHEDULE_BUTTON_13_RKC			0x0002
#define MOBILE_NUM_SET_BUTTON_13_RKC		0x0003
#define REC_EMAILID_SET_BUTTON_13_RKC		0x0004
#define SEND_EMAILID_SET_BUTTON_13_RKC		0x0005
#define WIFI_AND_PANEL_ID_SET_BUTTON_13_RKC 0x0006


//OUTGOING_EMAIL_SRV_SET_PAGE VP & RKC		24
#define OUTGOING_EMAIL_SRV_SET_PAGE_VP		0x1014
#define BACK_BUTTON_14_RKC					0x0001
#define SAVE_BUTTON_14_RKC					0x0002


// WIFI_PANEL_ID_SETTING_PAGE VP & RKC		25
#define WIFI_PANEL_ID_SETTING_PAGE_VP		0x1015
#define BACK_BUTTON_15_RKC					0x0001
#define SAVE_BUTTON_15_RKC					0x0002


// REC_EMAIL_ID_SET1_PAGE			26
#define REC_EMAIL_ID_SET1_PAGE_VP			0x1016
#define BACK_BUTTON_16_RKC					0x0001
#define SAVE_AND_NEXT_BUTTON_16_RKC			0x0002


// REC_EMAIL_ID_SET2_PAGE			27
#define REC_EMAIL_ID_SET2_PAGE_VP			0x1017
#define BACK_BUTTON_17_RKC					0x0001
#define SAVE_BUTTON_17_RKC					0x0002


// PHONE_NUM_SET1_PAGE				28
#define PHONE_NUM_SET1_PAGE_VP				0x1018
#define BACK_BUTTON_18_RKC					0x0001
#define SAVE_AND_NEXT_BUTTON_18_RKC			0x0002

// PHONE_NUM_SET2_PAGE				29
#define PHONE_NUM_SET2_PAGE_VP				0x1019
#define BACK_BUTTON_19_RKC					0x0001
#define SAVE_BUTTON_19_RKC					0x0002


// SCHEDULING_AND_ESC_TIMING_PAGE	30
#define SCHEDULING_AND_ESC_TIMING_PAGE_VP	0x101A
#define BACK_BUTTON_1A_RKC					0x0001
#define SAVE_BUTTON_1A_RKC					0x0002


esp_err_t dwin_process_frame(struct dwin_frame *dwin_frame);





//Page GUI Handelling Functions

void home_page_handel(struct dwin_frame *dwin_frame);


void event_log_page_handel(struct dwin_frame *dwin_frame);


void setting_page_handel(struct dwin_frame *dwin_frame);


void no_of_io_board_page_handel(struct dwin_frame *dwin_frame);


void io_bord_setting_select_page_handle(struct dwin_frame *dwin_frame);


void io_board_select_page_handel(struct dwin_frame *dwin_frame);


void io_board_name_change_page_handel(struct dwin_frame *dwin_frame);


void io_board_input_setting_page_handel(struct dwin_frame *dwin_frame);


void rs485_input_setting_page_handel(struct dwin_frame *dwin_frame);


void rs485_input_name_change_page_handle(struct dwin_frame *dwin_frame);


void rs485_limit_control_page_handel(struct dwin_frame *dwin_frame);


void analog_input_setting_select_page_handel(struct dwin_frame *dwin_frame);


void analog_input_name_change_page_handel(struct dwin_frame *dwin_frame);


void analog_input_limit_set_page_handel(struct dwin_frame *dwin_frame);


void digital_input_setting1_handel(struct dwin_frame *dwin_frame);


void digital_input_setting2_handel(struct dwin_frame *dwin_frame);


void escalation_matrix_setting_page_handel(struct dwin_frame *dwin_frame);


void outgoing_email_srv_set_page_handel(struct dwin_frame *dwin_frame);


void wifi_panel_id_setting_page_handel(struct dwin_frame *dwin_frame);


void rec_email_id_set1_page_handel(struct dwin_frame *dwin_frame);


void rec_email_id_set2_page_handel(struct dwin_frame *dwin_frame);


void phone_num_set1_page_handel(struct dwin_frame *dwin_frame);


void phone_num_set2_page_handel(struct dwin_frame *dwin_frame);


void scheduling_and_esc_timing_page_handel(struct dwin_frame *dwin_frame);





#endif /* MAIN_DWIN_GUI_HANDLE_H_ */
