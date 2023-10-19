/*
 * gui_vp_page_adr.h
 *
 *  Created on: 08-Oct-2023
 *      Author: vaibh
 */

#ifndef GUI_VP_PAGE_ADR_H_
#define GUI_VP_PAGE_ADR_H_

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>


//Pages Numbers
#define WELLCOME1_PAGE							0
#define WELLCOME2_PAGE							1
#define HOME_PAGE 								2
#define EVENT_LOG_PAGE							3
#define SETTING_PAGE							4
#define NO_OF_IO_BORD_SET_PAGE					5
#define IO_BOARD_SETTING_SEL_PAGE				6
#define IO_BOARD_SELECT_PAGE					7
#define IO_BOARD_NAME_CANGE_PAGE				8
#define IO_BOARD_IP_SETTING_PAGE				9
#define RS485_IP_SETTING_PAGE					10
#define DUMMY_PAGE								11
#define RS485_IP_NAME_CHNGE_PAGE				12
#define RS485_IP_LIMIT_SET_PAGE					13
#define	ANALOG_IP_SET_SEL_PAGE					14
#define ANALOG_IP_NAME_CHANGE_PAGE				15
#define ANALOG_IP_LIMIT_SET_PAGE				16
#define DIGITAL_IP_NAME1_PAGE					17
#define DIGITAL_IP_NAME2_PAGE					22
#define ESCALATION_MATRIX_SETTING_PAGE			23
#define OUTGOING_EMAIL_SRV_SET_PAGE				24
#define WIFI_PANEL_ID_SETTING_PAGE				25
#define REC_EMAIL_ID_SET1_PAGE					26
#define REC_EMAIL_ID_SET2_PAGE					27
#define PHONE_NUM_SET1_PAGE						28
#define PHONE_NUM_SET2_PAGE						29
#define SCHEDULING_AND_ESC_TIMING_PAGE			30


#define GO_TO_HOME_PAGE_RKC						0x0001

//Home Page VP & RKC
#define HOME_PAGE_VP							0x1003
#define SILENCE_BUTTON_RKC						0x0001
#define GO_TO_LOG_PAGE_RKC						0x0002
#define GO_TO_SETTING_PAGE_RKC					0x0003
#define GO_TO_HELP_PAGE_RKC						0x0004

#define GSM_STATUS_TXT_ENT_VP					0x2000
#define WIFI_STATUS_TXT_ENT_VP					0x2010
#define DATE_DISP_TXT_ENT_VP					0x2020
#define TIME_DISP_TXT_ENT_VP					0x2026
#define SYS_STATUS_TXT_ENT1_VP					0x2030
#define SYS_STATUS_TXT_ENT2_VP					0x2040
#define SYS_STATUS_TXT_ENT3_VP					0x2050
#define SYS_STATUS_TXT_ENT4_VP					0x2060


//Event Log Page VP & RKC
#define EVENT_LOG_PAGE_VP						0x1004
#define BACK_BUTTON_4_RKC						0x0001
#define NEXT_BUTTON_4_RKC						0x0002
#define MAX_LOG_PAGES							100

#define LG_SR_NO1__VER_VP						0x2100
#define LG_SR_NO2__VER_VP						0x2101
#define LG_SR_NO3__VER_VP						0x2102
#define LG_SR_NO4__VER_VP						0x2103
#define LG_SR_NO5__VER_VP						0x2104

#define LG_EVENT1_NAME_TXT_ENT_VP				0x2110
#define LG_EVENT2_NAME_TXT_ENT_VP				0x2120
#define LG_EVENT3_NAME_TXT_ENT_VP				0x2130
#define LG_EVENT4_NAME_TXT_ENT_VP				0x2140
#define LG_EVENT5_NAME_TXT_ENT_VP				0x2150

#define LG_EVENT1_DETAILS_TXT_ENT_VP			0x2160
#define LG_EVENT2_DETAILS_TXT_ENT_VP			0x2170
#define LG_EVENT3_DETAILS_TXT_ENT_VP			0x2180
#define LG_EVENT4_DETAILS_TXT_ENT_VP			0x2190
#define LG_EVENT5_DETAILS_TXT_ENT_VP			0x21A0

#define LG_EVENT1_TIMSTMP_TXT_ENT_VP			0x21B0
#define LG_EVENT2_TIMSTMP_TXT_ENT_VP			0x21BB
#define LG_EVENT3_TIMSTMP_TXT_ENT_VP			0x21C6
#define LG_EVENT4_TIMSTMP_TXT_ENT_VP			0x21D1
#define LG_EVENT5_TIMSTMP_TXT_ENT_VP			0x21DC

