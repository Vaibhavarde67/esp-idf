/*
 * define.h
 *
 *  Created on: 14-Oct-2022
 *      Author: Embin 7
 */

//in develope 2



#ifndef DEFINE_H_
#define DEFINE_H_

#include "driver/gpio.h"
#include "driver/timer.h"


//#define ENGLISH_SMALL	1
//#define ENGLISH_BIG		1
//#define HINDI			1
//#define MALYALAM		1
///*#define AKRUTI_DEV_MOUJ 1*/
//#define APS_FONTS		1

#define HIGH	1
#define LOW		0

#define TRUE	1
#define FALSE	0


#define SCROLL_CHARACTER_LIMIT				150

#define MAX_FOTA_RETRY_COUNT	3
#define MAX_FOTA_PACKET_RETRY	60


uint8_t mac[6];
char mac_str[20];

//Variable for Display
volatile uint16_t sk;

uint8_t no_row, no_col, no_brisk, no_of_vert_reg, no_of_vert_reg2, disp_array_rows;
uint16_t buffer_array_cursor, array_size, total_column_dis, total_column_dis_row;
uint16_t leds_in_a_line, vert_reg_for_row,skip_pos;

uint8_t current_program, selected_program;
uint16_t current_program_cnt;
int intensiy_per, intensity_duty;
bool nothing_to_disp;


////Initalizing macros required for Display Scroll timer
//#define SCR_TIMER_DIVIDER	80-1
//#define	SCR_TIMER_GRP		TIMER_GROUP_1
//#define SCR_TIMER_ID		TIMER_1
////Enter Display refresh time in microsecond now in our case it is 4Msec
//#define SCROLLING_TIME	 	7000-1		//Actually we get stable output at 100MSec interval




//Variable to store Pixel data for Display
//uint16_t (*PixelData)[16];// = (uint16_t *)malloc(NO_OF_VERT_REG_COL_SCR * sizeof(uint16_t));
//uint16_t PixelData[170][16];

volatile uint16_t Pixel_Data[7][30][16];
volatile uint16_t Pixel_Data2[7][30][16];
volatile uint8_t selc_row;
volatile uint8_t column_cn[7];
volatile uint8_t crnt_col_sel[7];

volatile uint8_t scr_cn;
volatile uint8_t scr_sel;

uint8_t disp_init, time_init, fota_init, graph_init;

//uint16_t (*Pixel_Data)[30][16];
//uint16_t (*Pixel_Data2)[30][16];

#define MAX_LINE_LIMIT		464
#define MAX_LINE_VERT_REG	30

#define MAX_DISPLAY_PROGRAMS	14

//Variable to decide Static or Scrolling Display
enum display_mode {
					static_display,
					scrolling_display
				  }current_dis_mode;

//Enum to indicate mode of a program
enum dis_mode {
				offline=0,
				online
				};

//Enum to indicate language of a program
enum language {
				en=1,
				hi,
				ml
				};

//Enum to indicate font for a perticular program
enum font {
			EnglishBig=1,
			EnglishSmall
			};

//Enum to store type of data to be displayed on a line
enum line_data_type {
					 simple=1,
					 column
					};

//Structure to store a line data
struct display_line
{
			enum line_data_type type;
			uint8_t graphics;
			uint8_t text[256];
			uint8_t msg_len;
			bool scroll;
			uint16_t scr_pos;
			uint16_t end_pos;
			uint16_t cur_scr_pos;
			int route_no;
			uint16_t eta;
			int16_t prv_orgin_point;
			int16_t prv_lenhth;
			};

//Structure to store the various Program data that we are going to display on Display pannel
struct display_prg {
					  uint8_t prg_id;
					  bool prg_status;
					  enum dis_mode prg_mode;
					  uint8_t prg_repeat_cnt;
					  uint8_t prg_bright;
					  uint16_t prg_sec;
					  enum language dis_lang;
					  enum font prg_font;
					  uint64_t prg_exp_tm;
					  struct display_line prg_lines[7];		//As in our case we have maximum 7 line display
					}display_prograns[MAX_DISPLAY_PROGRAMS+1];

//Array to store string to be display
volatile uint8_t display_ln[7][256];

