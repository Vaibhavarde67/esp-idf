/*
 * display_function.c
 *
 *  Created on: 15-Oct-2022
 *      Author: Embin 7
 */

//#include "font.h"
#include "display_function.h"
#include "constTextDisp.h"




static const char *TAG = "DISPLAY FUNCTION";

 uint8_t SPIScanId=0;



 /*
  * Function to Handel Timer Interrupt for Display Refresh Time
  * We
 */
static void IRAM_ATTR disp_ref_timer_callback(void *args)
{
// 	static uint8_t c =0;
//
// 	if(c==0)
// 		c=1;
// 	else
// 		c=0;
// 	gpio_set_level(19, c);
	//ets_printf("in ref time\n");
	//timer_spinlock_take(DIS_TIMER_GRP);
 	//Clear the Timer Interrupt Status and Flag bits
	//timer_pause(SCR_TIMER_GRP, SCR_TIMER_ID);

	timer_pause(SCR_TIMER_GRP, SCR_TIMER_ID);

 	timer_group_clr_intr_status_in_isr(DIS_TIMER_GRP, DIS_TIMER_ID);
 	// Timer's counter will get reset
 	timer_set_counter_value(DIS_TIMER_GRP, DIS_TIMER_ID, 0);
 	//Start Timer

 	timer_group_enable_alarm_in_isr(DIS_TIMER_GRP, DIS_TIMER_ID);
 	//timer_set_alarm_value(DIS_TIMER_GRP, DIS_TIMER_ID, DIS_REFRESH_TIME);
 	DispFunction_scanDisplayBySPI();

 	timer_start(DIS_TIMER_GRP, DIS_TIMER_ID);
 	timer_start(SCR_TIMER_GRP, SCR_TIMER_ID);
 	//timer_start(SCR_TIMER_GRP, SCR_TIMER_ID);



 	//timer_spinlock_give(DIS_TIMER_GRP);

 	//Start Timer
 	//timer_start(DIS_TIMER_GRP, DIS_TIMER_ID);
 	//ets_printf("out ref time\n");
 	//return pdTRUE;
}

/*
  * Function to Handel Timer Interrupt for Display Scroll Time
  *
 */