uint8_t current_log_pg_cn;


// SETTING PAGE VP & RKC
#define SETTING_PAGE_VP							0x1005
#define BACK_BUTTON_5_RKC						0x0001
#define ESCALATION_MATRIX_SET_BUT_RKC			0x0002
#define IO_BOARD_SET_BUTTON_RKC					0x0003


//NO_OF_IO_BORD_SET_PAGE VP & RKC
#define NO_OF_IO_BORD_SET_PAGE_VP				0x1006
#define BACK_BUTTON_6_RKC						0x0001
#define SAVE_And_NEXT_BUTTON_6_RKC				0x0002

#define NO_OF_IO_BRD_VER_VP						0x2200

//IO_BOARD_SETTING_SEL_PAGE VP & RKC
#define IO_BOARD_SETTING_SEL_PAGE_VP			0x1007
#define BACK_BUTTON_7_RKC						0x0001
#define IO_BOARD_NAME_CHANGE_BUTTON_RKC			0x0002
#define IO_BOARD_IP_SETTING_BUTTON_RKC			0x0003

//IO_BOARD_SELECT_PAGE VP & RKC
#define IO_BOARD_SELECT_PAGE_VP					0x1008
#define BACK_BUTTON_8_RKC						0x0001
#define IO_BOARD1_BUTTON_RKC					0x0002
#define IO_BOARD2_BUTTON_RKC					0x0003
#define IO_BOARD3_BUTTON_RKC					0x0004
#define IO_BOARD4_BUTTON_RKC					0x0005

#define IO_BRD1_NAME_TXT_ENT_VP					0x2300
#define IO_BRD2_NAME_TXT_ENT_VP					0x2310
#define IO_BRD3_NAME_TXT_ENT_VP					0x2320
#define IO_BRD4_NAME_TXT_ENT_VP					0x2330

uint8_t selected_io_board;

//IO_BOARD_NAME_CANGE_PAGE VP & RKC
#define IO_BOARD_NAME_CANGE_PAGE_VP				0x1009
#define BACK_BUTTON_9_RKC						0x0001
#define SAVE_BUTTON_9_RKC						0x0002

//IO_BOARD_IP_SETTING_PAGE VP & RKC
#define IO_BOARD_IP_SETTING_PAGE_VP				0x100A
#define BACK_BUTTON_A_RKC						0x0001
#define RS485_IP_SET_PAGE_BUTTON				0x0002
#define ANALOG_IP_SET_PAGE_BUTTON				0x0003
#define DIGITAL_IP_SET_PAGE_BUTTON				0x0004

#define SELECTED_IO_BRD_TXT_ENT_VP				0x2380

//RS485_IP_SETTING_PAGE VP & RKC
#define RS485_IP_SETTING_PAGE_VP				0x100B
#define BACK_BUTTON_B_RKC						0x0001
#define RS485_IP_NAME_CHANGE_BUTTON_RKC			0x0002
#define RS485_IP_LIMIT_BUTTON_RKC				0x0003


//RS485_IP_NAME_CHNGE_PAGE VP & RKC
#define RS485_IP_NAME_CHNGE_PAGE_VP				0x100C
#define BACK_BUTTON_C_RKC						0x0001
#define ANALOG_IP_SET_BUTTON_C_RKC				0x0002
#define DIGITAL_IP_SET_BUTTON_C_RKC				0x0003
#define SAVE_BUTTON_C_RKC						0x0004

#define SENSOR1_TEMP_NAME_TXT_ENT_VP			0x2400
#define SENSOR1_HUM_NAME_TXT_ENT_VP				0x2410
#define SENSOR2_TEMP_NAME_TXT_ENT_VP			0x2420
#define SENSOR2_HUM_NAME_TXT_ENT_VP				0x2430

//RS485_IP_LIMIT_SET_PAGE VP & RKC
#define RS485_IP_LIMIT_SET_PAGE_VP				0x100D
#define BACK_BUTTON_D_RKC						0x0001
#define ANALOG_IP_SET_BUTTON_D_RKC				0x0002
#define DIGITAL_IP_SET_BUTTON_D_RKC				0x0003
#define SAVE_BUTTON_D_RKC						0x0004

