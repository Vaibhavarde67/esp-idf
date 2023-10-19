/*
 * dwin_gui_handle.h
 *
 *  Created on: 21-Sep-2023
 *      Author: Embin_RD
 */

#ifndef MAIN_DWIN_GUI_HANDLE_H_
#define MAIN_DWIN_GUI_HANDLE_H_

#include "dwinlcd_functions.h"
#include "critical_alarm_data_structure.h"
#include "gui_vp_page_adr.h"



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