static void IRAM_ATTR disp_scroll_timer_callback(void *args)
{

// 	static uint8_t c =0;
//
// 	if(c==0)
// 		c=1;
// 	else
// 		c=0;
// 	gpio_set_level(19, c);
	//timer_spinlock_take(SCR_TIMER_GRP);

	//Pause refresh timer
	//timer_pause(DIS_TIMER_GRP, DIS_TIMER_ID);

 	//Clear the Timer Interrupt Status and Flag bits

 	timer_group_clr_intr_status_in_isr(SCR_TIMER_GRP, SCR_TIMER_ID);
 	// Timer's counter will get reset
 	timer_set_counter_value(SCR_TIMER_GRP, SCR_TIMER_ID, 0);

// 	for(uint8_t i = 0; i< no_row; i++)
// 	{
// 		if((display_prograns[current_program].prg_lines[i].cur_scr_pos <= (display_prograns[current_program].prg_lines[i].end_pos-32)))
// 		{
// 			display_prograns[current_program].prg_lines[i].cur_scr_pos++;
// 		}
// 		else
// 		{
// 			display_prograns[current_program].prg_lines[i].cur_scr_pos = 0;
// 			duplicate_the_disp_array();
// 			return;
// 		}
// 	}

 	//gpio_set_level(19, 1);
	//ets_printf("in scroll task\n");
	uint16_t i=0,j=0,l=0;
	int end_pos;

	//Pause refresh timer
	//timer_pause(DIS_TIMER_GRP, DIS_TIMER_ID);

	//loop to go through Lines
	selc_row++;
	if(selc_row == no_row)
		selc_row = 0;


	if((display_prograns[current_program].prg_lines[selc_row].cur_scr_pos != (display_prograns[current_program].prg_lines[selc_row].end_pos-32)))
	{
		display_prograns[current_program].prg_lines[selc_row].cur_scr_pos++;
	}
	else
	{
		display_prograns[current_program].prg_lines[selc_row].cur_scr_pos = 0;
		//ets_printf("Vaibhav Here\n");
		for(uint8_t v=0; v!=MAX_LINE_VERT_REG; v++)
		{
			for(uint8_t b=0 ; b != 16 ; b++)                   //This is because we have 16bit data
			{
				Pixel_Data2[selc_row][v][b] = Pixel_Data[selc_row][v][b];
			}
		}
		goto end;
	}


	l = selc_row;

//	for(l=0; l< no_row; l++)
//	{
		if(display_prograns[current_program].prg_lines[l].scroll)
		{
			if(display_prograns[current_program].prg_lines[l].type==simple)
			{

				i = 0;
				end_pos = MAX_LINE_VERT_REG-1;
				while(i!=end_pos)    //This loop is for shift register columns
				{
					for(j=0 ; j != 16 ; j++)                   //This is because we have 16bit data
					{
//						Pixel_Data2[l][i][j] = (Pixel_Data2[l][i][j]<<1);
//						Pixel_Data2[l][i][j] |= (Pixel_Data2[l][i+1][j]>>15);
						Pixel_Data2[l][i][j] = (Pixel_Data2[l][i][j])*2;
						Pixel_Data2[l][i][j] |= (Pixel_Data2[l][i+1][j]/32768);
					}
					i++;
				}
				//for last column
				for(j=0 ; j != 16 ; j++)                   //This is because we have 16bit data
				{
//					Pixel_Data2[l][i][j] = (Pixel_Data2[l][i][j]<<1);
					Pixel_Data2[l][i][j] = (Pixel_Data2[l][i][j])*2;
				}


			}
			else
			{
				i = 2;
				end_pos = vert_reg_for_row-3;
				//ets_printf("For Line = %d Start pos = %d, End pos %d\n",l,i,end_pos);
				while(i<(end_pos))    //This loop is for shift register columns
				{
					for(j=0 ; j != 16 ; j++)                   //This is because we have 16bit data
					{
//						Pixel_Data2[l][i][j] = (Pixel_Data2[l][i][j]<<1);
//						Pixel_Data2[l][i][j] |= (Pixel_Data2[l][i+1][j]>>15);
						Pixel_Data2[l][i][j] = (Pixel_Data2[l][i][j])*2;
						Pixel_Data2[l][i][j] |= (Pixel_Data2[l][i+1][j]/32768);
					}
					i++;
				}
				//for last column do nothing
				for(j=0 ; j != 16 ; j++)                   //This is because we have 16bit data
				{
//					Pixel_Data2[l][i][j] = (Pixel_Data2[l][i][j]<<1);
//					Pixel_Data2[l][i][j] |= (Pixel_Data2[l][i+3][j]>>15);
					Pixel_Data2[l][i][j] = (Pixel_Data2[l][i][j]*2);
					Pixel_Data2[l][i][j] |= (Pixel_Data2[l][i+3][j]/32768);
				}
				i +=3;
				//for buffers
				end_pos = MAX_LINE_VERT_REG-1;
				while(i!=end_pos)    //This loop is for shift register columns
				{
					for(j=0 ; j != 16 ; j++)                   //This is because we have 16bit data
					{
//						Pixel_Data2[l][i][j] = (Pixel_Data2[l][i][j]<<1);
//						Pixel_Data2[l][i][j] |= (Pixel_Data2[l][i+1][j]>>15);
						Pixel_Data2[l][i][j] = (Pixel_Data2[l][i][j]*2);
						Pixel_Data2[l][i][j] |= (Pixel_Data2[l][i+1][j]/32768);
					}
					i++;
				}
				for(j=0 ; j != 16 ; j++)                   //This is because we have 16bit data
				{
//					Pixel_Data2[l][i][j] = (Pixel_Data2[l][i][j]<<1);
					Pixel_Data2[l][i][j] = (Pixel_Data2[l][i][j]*2);
				}
				//ets_printf("Line = %d  i = %d , j = %d\n",l,i,j);
			}

		}


//	}


	//timer_set_counter_value(SCR_TIMER_GRP, SCR_TIMER_ID, 0);
end:    timer_start(SCR_TIMER_GRP, SCR_TIMER_ID);
	//Start refresh timer
 	//timer_start(DIS_TIMER_GRP, DIS_TIMER_ID);

 	timer_group_enable_alarm_in_isr(SCR_TIMER_GRP, SCR_TIMER_ID);
 	//timer_spinlock_give(SCR_TIMER_GRP);

}