//Initalizing macros required for Display refresh timer
#define DIS_TIMER_DIVIDER	80-1
#define	DIS_TIMER_GRP		TIMER_GROUP_1
#define DIS_TIMER_ID		TIMER_0//Timer_Group_0 is not available when we use Ethernet module
//Enter Display refresh time in microsecond now in our case it is 4Msec
#define DIS_REFRESH_TIME	3000-1		//Actually we get stable output at 5MSec interval
				  //We can give value of 1000


//esp32 pins used for the display connection
#define PIN_P6_nOE       32
#define PIN_P6_A         33
#define PIN_P6_B         27 //in Final PCB
//#define PIN_P6_B         25
#define PIN_P6_CLK       14
#define PIN_P6_SCLK      26
#define PIN_P6_R_DATA    13


//SPI interface used for display in esp32
#define P6_HOST    VSPI_HOST
//SPi data transfer speed
#define CONFIG_P6_SPI_CLOCK_MHZ	18	//As per 74HC595 Datasheet max 21MHz

#define NOT_USED		-1


//For Inyencity Control
#define MIN_INTENCITY		20
#define MAX_INTENCITY		60
#define INTENCITY_HS_TIMER          LEDC_TIMER_1
#define INTENSITY_HS_MODE           LEDC_HIGH_SPEED_MODE
#define INTENSITY_HS_CH0_GPIO       32
#define INTENSITY_HS_CH0_CHANNEL    LEDC_CHANNEL_0



//DMD I/O pin macros
#define LIGHT_P6_ROW_01_05_09_13()       { gpio_set_level( PIN_P6_B,  LOW ); gpio_set_level( PIN_P6_A,  LOW ); }
#define LIGHT_P6_ROW_02_06_10_14()       { gpio_set_level( PIN_P6_B,  LOW ); gpio_set_level( PIN_P6_A, HIGH ); }
#define LIGHT_P6_ROW_03_07_11_15()       { gpio_set_level( PIN_P6_B, HIGH ); gpio_set_level( PIN_P6_A,  LOW ); }
#define LIGHT_P6_ROW_04_08_12_16()       { gpio_set_level( PIN_P6_B, HIGH ); gpio_set_level( PIN_P6_A, HIGH ); }
#define LATCH_P6_SHIFT_REG_TO_OUTPUT()   { gpio_set_level( PIN_P6_SCLK, HIGH ); gpio_set_level( PIN_P6_SCLK,  LOW ); }


#define OE_P6_ROWS_OFF()                 { gpio_set_level( PIN_P6_nOE, LOW); }       //For in house P10 display designs. (Inverted control for OE pin)
#define OE_P6_ROWS_ON()                  { gpio_set_level( PIN_P6_nOE, HIGH); }

#define BRIGHTNESS_CTRL_ENABLE           0

//Display Error code
#define SERVER_ERROR						56

#define ETHERNET_MODULE_ERROR				66
#define DID_NOT_GET_IP_ADDRESS				67

#define GSM_MODULE_ERROR					76

#define FOTA_SUCESS							81



//Variable required for Scrolling
uint8_t scr_msg[SCROLL_CHARACTER_LIMIT];
uint16_t scr_msg_len, current_pos;
//bool msg_end, scr_start;
uint16_t current_cursor, current_cursor2;
const short int *scr_font;


/*
 * Defining Fonts
 */
const short int englishbig[4000];
const uint16_t english_small[2000];
const short int uni_hindi[4000];
//const short int hindi[4000];
const short int uni_malyalam[4000];

#define NO_OF_HINDI_COMBINATION		8
const uint8_t uni_hindi_compound_check[NO_OF_HINDI_COMBINATION][3];
const short int uni_hindi_compound_combi[11][12];
const short int uni_hindi_compound[500];

#define NO_OF_MALYALAM_COMBINATION		35
const uint8_t uni_malyalam_compound_check[NO_OF_MALYALAM_COMBINATION][3];
const short int uni_malyalam_compound_combi[132][12];
const short int uni_malyalam_compound[4000];

#define STS_LED1_PIN					19
#define STS_LED2_PIN					21



#endif /* MAIN_DEFINE_H_ */