#define SENSOR1_TEMP_LIMIT_VER_VP				0x2450
#define SENSOR1_HUM_LIMIT_VER_VP				0x2452
#define SENSOR2_TEMP_LIMIT_VER_VP				0x2454
#define SENSOR2_HUM_LIMIT_VER_VP				0x2456

//ANALOG_IP_SET_SEL_PAGE VP & RKC
#define ANALOG_IP_SET_SEL_PAGE_VP				0x100E
#define BACK_BUTTON_E_RKC						0x0001
#define ANALOG_IP_NAME_SET_BUTTON_RKC			0x0002
#define ANALOG_IP_LIMIT_SET_BUTTON_RKC			0x0003

//ANALOG_IP_NAME_CHANGE_PAGE VP & RKC
#define ANALOG_IP_NAME_CHANGE_PAGE_VP			0x100F
#define BACK_BUTTON_F_RKC						0x0001
#define DIGITAL_IP_SET_BUTTON_F_RKC				0x0002
#define RS485_IP_SET_BUTTON_F_RKC				0x0003
#define SAVE_BUTTON_F_RKC						0x0004

#define ANALOG_IP1_NAME_TXT_ENT_VP				0x2500
#define ANALOG_IP2_NAME_TXT_ENT_VP				0x2510
#define ANALOG_IP3_NAME_TXT_ENT_VP				0x2520
#define ANALOG_IP4_NAME_TXT_ENT_VP				0x2530

//ANALOG_IP_LIMIT_SET_PAGE VP & RKC
#define ANALOG_IP_LIMIT_SET_PAGE_VP				0x1010
#define BACK_BUTTON_10_RKC						0x0001
#define DIGITAL_IP_SET_BUTTON_10_RKC			0x0002
#define RS485_IP_SET_BUTTON_10_RKC				0x0003
#define SAVE_BUTTON_10_RKC						0x0004

#define ANALOG_IP1_LIMIT_VER_VP					0x2550
#define ANALOG_IP2_LIMIT_VER_VP					0x2552
#define ANALOG_IP3_LIMIT_VER_VP					0x2554
#define ANALOG_IP4_LIMIT_VER_VP					0x2556

//DIGITAL_IP_NAME1_PAGE VP & RKC
#define DIGITAL_IP_NAME1_PAGE_VP				0x1011
#define BACK_BUTTON_11_RKC						0x0001
#define ANALOG_IP_BUTTON_11_RKC					0x0002
#define RS485_IP_SET_BUTTON_11_RKC				0x0003
#define SAVE_AND_NEXT_BUTTON_11_RKC				0x0004

#define DIGITAL_IP1_NAME_TXT_ENT_VP				0x2600
#define DIGITAL_IP2_NAME_TXT_ENT_VP				0x2610
#define DIGITAL_IP3_NAME_TXT_ENT_VP				0x2620
#define DIGITAL_IP4_NAME_TXT_ENT_VP				0x2630
#define DIGITAL_IP5_NAME_TXT_ENT_VP				0x2640
#define DIGITAL_IP6_NAME_TXT_ENT_VP				0x2650
#define DIGITAL_IP7_NAME_TXT_ENT_VP				0x2660
#define DIGITAL_IP8_NAME_TXT_ENT_VP				0x2670
#define DIGITAL_IP9_NAME_TXT_ENT_VP				0x2680
#define DIGITAL_IP10_NAME_TXT_ENT_VP			0x2690
#define DIGITAL_IP11_NAME_TXT_ENT_VP			0x26A0
#define DIGITAL_IP12_NAME_TXT_ENT_VP			0x26B0
#define DIGITAL_IP13_NAME_TXT_ENT_VP			0x26C0
#define DIGITAL_IP14_NAME_TXT_ENT_VP			0x26D0
#define DIGITAL_IP15_NAME_TXT_ENT_VP			0x26E0
#define DIGITAL_IP16_NAME_TXT_ENT_VP			0x26F0


//DIGITAL_IP_NAME2_PAGE VP & RKC
#define DIGITAL_IP_NAME2_PAGE_VP	 			0x1012
#define BACK_BUTTON_12_RKC						0x0001
#define ANALOG_IP_BUTTON_12_RKC					0x0002
#define RS485_IP_SET_BUTTON_12_RKC				0x0003
#define SAVE_AND_NEXT_BUTTON_12_RKC				0x0004