void p6_display_init(uint8_t row, uint8_t col)
{
	sk = 0;
	selc_row = 0;
	//Setting Direction of pins used for p6 display
	//gpio_set_direction(PIN_P6_nOE, GPIO_MODE_OUTPUT);
	gpio_set_direction(PIN_P6_A, GPIO_MODE_OUTPUT);
	gpio_set_direction(PIN_P6_B, GPIO_MODE_OUTPUT);
	gpio_set_direction(PIN_P6_SCLK, GPIO_MODE_OUTPUT);
	gpio_set_direction(PIN_P6_R_DATA, GPIO_MODE_OUTPUT);

	//ForTest
//	gpio_set_direction(19, GPIO_MODE_OUTPUT);
//	gpio_set_level(19, 0);

	//Setting level of pins
	gpio_set_level(PIN_P6_A, 0);
	gpio_set_level(PIN_P6_B, 0);
	gpio_set_level(PIN_P6_SCLK, 0);
	//gpio_set_level(PIN_P6_nOE, 1);
	gpio_set_level(PIN_P6_R_DATA, 0);

	//Setting Display mode
	//Initialize Display Variable
	no_row = row;							//Aswe are going to have one row in Scrolling Display
	no_col = col;
	leds_in_a_line = col*32;
	disp_array_rows = no_row*no_col*2;
	no_brisk = no_col*no_row;
	no_of_vert_reg = (no_brisk+no_row)*2;		//For Buffer of Scrolling
	no_of_vert_reg2 = no_col*2;
	vert_reg_for_row = (no_col)*2;
	total_column_dis = no_of_vert_reg*16;
	total_column_dis_row = no_col*32+32;		//+32 for buffer brisk
	skip_pos = (no_col)*2+1;

//	ets_printf("no_of_vert_reg = %d\n",no_of_vert_reg);
//	ets_printf("disp_array_rows = %d\n",disp_array_rows);
//	Pixel_Data = malloc(7*30*16);
//	Pixel_Data2 = malloc(7*30*16);

	buffer_array_cursor = (no_col)*32;
	//PixelData = malloc(sizeof(uint16_t[no_of_vert_reg][16]));	//Allocate a block of size bytes of memory, returning a pointer to the beginning of the block.  Use calloc() to do same but zero initialise the buffer
	//(TAG,"Alloted size = %d",sizeof(uint16_t[no_of_vert_reg][16]));
	//memset(PixelData, 0, n*sizeof(arr[0]));
	//Initializing SPI

	spi_bus_config_t buscfg={
				.miso_io_num=NOT_USED,
				.mosi_io_num=PIN_P6_R_DATA,
				.sclk_io_num=PIN_P6_CLK,
				.quadwp_io_num=NOT_USED,
				.quadhd_io_num=NOT_USED,
			   //.max_transfer_sz= buffer_array_cursor,
			};

	spi_device_interface_config_t devcfg={
			.command_bits = 0,
			.address_bits = 0,
			.clock_speed_hz = CONFIG_P6_SPI_CLOCK_MHZ *1000 *1000,		//20MHz
			.mode=0,                                //SPI mode 0
			.spics_io_num=NOT_USED,               //CS pin
			.flags = 0,
			.queue_size = 2,
			.duty_cycle_pos= 100,
		};


	spi_bus_initialize(P6_HOST, &buscfg, SPI_DMA_DISABLED);

	spi_bus_add_device(P6_HOST, &devcfg, &disp_spi);
	//******************************************************************************

	//Initialize the Timer Required for Display Refresh operation (to send each line out of 4)
	//Select and initialize basic parameters of the timer
	//We will be not using auto reload facility
	timer_config_t config = {
								.divider = DIS_TIMER_DIVIDER,
								.counter_dir = TIMER_COUNT_UP,
								.counter_en = TIMER_PAUSE,
								.alarm_en = TIMER_ALARM_EN,
								.auto_reload = TIMER_AUTORELOAD_DIS,
							}; // default clock source is APB

	// Initaliaze Timer required for frame end detection in MODBUS
	timer_init(DIS_TIMER_GRP, DIS_TIMER_ID, &config);

	// Timer's counter will initially start from value below.
	timer_set_counter_value(DIS_TIMER_GRP, DIS_TIMER_ID, 0);


	// Configure the alarm value and the interrupt on alarm.
	timer_set_alarm_value(DIS_TIMER_GRP, DIS_TIMER_ID, DIS_REFRESH_TIME);

	//Adding Callback function for Timer interrupt
	//timer_isr_callback_add(DIS_TIMER_GRP, DIS_TIMER_ID, disp_ref_timer_callback, NULL, 0);
	timer_isr_register(DIS_TIMER_GRP, DIS_TIMER_ID, disp_ref_timer_callback, NULL, 0,NULL);

	//Creating Semaphore for detecting frame
	//refresh_disp = xSemaphoreCreateBinary();

	//Creating Event group to handel Display related Event
	display_events = xEventGroupCreate();

	//Setting Display Array
	//set_disp_array();

	//Enable interrupt
	timer_enable_intr(DIS_TIMER_GRP, DIS_TIMER_ID);
	//Start Timer
	timer_start(DIS_TIMER_GRP, DIS_TIMER_ID);

	//Enabling Timer For Scrolling Task
	timer_config_t config2 = {
									.divider = SCR_TIMER_DIVIDER,
									.counter_dir = TIMER_COUNT_UP,
									.counter_en = TIMER_PAUSE,
									.alarm_en = TIMER_ALARM_EN,
									.auto_reload = TIMER_AUTORELOAD_DIS,
								}; // default clock source is APB
	timer_init(SCR_TIMER_GRP, SCR_TIMER_ID, &config2);
	timer_set_counter_value(SCR_TIMER_GRP, SCR_TIMER_ID, 0);
	timer_set_alarm_value(SCR_TIMER_GRP, SCR_TIMER_ID, SCROLLING_TIME);
	//timer_isr_callback_add(SCR_TIMER_GRP, SCR_TIMER_ID, disp_scroll_timer_callback, NULL, 0);
	timer_isr_register(SCR_TIMER_GRP, SCR_TIMER_ID, disp_scroll_timer_callback, NULL, 0,NULL);
	timer_enable_intr(SCR_TIMER_GRP, SCR_TIMER_ID);
	//timer_start(SCR_TIMER_GRP, SCR_TIMER_ID);

	/*
	 * Prepare and set configuration of timers
	 * that will be used by Intensity Controller
	 */
	ledc_timer_config_t ledc_timer =
	{
		.duty_resolution = LEDC_TIMER_8_BIT, 	// resolution of PWM duty
		.freq_hz = 1000,                      // frequency of PWM signal
		.speed_mode = INTENSITY_HS_MODE,           // timer mode
		.timer_num = INTENCITY_HS_TIMER,            // timer index
		.clk_cfg = LEDC_AUTO_CLK,              // Auto select the source clock
	};

	// Set configuration of timer0 for high speed channels
	ledc_timer_config(&ledc_timer);

	/*
	 * - controller's channel number
	 * - output duty cycle, set initially to 0
	 * - GPIO number where LED is connected to
	 * - speed mode, either high or low
	 * - timer servicing selected channel
	 *   Note: if different channels use one timer,
	 *         then frequency and bit_num of these channels
	 *         will be the same
	 */

	ledc_channel_config_t ledc_channel =
	{
			.channel    = INTENSITY_HS_CH0_CHANNEL,
			.duty       = 0,
			.gpio_num   = INTENSITY_HS_CH0_GPIO,
			.speed_mode = INTENSITY_HS_MODE,
			.hpoint     = 0,
			.timer_sel  = INTENCITY_HS_TIMER,
	};

	// Set LED Controller with previously prepared configuration
	 ledc_channel_config(&ledc_channel);

	 //Set Intencity Display befor setting alarm for intencityt
	set_display_intencity(45);
	ledc_set_duty(INTENSITY_HS_MODE,INTENSITY_HS_CH0_CHANNEL, intensity_duty);
	ledc_update_duty(INTENSITY_HS_MODE,INTENSITY_HS_CH0_CHANNEL);

	//Initializing variable for start
	//OE_ON = pdTRUE;
	ROW_ID = 0;
	trans.length = 1*8;	//as we are sending one character at atime
	return;
}

/*
 * Function to control Intensity of Display
 */
void set_display_intencity(uint8_t int_per)
{
	int duty_val;
	if(int_per < MIN_INTENCITY)
		int_per = MIN_INTENCITY;
	else if(int_per > MAX_INTENCITY)
		int_per = MAX_INTENCITY;
	duty_val = int_per * 2.55;
	intensiy_per = int_per;
	intensity_duty = duty_val;
	//ledc_set_duty(INTENSITY_HS_MODE,INTENSITY_HS_CH0_CHANNEL, duty_val);
	//ledc_update_duty(INTENSITY_HS_MODE,INTENSITY_HS_CH0_CHANNEL);
	//ESP_LOGE(TAG,"Array Size = %d", NO_OF_VERT_REG_COL);
}






//void IRAM_ATTR DispFunction_PassBitmemToDisp(uint8_t line)
//{
//	uint8_t byte=0;
//	int i=0,j=0,k=0,l=0,m=0;
//
//	i= 3-line;
//	uint16_t skip_cn =0;
//	spi_device_acquire_bus(disp_spi, portMAX_DELAY);
//	//gpio_set_level(19, 1);
//	for(j=0 ; j< no_of_vert_reg; j++)    //This loop is for shift register columns
//	{
//		skip_cn++;
//		if(skip_cn == skip_pos)
//		{
//			j +=1;
//			skip_cn = 0;
//			//ets_printf("got at j = %d",j);
//			continue;
//		}
//
//		for(m=1 ; m >= 0 ; m--)                   //This is because we have 16bit data
//		{
//			l=15-i;
//			for(k=3 ; k >= 0 ; k--)           //This is for 1 74595 Column
//			{
//				//
//				if(m)
//				{
//					byte = ~(PixelData[j][l] >> 8);
//					//ESP_LOGE(TAG, "byte = %x", PixelData[j][l]);
//
//				}
//				else
//				{
//					byte = ~(PixelData[j][l]);
//					//ESP_LOGE(TAG, "byte = %x", byte);
//				}
//				//ets_printf("[%d][%d]\n", j,l);
//				l -=4;
//				//gpio_set_level(19, 1);
//				//spi_device_acquire_bus(disp_spi, portMAX_DELAY);
//				trans.tx_buffer = &byte;
//
//				spi_device_polling_transmit(disp_spi, &trans);                 //Send register location
//				//spi_device_release_bus(disp_spi);
//				//cn++;
//				//gpio_set_level(19, 0);
//			}
//		}
//	}
//	//gpio_set_level(19, 0);
//	spi_device_release_bus(disp_spi);
//}