//ESCALATION_MATRIX_SETTING_PAGE VP & RKC	23
#define ESCALATION_MATRIX_SETTING_PAGE_VP		0x1013
#define BACK_BUTTON_13_RKC						0x0001
#define SMS_SCHEDULE_BUTTON_13_RKC				0x0002
#define MOBILE_NUM_SET_BUTTON_13_RKC			0x0003
#define REC_EMAILID_SET_BUTTON_13_RKC			0x0004
#define SEND_EMAILID_SET_BUTTON_13_RKC			0x0005
#define WIFI_AND_PANEL_ID_SET_BUTTON_13_RKC 	0x0006


//OUTGOING_EMAIL_SRV_SET_PAGE VP & RKC		24
#define OUTGOING_EMAIL_SRV_SET_PAGE_VP			0x1014
#define BACK_BUTTON_14_RKC						0x0001
#define SAVE_BUTTON_14_RKC						0x0002

#define EMAIL_SERV_TXT_ENT_VP					0x2700
#define SENDER_EMAIL_ID_TXT_ENT_VP				0x2720
#define SENDER_PASSWORD_TXT_ENT_VP				0x2740
#define SENDER_NAME_TXT_ENT_VP					0x2750

#define EMAIL_PORT_NUM_VER_VP					0x2760


// WIFI_PANEL_ID_SETTING_PAGE VP & RKC		25
#define WIFI_PANEL_ID_SETTING_PAGE_VP			0x1015
#define BACK_BUTTON_15_RKC						0x0001
#define SAVE_BUTTON_15_RKC						0x0002

#define WIFI_SSID_TXT_ENT_VP					0x2800
#define WIFI_PASSWORD_TXT_ENT_VP				0x2810
#define DEVICE_ID_NAME_TXT_ENT_VP				0x2820

// REC_EMAIL_ID_SET1_PAGE			26
#define REC_EMAIL_ID_SET1_PAGE_VP				0x1016
#define BACK_BUTTON_16_RKC						0x0001
#define SAVE_AND_NEXT_BUTTON_16_RKC				0x0002

#define REC1_EMAI_ID_TXT_ENT_VP					0x2900
#define REC2_EMAI_ID_TXT_ENT_VP					0x291A
#define REC3_EMAI_ID_TXT_ENT_VP					0x2934
#define REC4_EMAI_ID_TXT_ENT_VP					0x294E
#define REC5_EMAI_ID_TXT_ENT_VP					0x2968
#define REC6_EMAI_ID_TXT_ENT_VP					0x2982
#define REC7_EMAI_ID_TXT_ENT_VP					0x299C
#define REC8_EMAI_ID_TXT_ENT_VP					0x29B6
#define REC9_EMAI_ID_TXT_ENT_VP					0x29D0
#define REC10_EMAI_ID_TXT_ENT_VP				0x29EA



// REC_EMAIL_ID_SET2_PAGE			27
#define REC_EMAIL_ID_SET2_PAGE_VP				0x1017
#define BACK_BUTTON_17_RKC						0x0001
#define SAVE_BUTTON_17_RKC						0x0002


// PHONE_NUM_SET1_PAGE				28
#define PHONE_NUM_SET1_PAGE_VP					0x1018
#define BACK_BUTTON_18_RKC						0x0001
#define SAVE_AND_NEXT_BUTTON_18_RKC				0x0002

#define REC1_PHONE_NUM_TXT_ENT_VP				0x2A00
#define REC2_PHONE_NUM_TXT_ENT_VP				0x2A10
#define REC3_PHONE_NUM_TXT_ENT_VP				0x2A20
#define REC4_PHONE_NUM_TXT_ENT_VP				0x2A30
#define REC5_PHONE_NUM_TXT_ENT_VP				0x2A40
#define REC6_PHONE_NUM_TXT_ENT_VP				0x2A50
#define REC7_PHONE_NUM_TXT_ENT_VP				0x2A60
#define REC8_PHONE_NUM_TXT_ENT_VP				0x2A70
#define REC9_PHONE_NUM_TXT_ENT_VP				0x2A80
#define REC10_PHONE_NUM_TXT_ENT_VP				0x2A90


// PHONE_NUM_SET2_PAGE				29
#define PHONE_NUM_SET2_PAGE_VP					0x1019
#define BACK_BUTTON_19_RKC						0x0001
#define SAVE_BUTTON_19_RKC						0x0002


// SCHEDULING_AND_ESC_TIMING_PAGE	30
#define SCHEDULING_AND_ESC_TIMING_PAGE_VP		0x101A
#define BACK_BUTTON_1A_RKC						0x0001
#define SAVE_BUTTON_1A_RKC						0x0002


#endif /* GUI_VP_PAGE_ADR_H_ */