void IRAM_ATTR DispFunction_PassBitmemToDisp(uint8_t line)
{
	uint8_t byte=0;
	int i=0,j=0,k=0,l=0,m=0, n=0;

	i= 3-line;
	trans.tx_buffer = &byte;

	spi_device_acquire_bus(disp_spi, portMAX_DELAY);
	//gpio_set_level(19, 1)
	for(n=0; n!=no_row; n++)
	{
//		div = display_prograns[current_program].prg_lines[n].cur_scr_pos / 16;
//		rem = display_prograns[current_program].prg_lines[n].cur_scr_pos % 16;
		for(j=0; j!= no_of_vert_reg2; j++)    //This loop is for shift register columns
		{

			for(m=1; m >= 0 ; m--)                   //This is because we have 16bit data
			{
				l=15-i;
				for(k=3 ; k >= 0 ; k--)           //This is for 1 74595 Column
				{
					//
					if(m)
					{
//						byte = ~(Pixel_Data2[n][j][l] >> 8);
						byte = ~(Pixel_Data2[n][j][l] /256);
						//ESP_LOGE(TAG, "byte = %x", PixelData[j][l]);

					}
					else
					{
						byte = ~(Pixel_Data2[n][j][l]);
						//ESP_LOGE(TAG, "byte = %x", byte);
					}
					//ets_printf("[%d][%d]\n", j,l);
					l -=4;
					//gpio_set_level(19, 1);
					//spi_device_acquire_bus(disp_spi, portMAX_DELAY);
//					trans.tx_buffer = &byte;

					spi_device_polling_transmit(disp_spi, &trans);                 //Send register location
					//spi_device_release_bus(disp_spi);
					//cn++;
					//gpio_set_level(19, 0);
				}
			}
		}
	}
	//gpio_set_level(19, 0);
	spi_device_release_bus(disp_spi);
}


void IRAM_ATTR DispFunction_scanDisplayBySPI()
{
	//if(current_dis_mode == static_display)
	//gpio_set_level(19, 1);
	DispFunction_PassBitmemToDisp(SPIScanId);
	ledc_timer_pause(INTENSITY_HS_MODE, INTENCITY_HS_TIMER);
	ledc_timer_rst(INTENSITY_HS_MODE, INTENCITY_HS_TIMER);
	ledc_stop(INTENSITY_HS_MODE, INTENSITY_HS_CH0_CHANNEL, 0);
	//gpio_set_level(19, 0);
	//else if(current_dis_mode == scrolling_display)
		//DispFunction_PassBitmemToDisp(SPIScanId, PixelDataScr);
	//gpio_set_level(PIN_P6_SCLK, 1);

	switch (SPIScanId)
	{
		case 0: // row 1, 5, 9, 13 were clocked out
			LIGHT_P6_ROW_01_05_09_13();
			SPIScanId = 1;
			break;
		case 1: // row 2, 6, 10, 14 were clocked out
			LIGHT_P6_ROW_02_06_10_14();
			SPIScanId = 2;
			break;
		case 2: // row 3, 7, 11, 15 were clocked out
			LIGHT_P6_ROW_03_07_11_15();
			SPIScanId = 3;
			break;
		case 3: // row 4, 8, 12, 16 were clocked out
			LIGHT_P6_ROW_04_08_12_16();
			SPIScanId = 0;
			break;
	}
	//delayMicroseconds(10);

	//OE_P6_ROWS_OFF();
	//Giving Latch Signal

	//ledc_set_duty(INTENSITY_HS_MODE,INTENSITY_HS_CH0_CHANNEL, 0);
	//ledc_update_duty(INTENSITY_HS_MODE,INTENSITY_HS_CH0_CHANNEL);
	gpio_set_level(PIN_P6_SCLK, 1);
//	for(int l=0;l<100; l++)
//	{
//
//	}
	gpio_set_level(PIN_P6_SCLK, 0);
	//OE_P6_ROWS_ON();
	ledc_timer_resume(INTENSITY_HS_MODE, INTENCITY_HS_TIMER);
	ledc_set_duty(INTENSITY_HS_MODE,INTENSITY_HS_CH0_CHANNEL, intensity_duty);
	ledc_update_duty(INTENSITY_HS_MODE,INTENSITY_HS_CH0_CHANNEL);
	//OE_P6_ROWS_ON();
}
