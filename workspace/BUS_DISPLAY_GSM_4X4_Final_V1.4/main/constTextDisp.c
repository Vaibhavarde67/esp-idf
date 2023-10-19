/*
 * constTextDisp.c
 *
 *  Created on: 17-Oct-2022
 *      Author: Embin 7
 */


#include "graphics.h"
#include "constTextDisp.h"
//#include "esp_log.h"

static const char *TAG = "CnstTextDisp";


void Rotatefont(uint16_t Buffptr[][16],int index,uint8_t dataStartPos,const short int *font)
{
	uint16_t fontVal=0;
	uint8_t i=0,j=0;
	unsigned int convertedfont=0;

	font +=index;
	for(i=0 ; i<=15 ; i++)
	{
		convertedfont=0;
		for(j=0 ; j<=15 ; j++)
		{
			fontVal = font[j];
			if(fontVal & (1<<i))
			{
				convertedfont |= 1 << (15-j);
			}
		}
		Buffptr[0][i] = convertedfont >> dataStartPos;
		Buffptr[1][i] = convertedfont << (16-dataStartPos) ;
	}
}

/*
 * Function to Load Graphics into Memory
 */
uint16_t constGraphics_LoadInBitMem(uint8_t graph_index, uint16_t curser, uint8_t line_no)
{
	signed char cur_off;
	uint16_t BitmemIndex,i=0,j=0;
	uint16_t Charindex=0;
	uint16_t tempAddr, temporaryBuff[2][16]={0};
	line_no +=1;
	graph_index -=1;
	tempAddr = curser-1;
	cur_off = graphics_data[graph_index][0];
	//printf("cursr of =%d",cur_off);


	if((tempAddr+cur_off) > (MAX_LINE_LIMIT-1))            //return in case exceeding actual disp width +32 for buffer
			return curser;

	BitmemIndex = (tempAddr%16);
	Charindex = tempAddr/16;

	//Load Graphics in temparery buffer
	for(i=1 ; i<=16 ; i++)
	{

		temporaryBuff[0][i-1] = graphics_data[graph_index][i] >> BitmemIndex;
		temporaryBuff[1][i-1] = graphics_data[graph_index][i] << (16-BitmemIndex);
	}
	for(i=0 ; i<= 1; i++)
	{
		for(j=0 ; j<16 ; j++)
		{
			//PixelData[i+Charindex][j] |=temporaryBuff[i][j];
			Pixel_Data[line_no-1][i+Charindex][j] |=temporaryBuff[i][j];

			//printf("Data to load at[%d][%d]=%x\n",i,j,temporaryBuff[i][j]);
			//ets_printf("index = [%d][%d]\n",i+Charindex,j);
		}
	}
	curser += cur_off+4;		//4 Line for Buffer

	return (curser);

}


uint16_t constText_CharLoadInBitMem(uint8_t Character, uint16_t curser, const short int *font, uint8_t line_no)
{
	uint16_t BitmemIndex,i=0,j=0;
	uint16_t Charindex=0;
	int16_t st_off,cur_off, org_point, type_char;						//if we declair it as uint8_ t then it create issue
	int16_t prv_org_point = display_prograns[current_program].prg_lines[line_no].prv_orgin_point, prv_len = display_prograns[current_program].prg_lines[line_no].prv_lenhth;
	uint16_t tempAddr, temporaryBuff[2][16]={0};
	line_no +=1;		//as line number start from 0 instead of 1

	if((Character > 0xcf) || (curser==0))
		return curser;

	tempAddr = curser-1;
	//Character = Character - 0x20;		//This is due to new unicode mapping

	Charindex = (uint16_t)Character * 20;          //rd notes: 16 text bytes and 4 some info bytes

	org_point = font[Charindex++];
	st_off = font[Charindex++];    				  //rd notes: reading 2nd and 3rd values in 4 character info data.
	cur_off = font[Charindex++];
	type_char = font[Charindex++];


	if(type_char == 2)
	{
		tempAddr -= prv_len - prv_org_point + org_point;
		if((tempAddr+cur_off+st_off) > MAX_LINE_LIMIT)            //return in case exceeding actual disp width +32 for buffer
			return curser;
	}
	else
	{
		if((tempAddr+cur_off+st_off) > MAX_LINE_LIMIT)            //return in case exceeding actual disp width +32 for buffer
			return curser;
		tempAddr += st_off;
	}

	BitmemIndex = (tempAddr%16);


	Rotatefont(temporaryBuff,Charindex,BitmemIndex,font);
	Charindex = tempAddr/16;


    for(i=0 ; i<= 1; i++)
    {
		for(j=0 ; j<16 ; j++)
		{
			//PixelData[i+Charindex][j] |=temporaryBuff[i][j] ;
			Pixel_Data[line_no-1][i+Charindex][j] |=temporaryBuff[i][j];
			//ets_printf("index = [%d][%d]\n",i+Charindex,j);
		}
    }
    if(type_char != 2)
    {
    	curser += cur_off;
		//prv_org_point = org_point;
    	display_prograns[current_program].prg_lines[line_no-1].prv_orgin_point = org_point;
		//prv_len = cur_off;
    	display_prograns[current_program].prg_lines[line_no-1].prv_lenhth = cur_off;
    }

//	return (curser+1);
	return (curser);
}

/*
 * Function to Load 8x8 font
 */

uint16_t constSmallText_CharLoadInBitMem(char Character, uint16_t curser, const uint16_t *font, uint8_t line_no, uint8_t sub_line)
{
	uint16_t BitmemIndex,i=0,j=0;
	uint16_t Charindex=0;
	signed char cur_off;						//if we declair it as uint8_ t then it create issue
	uint16_t tempAddr, temporaryBuff[2][8]={0};
	line_no +=1;		//as line number start from 0 instead of 1

	if((Character > 0xcf) || (Character < 0x20) || (curser==0))
		return curser;

	tempAddr = curser-1;
	//Character = Character - 0x20;		//This is commented due to new unicode logic

	Charindex = (uint16_t)Character * 9;          //rd notes: 16 text bytes and 4 some info bytes

	cur_off = font[Charindex];
	//printf("width =%d\n", cur_off);


	if((tempAddr+cur_off) > (leds_in_a_line))            //return in case exceeding actual disp width +32 for buffer
		return curser;


	BitmemIndex = (tempAddr%16);

	//Load Graphics in temparery buffer
	for(i=1 ; i<=8 ; i++)
	{
		temporaryBuff[0][i-1] = (font[Charindex+i]<<8) >> (BitmemIndex);
		temporaryBuff[1][i-1] = (font[Charindex+i]<<8) << (16-BitmemIndex);

	}

	//Rotatefont(temporaryBuff,Charindex,BitmemIndex,font);
	Charindex = tempAddr/16;
	if(sub_line==0)
	{
		for(i=0 ; i<= 1; i++)
		{
			for(j=0 ; j<8 ; j++)
			{
				//PixelData[i+Charindex][j] |=temporaryBuff[i][j];
				Pixel_Data[line_no-1][i+Charindex][j] |=temporaryBuff[i][j];
				//printf("Data to load at[%d][%d]=%x\n",i,j,temporaryBuff[i][j]);
				//ets_printf("index = [%d][%d]\n",i+Charindex,j);
			}
		}
	}
	else
	{
		for(i=0 ; i<= 1; i++)
		{
			for(j=0 ; j<8 ; j++)
			{
				//PixelData[i+Charindex][j+8] |=temporaryBuff[i][j];
				Pixel_Data[line_no-1][i+Charindex][j+8] |=temporaryBuff[i][j];
				//printf("Data to load at[%d][%d]=%x\n",i,j,temporaryBuff[i][j]);
				//ets_printf("index = [%d][%d]\n",i+Charindex,j);
			}
		}
	}
	curser += cur_off;		//4 Line for Buffer

	return (curser);
}


/*
 * Function to Display Clear Display
 */
void clear_p6_display_data(void)
{
	memset(column_cn, 0, 7);
	memset(crnt_col_sel, 0, 7);
	scr_cn = 0;
	scr_sel = 0;
	for(uint8_t i=0 ; i< no_row; i++)    //This loop is for shift register columns
	{
		for(uint8_t j=0; j<MAX_LINE_VERT_REG;j++)
		{
			for(uint8_t k=0 ; k < 16 ; k++)                   //This is because we have 16bit data
			{

				Pixel_Data[i][j][k] = 0;
				Pixel_Data2[i][j][k] = 0;
			}
		}
	}
}




void IRAM_ATTR duplicate_the_disp_array(uint8_t row_no)
{
	for(uint8_t j=0; j<MAX_LINE_VERT_REG;j++)
	{
		for(uint8_t k=0 ; k < 16 ; k++)                   //This is because we have 16bit data
		{
			Pixel_Data2[row_no][j][k] = Pixel_Data[row_no][j][k];
		}
	}
}



/*
 * Function to check for presence of Compound Words
 */
bool check_for_compound_word(uint8_t *data, uint8_t line, uint8_t *data_pos, uint8_t *line_pos)
{
	uint16_t comb_index, comb_no, fnd_comb_index, no_ofchar;
	bool combi_found = FALSE;
	uint8_t pos;
	if(data[*data_pos] == 0x09)
	{
		for(uint8_t i = 0; i<NO_OF_HINDI_COMBINATION; i++)
		{
			if(uni_hindi_compound_check[i][0]==data[*data_pos+1])
			{
				//printf("Initial Letter = %x match\n",data[*data_pos+1]);
				comb_index = uni_hindi_compound_check[i][1];
				comb_no = uni_hindi_compound_check[i][2];
				for(uint8_t j = comb_index; j<(comb_index+comb_no); j++)
				{
					combi_found = FALSE;
					pos = *data_pos;
					for(uint8_t l = 2; (l<12)&&(uni_hindi_compound_combi[j][l]>0);l++)
					{

						//printf("data to compare = %x  %x\n",data[pos],uni_hindi_compound_combi[j][l]);
						if(data[pos] != uni_hindi_compound_combi[j][l])
						{
							combi_found = FALSE;
							break;
						}
						else
							combi_found = TRUE;
						pos++;
					}
					if(combi_found)
					{
						//printf("Hindi Compound Word found\n");
						*data_pos = pos;
						fnd_comb_index = uni_hindi_compound_combi[j][1];
						no_ofchar = uni_hindi_compound_combi[j][0];
						char next_char = data[pos+1];
						if(uni_hindi[next_char*20+3] == 3)
						{
							display_ln[line][*line_pos] = 0x09;
							display_ln[line][*line_pos+1] = next_char;
							*data_pos = pos+2;
							*line_pos +=2;
						}
						for(uint8_t k = 0; k<no_ofchar; k++)
						{
							display_ln[line][*line_pos] = 0x19;
							display_ln[line][*line_pos+1] = fnd_comb_index;
							fnd_comb_index++;

							//printf("Hindi Compound Word loaded at %d  and %d lang char =%x and font char = %x\n", *line_pos, line,display_ln[line][*line_pos],display_ln[line][*line_pos+1]);
							*line_pos +=2;
						}
						return TRUE;
					}
				}
				break;
			}
		}
		return FALSE;

	}
	else if(data[*data_pos] == 0x0d)
	{
		for(uint8_t i = 0; i<NO_OF_MALYALAM_COMBINATION; i++)
		{
			if(uni_malyalam_compound_check[i][0]==data[*data_pos+1])
			{
				comb_index = uni_malyalam_compound_check[i][1];
				comb_no = uni_malyalam_compound_check[i][2];
				for(uint8_t j = comb_index; j<(comb_index+comb_no); j++)
				{
					combi_found = FALSE;
					pos = *data_pos;
					for(uint8_t l = 2; (l<12)&&(uni_malyalam_compound_combi[j][l]>0);l++)
					{

						//printf("data to compare = %x  %x\n",data[pos],uni_malyalam_compound_combi[j][l]);
						if(data[pos] != uni_malyalam_compound_combi[j][l])
						{
							combi_found = FALSE;
							break;
						}
						else
							combi_found = TRUE;
						pos++;
					}
					if(combi_found)
					{
						//printf("Malm Compound Found\n");
						*data_pos = pos;
						fnd_comb_index = uni_malyalam_compound_combi[j][1];
						no_ofchar = uni_malyalam_compound_combi[j][0];

						char next_char = data[pos+1];
						uint8_t char_told = 0x00;
						if(uni_malyalam[next_char*20+3] == 5)
						{
							//before and after matra
							if((next_char == 0x4A) || (next_char == 0x4b))
							{
								display_ln[line][*line_pos] = 0x0d;
								display_ln[line][*line_pos+1] = next_char;
								*data_pos = pos+2;
								*line_pos +=2;
								char_told = 0x3e;
							}
							else if(next_char == 0x4c)
							{
								display_ln[line][*line_pos] = 0x0d;
								display_ln[line][*line_pos+1] = next_char;
								*data_pos = pos+2;
								*line_pos +=2;
								char_told = 0x57;
							}
						}
						else if(uni_malyalam[next_char*20+3] == 3)
						{
							//before char matra
							display_ln[line][*line_pos] = 0x0d;
							display_ln[line][*line_pos+1] = next_char;
							*data_pos = pos+2;
							*line_pos +=2;
						}

						for(uint8_t k = 0; k<no_ofchar; k++)
						{
							display_ln[line][*line_pos] = 0x1d;
							display_ln[line][*line_pos+1] = fnd_comb_index;
							fnd_comb_index++;
							//printf("Malyalam Compound Word loaded at %d  and %d lang char =%x and font char = %x\n", *line_pos, line, display_ln[line][*line_pos],display_ln[line][*line_pos+1]);
							*line_pos +=2;
						}

						if(char_told > 0)
						{
							display_ln[line][*line_pos] = 0x0d;
							display_ln[line][*line_pos+1] = char_told;
							*data_pos = pos+2;
							*line_pos +=2;
						}

						return TRUE;
					}
				}
				break;
			}
		}
		return FALSE;

	}
	return FALSE;
}


/*
 * Function to write Data to line
 */
void Cnstdata_writeLinetiDispMem(char* ptr, uint8_t line, const short int *font)
{
	uint8_t i;
	char presentChar=0;
	scr_font = font;
	i=0;

	current_cursor=line*(leds_in_a_line+32)+1;
	while(ptr[i] != '\0')
	{

		presentChar = ptr[i];
		//ESP_LOGE(TAG, "Data=%c",presentChar);
		current_cursor = constText_CharLoadInBitMem(presentChar,current_cursor , font, line);
		i++;
	}

}


/*
 * Function to Load Program Data On Screen
 */

void Cnstdata_writeProgramIntoDispMem( struct display_prg* prgm)
{
	uint8_t i;
	uint16_t text_start, text_end, max_display_size;
	uint16_t text_start2, text_end2, max_display_size2;
	unsigned char presentChar, nextChar, nextChar2, nextChar3;

//	else
//	{
//		//Return Here Some Error Code
//	}

	i=0;
	clear_p6_display_data();
	int dis_size, Charindex;
	uint8_t k;
	if(prgm->prg_font == EnglishBig)
	{
		for(uint8_t j =0; j< no_row; j++)
		{
			if(prgm->dis_lang == en)
				scr_font = englishbig;
			else if(prgm->dis_lang == hi)
				scr_font = uni_hindi;
			else
				scr_font = uni_malyalam;

			dis_size = 0;
			Charindex = 0;
			k = 0;
			text_start = j*(leds_in_a_line+32)+1;
			text_start2 = 0;

			if(prgm->prg_lines[j].type == simple)
			{
				max_display_size = leds_in_a_line;
				//buffer_array_cursor = (no_brisk)*32;
			}
			else
			{
				max_display_size = leds_in_a_line-64; 		// As in this case we will be using 2 brisk to display route_no and ETA
				//buffer_array_cursor = (no_col)*32;
				char route_no_str[4]; //= (prgm->prg_lines[j].route_no).toString();
				strcpy(route_no_str, prgm->prg_lines[j].route_no);

	//			if(prgm->prg_lines[j].route_no<10)
	//				route_no_str = "  "+route_no_str;
	//			else if(prgm->prg_lines[j].route_no<100)
	//				route_no_str = " "+route_no_str;

				dis_size = 0;
				k = 0;
				while(route_no_str[k] != '\0')
				{
//					if(prgm->dis_lang == hi)
//					{
//						route_no_str[k] += 54;	//This is due to unicode mapping
//					}
					//As per new requirment they dont want malyalam numbers
//					else if(prgm->dis_lang == ml)
//					{
//						route_no_str[k] += 54;//need to do
//					}
					Charindex = (uint16_t)route_no_str[k]* 20;          //rd notes: 16 text bytes and 4 some info bytes
					Charindex += 2;
					//if(scr_font == uni_malyalam)
						dis_size += englishbig[Charindex];
					//else
						//dis_size += scr_font[Charindex];
					k++;
				}
				current_cursor= text_start+(30-dis_size);
				current_cursor2= text_start2+(30-dis_size);//To leave 8 led line after route no and to get right line data
				i = 0;
				while(route_no_str[i] != '\0')
				{

					presentChar = route_no_str[i];
					//ESP_LOGE(TAG, "Data=%c",presentChar);
					//if(scr_font == uni_malyalam)
						current_cursor2 = constText_CharLoadInBitMem(presentChar,current_cursor2 , englishbig, j);
					//else
						//current_cursor2 = constText_CharLoadInBitMem(presentChar,current_cursor2 , scr_font, j);
					i++;
				}

				char eta_str[10];// = (prgm->prg_lines[j].eta).toString();
				if(prgm->prg_lines[j].eta>9)
					sprintf(eta_str, "%d", prgm->prg_lines[j].eta);
				else
					sprintf(eta_str, "0%d", prgm->prg_lines[j].eta);
	//			if(prgm->prg_lines[j].eta<10)
	//				eta_str = " 0"+eta_str;
				dis_size = 0;
				k = 0;
				while(eta_str[k] != '\0')
				{
//					if(prgm->dis_lang == hi)
//					{
//						eta_str[k] += 54;	//This is due to unicode mapping
//					}
					//As per client requirement
//					else if(prgm->dis_lang == ml)
//					{
//						eta_str[k] += 54;//need to do
//					}
					Charindex = (uint16_t)(eta_str[k])* 20;          //rd notes: 16 text bytes and 4 some info bytes
					Charindex += 2;
					//if(scr_font == uni_malyalam)
						dis_size += englishbig[Charindex];
					//else
						//dis_size += scr_font[Charindex];
					k++;
				}
				current_cursor= text_start + leds_in_a_line-32+(32-dis_size)/2;
				current_cursor2= text_start2 + leds_in_a_line-32+(32-dis_size)/2;
				i = 0;
				while(eta_str[i] != '\0')
				{

					presentChar = eta_str[i];
					//ESP_LOGE(TAG, "Data=%c",presentChar);
					//if(scr_font == uni_malyalam)
						current_cursor2 = constText_CharLoadInBitMem(presentChar,current_cursor2 , englishbig, j);
					//else
						//current_cursor2 = constText_CharLoadInBitMem(presentChar,current_cursor2 , scr_font, j);
					i++;
				}

				text_start += 32;
				text_start2 += 32;
			}

			dis_size = 0;
			k = 0;		//To start from data not the language identifier
			prgm->prg_lines[j].msg_len = 0;
			if(prgm->prg_lines[j].graphics>0)
				dis_size += graphics_data[prgm->prg_lines[j].graphics][0]+4;		//For Buffer

			//Finding Area taken by Message
//			while(prgm->prg_lines[j].text[k] != '\0')
//			{
//				Charindex = (uint16_t)(prgm->prg_lines[j].text[k+1] )* 20;          //rd notes: 16 text bytes and 4 some info bytes
//				Charindex += 2;
//				if(prgm->prg_lines[j].text[k] == 0x01)
//					dis_size += englishbig[Charindex];
//				else if(prgm->prg_lines[j].text[k] == 0x09)
//					dis_size += uni_hindi[Charindex];
//				else if(prgm->prg_lines[j].text[k] == 0x0d)
//					dis_size += malyalam[Charindex];
//				k += 2;
//				prgm->prg_lines[j].msg_len += 2;
//			}


			while(prgm->prg_lines[j].text[k] != '\0')
			{
				//printf("Char = %x\n",prgm->prg_lines[j].text[k]);
				if((prgm->prg_lines[j].text[k] == 0x01)&&(prgm->prg_lines[j].text[k+1]<0x80))		//0x80 is limit for English
				{
					display_ln[j][prgm->prg_lines[j].msg_len]= 0x01;
					display_ln[j][prgm->prg_lines[j].msg_len+1]= prgm->prg_lines[j].text[k+1];
					k += 2;
					prgm->prg_lines[j].msg_len +=2;
				}

				else if((prgm->prg_lines[j].text[k] == 0x09)&&(prgm->prg_lines[j].text[k+1]<0xaf))
				{
					if(!(check_for_compound_word(prgm->prg_lines[j].text,j,&k, &prgm->prg_lines[j].msg_len)))
					{
						presentChar = prgm->prg_lines[j].text[k+1];
						nextChar = prgm->prg_lines[j].text[k+3];
						nextChar2 = prgm->prg_lines[j].text[k+5];
						nextChar3 = prgm->prg_lines[j].text[k+7];

						if(uni_hindi[nextChar*20+3] == 3)
						{
							display_ln[j][prgm->prg_lines[j].msg_len]= 0x09;
							display_ln[j][prgm->prg_lines[j].msg_len+1]= nextChar;
							display_ln[j][prgm->prg_lines[j].msg_len+2]= 0x09;
							display_ln[j][prgm->prg_lines[j].msg_len+3] = presentChar;
							k += 2;
							prgm->prg_lines[j].msg_len +=2;

						}
						else if(nextChar == 0x4d)
						{
							if(uni_hindi[nextChar3*20+3] == 3)
							{
								display_ln[j][prgm->prg_lines[j].msg_len]= 0x09;
								display_ln[j][prgm->prg_lines[j].msg_len+1]= nextChar3;
								k += 2;
								prgm->prg_lines[j].msg_len +=2;

							}
							if(presentChar == 0x30)
							{
								presentChar += 0x6B;
								display_ln[j][prgm->prg_lines[j].msg_len]= 0x09;
								display_ln[j][prgm->prg_lines[j].msg_len+1]= nextChar2;
								display_ln[j][prgm->prg_lines[j].msg_len+2]= 0x09;
								display_ln[j][prgm->prg_lines[j].msg_len+3] = presentChar;

								k += 4;
								prgm->prg_lines[j].msg_len +=2;
							}
							else if(nextChar2 == 0x30)
							{
								display_ln[j][prgm->prg_lines[j].msg_len]= 0x09;
								display_ln[j][prgm->prg_lines[j].msg_len+1]= presentChar;
								display_ln[j][prgm->prg_lines[j].msg_len+2]= 0x09;
								display_ln[j][prgm->prg_lines[j].msg_len+3] = nextChar2+0x6B+1;

								k += 4;
								prgm->prg_lines[j].msg_len +=2;
							}
							else
							{
								presentChar += 0x6B;
								display_ln[j][prgm->prg_lines[j].msg_len]= 0x09;
								display_ln[j][prgm->prg_lines[j].msg_len+1]= presentChar;
								display_ln[j][prgm->prg_lines[j].msg_len+2]= 0x09;
								display_ln[j][prgm->prg_lines[j].msg_len+3]= nextChar2;
								k += 4;
								prgm->prg_lines[j].msg_len +=2;
							}

							//dis_size--;
						}
						else
						{

							display_ln[j][prgm->prg_lines[j].msg_len]= 0x09;
							display_ln[j][prgm->prg_lines[j].msg_len+1]= presentChar;
						}

						k += 2;
						prgm->prg_lines[j].msg_len +=2;
						//printf("size = %d\n",dis_size);
					}
					else
						printf("Value of msg len = %d and k = %d\n",prgm->prg_lines[j].msg_len, k);

				}
				else if((prgm->prg_lines[j].text[k] == 0x0d)&&(prgm->prg_lines[j].text[k+1]< 0x80))
				{
//					printf("Loading Malyalam char\n");
					if(!(check_for_compound_word(prgm->prg_lines[j].text,j,&k, &prgm->prg_lines[j].msg_len)))
					{

						presentChar = prgm->prg_lines[j].text[k+1];
						nextChar = prgm->prg_lines[j].text[k+3];
						nextChar2 = prgm->prg_lines[j].text[k+5];
						nextChar3 = prgm->prg_lines[j].text[k+7];
						if(uni_malyalam[nextChar*20+3] == 5)
						{
							//printf("Before After Matra\n");
							//before and after matra
							if((nextChar == 0x4A) || (nextChar == 0x4b))
							{
								display_ln[j][prgm->prg_lines[j].msg_len++]= 0x0d;
								display_ln[j][prgm->prg_lines[j].msg_len++]= nextChar;
								display_ln[j][prgm->prg_lines[j].msg_len++]= 0x0d;
								display_ln[j][prgm->prg_lines[j].msg_len++] = presentChar;
								display_ln[j][prgm->prg_lines[j].msg_len++]= 0x0d;
								display_ln[j][prgm->prg_lines[j].msg_len++] = 0x3e;
								k += 4;
							}
							else if(nextChar == 0x4c)
							{
								display_ln[j][prgm->prg_lines[j].msg_len++]= 0x0d;
								display_ln[j][prgm->prg_lines[j].msg_len++]= nextChar;
								display_ln[j][prgm->prg_lines[j].msg_len++]= 0x0d;
								display_ln[j][prgm->prg_lines[j].msg_len++] = presentChar;
								display_ln[j][prgm->prg_lines[j].msg_len++]= 0x0d;
								display_ln[j][prgm->prg_lines[j].msg_len++] = 0x57;
								k += 4;
							}
						}
						else if(uni_malyalam[nextChar*20+3] == 3)
						{
							//before char matra
							//printf("Before matra\n");
							display_ln[j][prgm->prg_lines[j].msg_len]= 0x0d;
							display_ln[j][prgm->prg_lines[j].msg_len+1]= nextChar;
							display_ln[j][prgm->prg_lines[j].msg_len+2]= 0x0d;
							display_ln[j][prgm->prg_lines[j].msg_len+3] = presentChar;
							k += 4;
							prgm->prg_lines[j].msg_len +=4;
						}
						else
						{
							//printf("Normal Malyalam char\n");
							display_ln[j][prgm->prg_lines[j].msg_len]= 0x0d;
							display_ln[j][prgm->prg_lines[j].msg_len+1] = presentChar;
							k +=2;
							prgm->prg_lines[j].msg_len +=2;
						}

					}
				}
				//ets_printf("line = %d, cursor = %d\n",j,current_cursor);

			}
			display_ln[j][prgm->prg_lines[j].msg_len]= '\0';

			uint8_t g = 0;
			while(g< prgm->prg_lines[j].msg_len)
			{
				//("at %d is = %x\n",g,display_ln[j][g]);
//				if(display_ln[j][g]==0x4b)
//					display_ln[j][g] = 0x34;
				g +=1;
			}

			k=0;
			signed char st_off,cur_off, org_point, type_char, temp;						//if we declair it as uint8_ t then it create issue
			signed char prv_org_point = 0, prv_len = 0;
			const short int *font;
			while(display_ln[j][k]!='\0')
			{

				if(display_ln[j][k]==0x01)
					font = englishbig;
				else if(display_ln[j][k]==0x09)
					font = uni_hindi;
				else if(display_ln[j][k]==0x19)
					font = uni_hindi_compound;
				else if(display_ln[j][k]==0x0d)
					font = uni_malyalam;
				else
					font = uni_malyalam_compound;

				presentChar = display_ln[j][k+1];
				Charindex = (uint16_t)presentChar * 20;          //rd notes: 16 text bytes and 4 some info bytes

				org_point = font[Charindex++];
				st_off = font[Charindex++];    				  //rd notes: reading 2nd and 3rd values in 4 character info data.
				cur_off = font[Charindex++];
				type_char = font[Charindex++];

				if(type_char == 2)
				{
					temp = prv_len - prv_org_point + org_point;
					if(cur_off > temp)
						dis_size += cur_off-temp;
				}
				else
				{
					dis_size += cur_off;
					prv_org_point = org_point;
					prv_len = cur_off;
				}

				k += 2;
				//printf(" Line = %d is %d \n",j, dis_size);
			}
			//printf(" Line = %d is %d \n",j, dis_size);

			if(dis_size > max_display_size)
			{
				prgm->prg_lines[j].scroll = TRUE;
				prgm->prg_lines[j].scr_pos = 0;
				prgm->prg_lines[j].cur_scr_pos = 0;
				prgm->prg_lines[j].end_pos = buffer_array_cursor+16;
				if(prgm->prg_lines[j].graphics>0)
					prgm->prg_lines[j].end_pos = constGraphics_LoadInBitMem(prgm->prg_lines[j].graphics, prgm->prg_lines[j].end_pos, j);

				while((display_ln[j][prgm->prg_lines[j].scr_pos] != '\0') && (prgm->prg_lines[j].end_pos < MAX_LINE_LIMIT))
				{

					if(display_ln[j][prgm->prg_lines[j].scr_pos] == 0x01)
					{
						scr_font = englishbig;
						presentChar = display_ln[j][prgm->prg_lines[j].scr_pos+1];
						prgm->prg_lines[j].end_pos = constText_CharLoadInBitMem(presentChar,prgm->prg_lines[j].end_pos , scr_font, j);
					}
					//ESP_LOGE(TAG, "Data=%c",presentChar);
					else if(display_ln[j][prgm->prg_lines[j].scr_pos] == 0x09)
					{
						scr_font = uni_hindi;
						presentChar = display_ln[j][prgm->prg_lines[j].scr_pos+1];
						//ESP_LOGE(TAG, "Data=%x",presentChar);
						prgm->prg_lines[j].end_pos = constText_CharLoadInBitMem(presentChar,prgm->prg_lines[j].end_pos , scr_font, j);
					}
					else if(display_ln[j][prgm->prg_lines[j].scr_pos] == 0x19)
					{
						scr_font = uni_hindi_compound;
						presentChar = display_ln[j][prgm->prg_lines[j].scr_pos+1];
						prgm->prg_lines[j].end_pos = constText_CharLoadInBitMem(presentChar,prgm->prg_lines[j].end_pos , scr_font, j);
					}
					else if(display_ln[j][prgm->prg_lines[j].scr_pos] == 0x0d)
					{
						scr_font = uni_malyalam;
						presentChar = display_ln[j][prgm->prg_lines[j].scr_pos+1];
						prgm->prg_lines[j].end_pos = constText_CharLoadInBitMem(presentChar,prgm->prg_lines[j].end_pos , scr_font, j);
					}
					else if(display_ln[j][prgm->prg_lines[j].scr_pos] == 0x1d)
					{
						scr_font = uni_malyalam_compound;
						presentChar = display_ln[j][prgm->prg_lines[j].scr_pos+1];
						prgm->prg_lines[j].end_pos = constText_CharLoadInBitMem(presentChar,prgm->prg_lines[j].end_pos , scr_font, j);
					}
					prgm->prg_lines[j].scr_pos +=2;
					//ESP_LOGI(TAG, "Line = %d Loading Char = %d, Cursor = %d",j, display_ln[j][prgm->prg_lines[j].scr_pos+1], prgm->prg_lines[j].end_pos);
				}
				//timer_start(SCR_TIMER_GRP, SCR_TIMER_ID);
			}
			else
			{
				text_start2 += (max_display_size-dis_size)/2;		//for center allign
				prgm->prg_lines[j].scroll = FALSE;
				i = 0;
				current_cursor2 = text_start2;
				if(prgm->prg_lines[j].graphics>0)
				{
					current_cursor2 = constGraphics_LoadInBitMem(prgm->prg_lines[j].graphics, current_cursor2, j);
					//printf("Printed Graphics =\n");
				}
				while(display_ln[j][i] != '\0')
				{
					if(display_ln[j][i] == 0x01)
					{
						scr_font = englishbig;
						presentChar = display_ln[j][i+1];
						//printf("Printing Char = %c\n",presentChar);
						current_cursor2 = constText_CharLoadInBitMem(presentChar,current_cursor2 , scr_font, j);
						i += 2;
					}
					//ESP_LOGE(TAG, "Data=%c",presentChar);
					else if(display_ln[j][i] == 0x09)
					{
						scr_font = uni_hindi;
						presentChar = display_ln[j][i+1];
						current_cursor2 = constText_CharLoadInBitMem(presentChar,current_cursor2 , scr_font, j);
						i += 2;
					}
					else if(display_ln[j][i] == 0x19)
					{
						scr_font = uni_hindi_compound;
						presentChar = display_ln[j][i+1];

						current_cursor2 = constText_CharLoadInBitMem(presentChar,current_cursor2 , scr_font, j);
						i += 2;
					}
					else if(display_ln[j][i] == 0x0d)
					{
						scr_font = uni_malyalam;
						presentChar = display_ln[j][i+1];
						//printf("Printing Char = %x\n",presentChar);
						current_cursor2 = constText_CharLoadInBitMem(presentChar,current_cursor2 , scr_font, j);
						i += 2;
					}
					else if(display_ln[j][i] == 0x1d)
					{
						scr_font = uni_malyalam_compound;
						presentChar = display_ln[j][i+1];
						//printf("Printing Char2 = %x\n",presentChar);
						current_cursor2 = constText_CharLoadInBitMem(presentChar,current_cursor2 , scr_font, j);
						i += 2;
					}

					//ets_printf("line = %d, cursor = %d\n",j,current_cursor);

				}
			}

		}
	}

	//Have to do
	else
	{
		for(uint8_t j =0; j< no_row; j++)
		{
			uint16_t new_line_index = 0;
			uint16_t space_index = 0;
			Charindex = 0;
			dis_size = 0;
			k = 0;
			text_start = 1;
			prgm->prg_lines[j].scroll = FALSE;

			while(prgm->prg_lines[j].text[k] != '\0')
			{
				if(prgm->prg_lines[j].text[k] == ' ')
					space_index = k;
				Charindex = (uint16_t)(prgm->prg_lines[j].text[k] )* 9;          //rd notes: 16 text bytes and 4 some info bytes
				dis_size += english_small[Charindex];
				if((dis_size > leds_in_a_line)&&(new_line_index==0))
					new_line_index = space_index;
				k++;
			}
			if(dis_size < leds_in_a_line)
				text_start += (leds_in_a_line-dis_size)/2;

			i = 0;
			current_cursor = text_start;
			while(prgm->prg_lines[j].text[i] != '\0')
			{
				presentChar =prgm->prg_lines[j].text[i];
				//ESP_LOGE(TAG, "Data=%c",presentChar);
				if((new_line_index>0)&&(i>=new_line_index))
				{
					current_cursor = constSmallText_CharLoadInBitMem(presentChar, current_cursor ,english_small, j, 1);
					//ets_printf("Here vaibh\n");
				}
				else
				{
					current_cursor = constSmallText_CharLoadInBitMem(presentChar, current_cursor ,english_small, j, 0);

				}
				//ets_printf("character = %c line = %d, cursor = %d\n",presentChar,j,current_cursor);
				i++;
				if(i==new_line_index)
				{
					current_cursor = text_start;
					i++;
				}
			}

		}
	}

	for(uint8_t h=0;h<no_row;h++)
		duplicate_the_disp_array(h);

}

/*
 * Function To Load Dummy Data
 */

void load_dummy_data(void)
{
	//ets_printf("Loaded data Succefully\n");

	for(uint8_t i=0; i<14; i++)
	{

		display_prograns[i].prg_id = i;
		display_prograns[i].prg_status = FALSE;
		display_prograns[i].prg_mode = online;
		display_prograns[i].prg_repeat_cnt = 1;
		display_prograns[i].prg_bright = 60;
		display_prograns[i].prg_sec = 0;
		display_prograns[i].dis_lang = en;
		display_prograns[i].prg_font = EnglishBig;
		display_prograns[i].prg_exp_tm = 0;
		for(uint8_t j=0; j<7; j++)
		{
			display_prograns[i].prg_lines[j].type = column;
			display_prograns[i].prg_lines[j].graphics = 0;



			//For Teting

//			display_prograns[i].prg_lines[j].eta = 210+j;
//			display_prograns[i].prg_lines[j].route_no = 250+j;
//
//			uint8_t b;
//			for(b=0; b<61; b++)
//			{
//				display_prograns[i].prg_lines[j].text[b] = 9;
//				display_prograns[i].prg_lines[j].text[b+1] = 65+b/2;
//				b++;
//			}
//
//			//Required for testing of English Small font

//			//display_prograns[i].prg_lines[j].text[b/2] = 1;
//			//display_prograns[i].prg_lines[j].text[b/2+1] =32;

//			display_prograns[i].prg_lines[j].text[b] = '\0';



			display_prograns[i].prg_lines[j].text[0] = '\0';
			display_prograns[i].prg_lines[j].scroll = 0;
		}
	}
	display_prograns[14].prg_id = 14;
	display_prograns[14].prg_status = TRUE;
	display_prograns[14].prg_mode = offline;
	display_prograns[14].prg_repeat_cnt = 1;
	display_prograns[14].prg_bright = 60;
	display_prograns[14].prg_sec = 0;
	display_prograns[14].dis_lang = en;
	display_prograns[14].prg_font = EnglishBig;
	display_prograns[14].prg_exp_tm = 9999999999;
	for(uint8_t j=0; j<7; j++)
	{
		display_prograns[14].prg_lines[j].type = simple;
		display_prograns[14].prg_lines[j].graphics = 0;
		display_prograns[14].prg_lines[j].text[0] = '\0';
		display_prograns[14].prg_lines[j].scroll = 0;
	}
	char temp[] = "Smart City Thiruvananthapuram limited";
	int h = 0, l =0;
	while(temp[h]!='\0')
	{
		display_prograns[14].prg_lines[0].text[l++] = 0x01;
		display_prograns[14].prg_lines[0].text[l++] = temp[h++];
	}
	display_prograns[14].prg_lines[0].scroll = 1;
	display_prograns[14].prg_lines[0].text[l] = '\0';

//	display_prograns[0].prg_id = 0;
//	display_prograns[0].prg_status = FALSE;
//	display_prograns[0].prg_mode = online;
//	display_prograns[0].prg_repeat_cnt = 3;
//	display_prograns[0].prg_bright = 60;
//	display_prograns[0].prg_sec = 20;
//	display_prograns[0].dis_lang = en;
//	display_prograns[0].prg_font = EnglishBig;
//	//display_prograns[0].prg_font = EnglishSmall;
//	display_prograns[0].prg_exp_tm = 0;
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[0].prg_lines[i].type = simple;
////		display_prograns[0].prg_lines[i].graphics = i+1;
////		//char *temp = "Vaibhav ARde";
////		if(i==0)
////			sprintf(display_prograns[0].prg_lines[i].text, "PROGRAM ID IS 0 %d ", 0);
////		else if(i%2 ==0)
////		{
////			sprintf(display_prograns[0].prg_lines[i].text, "PIS DISPLAY PROJECT%d ", i);
////		}
////		else
////			sprintf(display_prograns[0].prg_lines[i].text, "TO CHECK ALL TYPES OF DATA%d ", i);
////		//strcpy(display_prograns[0].prg_lines[i].text, temp);
////	}
//
//	/*
//	 *  For program id 1
//	 */
//
//	display_prograns[1].prg_id = 1;
//	display_prograns[1].prg_status = FALSE;
//	display_prograns[1].prg_mode = online;
//	display_prograns[1].prg_repeat_cnt = 3;
//	display_prograns[1].prg_bright = 60;
//	display_prograns[1].prg_sec = 20;
//	display_prograns[1].dis_lang = en;
//	display_prograns[1].prg_font = EnglishBig;
//	display_prograns[1].prg_exp_tm = 900;
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[1].prg_lines[i].type = column;
////		//char *temp = "Vaibhav ARde";
////		if(i==0)
////					sprintf(display_prograns[1].prg_lines[i].text, "PROGRAM ID IS  %d ", 1);
////		else if(i%2 ==0)
////		{
////			if(i%4 ==0)
////				sprintf(display_prograns[1].prg_lines[i].text, "USED FOR PERCENTAGE 4 %d ", i);
////			else
////				sprintf(display_prograns[1].prg_lines[i].text, "EMBIN TECHNOLOGIES PVT LTD%d ", i);
////		}
////		else
////			sprintf(display_prograns[1].prg_lines[i].text, "NITESH shukla NSS %d ", i);
////		////strcpy(display_prograns[1].prg_lines[i].text, temp);
////		display_prograns[1].prg_lines[i].route_no = 12+i;
////		display_prograns[1].prg_lines[i].eta = 4+i;
////		display_prograns[1].prg_lines[i].graphics = i+1;
////	}
//	for(uint8_t i = 0; i< 7; i++)
//	{
//		display_prograns[1].prg_lines[i].type = simple;
//		display_prograns[1].prg_lines[i].graphics = 0;//i+1;
//		//char *temp = "Vaibhav ARde";
//		if(i==0)
//		{   //   they are running on road
//			display_prograns[1].prg_lines[i].text[0] = 0x01;
//			display_prograns[1].prg_lines[i].text[1] = 0x74;
//			display_prograns[1].prg_lines[i].text[2] = 0x01;
//			display_prograns[1].prg_lines[i].text[3] = 0x68;
//			display_prograns[1].prg_lines[i].text[4] = 0x01;
//			display_prograns[1].prg_lines[i].text[5] = 0x65;
//			display_prograns[1].prg_lines[i].text[6] = 0x01;
//			display_prograns[1].prg_lines[i].text[7] = 0x79;
//			display_prograns[1].prg_lines[i].text[8] = 0x01;
//			display_prograns[1].prg_lines[i].text[9] = 0x20;
//			display_prograns[1].prg_lines[i].text[10] = 0x01;
//			display_prograns[1].prg_lines[i].text[11] = 0x61;
//			display_prograns[1].prg_lines[i].text[12] = 0x01;
//			display_prograns[1].prg_lines[i].text[13] = 0x72;
//			display_prograns[1].prg_lines[i].text[14] = 0x01;
//			display_prograns[1].prg_lines[i].text[15] = 0x65;
//			display_prograns[1].prg_lines[i].text[16] = 0x01;
//			display_prograns[1].prg_lines[i].text[17] = 0x20;
//			display_prograns[1].prg_lines[i].text[18] = 0x01;
//			display_prograns[1].prg_lines[i].text[19] = 0x72;
//			display_prograns[1].prg_lines[i].text[20] = 0x01;
//			display_prograns[1].prg_lines[i].text[21] = 0x75;
//			display_prograns[1].prg_lines[i].text[22] = 0x01;
//			display_prograns[1].prg_lines[i].text[23] = 0x6e;
//			display_prograns[1].prg_lines[i].text[24] = 0x01;
//			display_prograns[1].prg_lines[i].text[25] = 0x6e;
//			display_prograns[1].prg_lines[i].text[26] = 0x01;
//			display_prograns[1].prg_lines[i].text[27] = 0x69;
//			display_prograns[1].prg_lines[i].text[28] = 0x01;
//			display_prograns[1].prg_lines[i].text[29] = 0x6e;
//			display_prograns[1].prg_lines[i].text[30] = 0x01;
//			display_prograns[1].prg_lines[i].text[31] = 0x67;
//			display_prograns[1].prg_lines[i].text[32] = 0x01;
//			display_prograns[1].prg_lines[i].text[33] = 0x20;
//			display_prograns[1].prg_lines[i].text[34] = 0x01;
//			display_prograns[1].prg_lines[i].text[35] = 0x6f;
//			display_prograns[1].prg_lines[i].text[36] = 0x01;
//			display_prograns[1].prg_lines[i].text[37] = 0x6e;
//			display_prograns[1].prg_lines[i].text[38] = 0x01;
//			display_prograns[1].prg_lines[i].text[39] = 0x20;
//			display_prograns[1].prg_lines[i].text[40] = 0x01;
//			display_prograns[1].prg_lines[i].text[41] = 0x72;
//			display_prograns[1].prg_lines[i].text[42] = 0x01;
//			display_prograns[1].prg_lines[i].text[43] = 0x6f;
//			display_prograns[1].prg_lines[i].text[44] = 0x01;
//			display_prograns[1].prg_lines[i].text[45] = 0x61;
//			display_prograns[1].prg_lines[i].text[46] = 0x01;
//			display_prograns[1].prg_lines[i].text[47] = 0x64;
//			display_prograns[1].prg_lines[i].text[48] = '\0';
//		}
////		else if(i%4 == 0)
////		{  // Embin
////			display_prograns[1].prg_lines[i].text[0] = 0x0d;
////			display_prograns[1].prg_lines[i].text[1] = 0x38;
////			display_prograns[1].prg_lines[i].text[2] = 0x0d;
////			display_prograns[1].prg_lines[i].text[3] = 0x4d;
////			display_prograns[1].prg_lines[i].text[4] = 0x0d;
////			display_prograns[1].prg_lines[i].text[5] = 0x31;
////			display_prograns[1].prg_lines[i].text[6] = 0x0d;
////			display_prograns[1].prg_lines[i].text[7] = 0x4d;
////			display_prograns[1].prg_lines[i].text[8] = 0x0d;
////			display_prograns[1].prg_lines[i].text[9] = 0x31;
////			display_prograns[1].prg_lines[i].text[10] = '\0';
////		}
//		else
//		{   // technology
//			display_prograns[1].prg_lines[i].text[0] = 0x01;
//			display_prograns[1].prg_lines[i].text[1] = 0x74;
//			display_prograns[1].prg_lines[i].text[2] = 0x01;
//			display_prograns[1].prg_lines[i].text[3] = 0x65;
//			display_prograns[1].prg_lines[i].text[4] = 0x01;
//			display_prograns[1].prg_lines[i].text[5] = 0x63;
//			display_prograns[1].prg_lines[i].text[6] = 0x01;
//			display_prograns[1].prg_lines[i].text[7] = 0x68;
//			display_prograns[1].prg_lines[i].text[8] = 0x01;
//			display_prograns[1].prg_lines[i].text[9] = 0x6e;
//			display_prograns[1].prg_lines[i].text[10] = 0x01;
//			display_prograns[1].prg_lines[i].text[11] = 0x6f;
//			display_prograns[1].prg_lines[i].text[12] = 0x01;
//			display_prograns[1].prg_lines[i].text[13] = 0x6c;
//			display_prograns[1].prg_lines[i].text[14] = 0x01;
//			display_prograns[1].prg_lines[i].text[15] = 0x6f;
//			display_prograns[1].prg_lines[i].text[16] = 0x01;
//			display_prograns[1].prg_lines[i].text[17] = 0x67;
//			display_prograns[1].prg_lines[i].text[18] = 0x01;
//			display_prograns[1].prg_lines[i].text[19] = 0x79;
//			display_prograns[1].prg_lines[i].text[20] = '\0';
//		}
//	}
//
//	/*
//	 *  For program id 2
//	 */
//
//	display_prograns[2].prg_id = 2;
//	display_prograns[2].prg_status = FALSE;
//	display_prograns[2].prg_mode = online;
//	display_prograns[2].prg_repeat_cnt = 3;
//	display_prograns[2].prg_bright = 60;
//	display_prograns[2].prg_sec = 50;
//	display_prograns[2].dis_lang = hi;
//	display_prograns[2].prg_font = EnglishBig;
//	//display_prograns[2].prg_font = EnglishSmall;
//	display_prograns[2].prg_exp_tm = 9000;
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[2].prg_lines[i].type = simple;
////		display_prograns[2].prg_lines[i].graphics = i+1;
////		//char *temp = "Vaibhav ARde";
////
////
////		if(i==0)
////				sprintf(display_prograns[2].prg_lines[i].text, "PROGRAM ID IS 0 %d ", 2);
////		else if(i%2 ==0)
////		{
////			if(i%4==0)
////				sprintf(display_prograns[2].prg_lines[i].text, "PROGRAM ID IS 2 ");
////			else
////				sprintf(display_prograns[2].prg_lines[i].text, "abcdefghijklm nopqrstuvwxyz");
////		}
////		else
////			sprintf(display_prograns[2].prg_lines[i].text, "123456789!@#$ ");
////		//strcpy(display_prograns[2].prg_lines[i].text, temp);
////	}
//	for(uint8_t i = 0; i< 7; i++)
//	{
//		display_prograns[2].prg_lines[i].type = column;
//		//char *temp = "Vaibhav ARde";
//		if(i==0)
//		{   //    वो व्यायाम कर रहा है
//			display_prograns[2].prg_lines[i].text[0] = 0x09;
//			display_prograns[2].prg_lines[i].text[1] = 0x35;
//			display_prograns[2].prg_lines[i].text[2] = 0x09;
//			display_prograns[2].prg_lines[i].text[3] = 0x3f;
//			display_prograns[2].prg_lines[i].text[4] = 0x01;
//			display_prograns[2].prg_lines[i].text[5] = 0x20;
//			display_prograns[2].prg_lines[i].text[6] = 0x09;
//			display_prograns[2].prg_lines[i].text[7] = 0x15;
//			display_prograns[2].prg_lines[i].text[8] = 0x09;
//			display_prograns[2].prg_lines[i].text[9] = 0x4d;
//			display_prograns[2].prg_lines[i].text[10] = 0x09;
//			display_prograns[2].prg_lines[i].text[11] = 0x37;
//			display_prograns[2].prg_lines[i].text[12] = 0x09;
//			display_prograns[2].prg_lines[i].text[13] = 0x3f;
//			display_prograns[2].prg_lines[i].text[14] = 0x09;
//			display_prograns[2].prg_lines[i].text[15] = 0x2f;
//			display_prograns[2].prg_lines[i].text[16] = 0x09;
//			display_prograns[2].prg_lines[i].text[17] = 0x3e;
//			display_prograns[2].prg_lines[i].text[18] = 0x09;
//			display_prograns[2].prg_lines[i].text[19] = 0x2e;
//			display_prograns[2].prg_lines[i].text[20] = 0x01;
//			display_prograns[2].prg_lines[i].text[21] = 0x20;
//			display_prograns[2].prg_lines[i].text[22] = 0x09;
//			display_prograns[2].prg_lines[i].text[23] = 0x15;
//			display_prograns[2].prg_lines[i].text[24] = 0x09;
//			display_prograns[2].prg_lines[i].text[25] = 0x30;
//			display_prograns[2].prg_lines[i].text[26] = 0x01;
//			display_prograns[2].prg_lines[i].text[27] = 0x20;
//			display_prograns[2].prg_lines[i].text[28] = 0x09;
//			display_prograns[2].prg_lines[i].text[29] = 0x30;
//			display_prograns[2].prg_lines[i].text[30] = 0x09;
//			display_prograns[2].prg_lines[i].text[31] = 0x39;
//			display_prograns[2].prg_lines[i].text[32] = 0x09;
//			display_prograns[2].prg_lines[i].text[33] = 0x3e;
//			display_prograns[2].prg_lines[i].text[34] = 0x01;
//			display_prograns[2].prg_lines[i].text[35] = 0x20;
//			display_prograns[2].prg_lines[i].text[36] = 0x09;
//			display_prograns[2].prg_lines[i].text[37] = 0x39;
//			display_prograns[2].prg_lines[i].text[38] = 0x09;
//			display_prograns[2].prg_lines[i].text[39] = 0x48;
//			display_prograns[2].prg_lines[i].text[40] = '\0';
//
//		}
//		else if(i%2 ==0)
//		{
//			     // priyakar
//			display_prograns[2].prg_lines[i].text[0] = 0x0d;
//			display_prograns[2].prg_lines[i].text[1] = 0x08;
//			display_prograns[2].prg_lines[i].text[2] = 0x0d;
//			display_prograns[2].prg_lines[i].text[3] = 0x38;
//			display_prograns[2].prg_lines[i].text[4] = 0x0d;
//			display_prograns[2].prg_lines[i].text[5] = 0x4d;
//			display_prograns[2].prg_lines[i].text[6] = 0x0d;
//			display_prograns[2].prg_lines[i].text[7] = 0x31;
//			display_prograns[2].prg_lines[i].text[8] = 0x0d;
//			display_prograns[2].prg_lines[i].text[9] = 0x4d;
//			display_prograns[2].prg_lines[i].text[10] = 0x0d;
//			display_prograns[2].prg_lines[i].text[11] = 0x31;
//			display_prograns[2].prg_lines[i].text[12] = 0x0d;
//			display_prograns[2].prg_lines[i].text[13] = 0x4d;
//			display_prograns[2].prg_lines[i].text[14] = 0x01;
//			display_prograns[2].prg_lines[i].text[15] = 0x20;
//			display_prograns[2].prg_lines[i].text[16] = 0x0d;
//			display_prograns[2].prg_lines[i].text[17] = 0x2b;
//			display_prograns[2].prg_lines[i].text[18] = 0x0d;
//			display_prograns[2].prg_lines[i].text[19] = 0x4b;
//			display_prograns[2].prg_lines[i].text[20] = 0x0d;
//			display_prograns[2].prg_lines[i].text[21] = 0x7c;
//			display_prograns[2].prg_lines[i].text[22] = 0x0d;
//			display_prograns[2].prg_lines[i].text[23] = 0x1f;
//			display_prograns[2].prg_lines[i].text[24] = 0x0d;
//			display_prograns[2].prg_lines[i].text[25] = 0x4d;
//			display_prograns[2].prg_lines[i].text[26] = 0x0d;
//			display_prograns[2].prg_lines[i].text[27] = 0x1f;
//			display_prograns[2].prg_lines[i].text[28] = 0x0d;
//			display_prograns[2].prg_lines[i].text[29] = 0x46;
//			display_prograns[2].prg_lines[i].text[30] = 0x0d;
//			display_prograns[2].prg_lines[i].text[31] = 0x4d;
//			display_prograns[2].prg_lines[i].text[32] = '\0';
//		}
//		else
//		{     //    varg
//			display_prograns[2].prg_lines[i].text[0] = 0x09;
//			display_prograns[2].prg_lines[i].text[1] = 0x32;
//			display_prograns[2].prg_lines[i].text[2] = 0x09;
//			display_prograns[2].prg_lines[i].text[3] = 0x4c;
//			display_prograns[2].prg_lines[i].text[4] = 0x09;
//			display_prograns[2].prg_lines[i].text[5] = 0x15;
//			display_prograns[2].prg_lines[i].text[6] = 0x0d;
//			display_prograns[2].prg_lines[i].text[7] = 0x3f;
//			display_prograns[2].prg_lines[i].text[8] = 0x09;
//			display_prograns[2].prg_lines[i].text[9] = 0x15;
//			display_prograns[2].prg_lines[i].text[10] = '\0';
//		}
//
//		display_prograns[2].prg_lines[i].route_no = 12+i;
//		display_prograns[2].prg_lines[i].eta = 4+i;
//		display_prograns[2].prg_lines[i].graphics = i+1;
//	}
//
//
//	/*
//	 *  For program id 3
//	 */
//
//	//load Dummy Data
//		display_prograns[3].prg_id = 3;
//		display_prograns[3].prg_status = FALSE;
//		display_prograns[3].prg_mode = online;
//		display_prograns[3].prg_repeat_cnt = 1;
//		display_prograns[3].prg_bright = 80;
//		display_prograns[3].prg_sec = 200;
//		display_prograns[3].dis_lang = en;
//		display_prograns[3].prg_font = EnglishBig;
//		//display_prograns[3].prg_font = EnglishSmall;
//		display_prograns[3].prg_exp_tm = 999999;//500;
//
//
//		display_prograns[3].prg_lines[0].route_no = 40;
//		display_prograns[3].prg_lines[0].eta = 3;
//
//		display_prograns[3].prg_lines[1].route_no = 201;
//		display_prograns[3].prg_lines[1].eta = 7;
//
//		display_prograns[3].prg_lines[2].route_no = 440;
//		display_prograns[3].prg_lines[2].eta = 12;
//
//		display_prograns[3].prg_lines[3].route_no = 462;
//		display_prograns[3].prg_lines[3].eta = 9;
//
//		for(uint8_t i = 0; i< 7; i++)
//		{
//			display_prograns[3].prg_lines[i].type = column;
//			display_prograns[3].prg_lines[i].graphics = 0;//i+1;
//
//			display_prograns[3].prg_lines[i].text[0]=0x01;
//			display_prograns[3].prg_lines[i].text[1]=0x45;
//			display_prograns[3].prg_lines[i].text[2]=0x01;
//			display_prograns[3].prg_lines[i].text[3]=0x61;
//			display_prograns[3].prg_lines[i].text[4]=0x01;
//			display_prograns[3].prg_lines[i].text[5]=0x73;
//			display_prograns[3].prg_lines[i].text[6]=0x01;
//			display_prograns[3].prg_lines[i].text[7]=0x74;
//			display_prograns[3].prg_lines[i].text[8]=0x01;
//			display_prograns[3].prg_lines[i].text[9]=0x20;
//			display_prograns[3].prg_lines[i].text[10]=0x01;
//			display_prograns[3].prg_lines[i].text[11]=0x66;
//			display_prograns[3].prg_lines[i].text[12]=0x01;
//			display_prograns[3].prg_lines[i].text[13]=0x6f;
//			display_prograns[3].prg_lines[i].text[14]=0x01;
//			display_prograns[3].prg_lines[i].text[15]=0x72;
//			display_prograns[3].prg_lines[i].text[16]=0x01;
//			display_prograns[3].prg_lines[i].text[17]=0x74;
//			display_prograns[3].prg_lines[i].text[18]='\0';
//		}
//
//
////			if(i==0)
////				sprintf(display_prograns[3].prg_lines[i].text, "PROGRAM ID IS 0 %d ", 3);
////			else if(i%2 ==0)
////			{
////				if(i%4==0)
////					sprintf(display_prograns[3].prg_lines[i].text, "program id is 3 %d ", i);
////				else
////					sprintf(display_prograns[3].prg_lines[i].text, "CHURCHGATE CHURCHGATE CHURCHGATE CHURCHGATEi%d ", i);
////			}
////			else
////				sprintf(display_prograns[3].prg_lines[i].text, "MARIN LINES MARIN LINES%d ", i);
//			//strcpy(display_prograns[3].prg_lines[i].text, temp);
//
//	/*
//	 *  For program id 4
//	 */
//
////load Dummy Data
//	display_prograns[4].prg_id = 4;
//	display_prograns[4].prg_status = FALSE;
//	display_prograns[4].prg_mode = online;
//	display_prograns[4].prg_repeat_cnt = 5;
//	display_prograns[4].prg_bright = 45;
//	display_prograns[4].prg_sec = 10;
//	display_prograns[4].dis_lang = hi;
//	display_prograns[4].prg_font = EnglishBig;
//	//display_prograns[4].prg_font = EnglishSmall;
//	display_prograns[4].prg_exp_tm = 999999;//160;
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[4].prg_lines[i].type = simple;
//////		display_prograns[4].prg_lines[i].graphics = i+1;
////		//char *temp = "Vaibhav ARde";
////		if(i==0)
////			sprintf(display_prograns[4].prg_lines[i].text, "PROGRAM ID IS 0 %d ", 4);
////		else if(i%2 ==0)
////		{
////			if(i%4==0)
////				sprintf(display_prograns[4].prg_lines[i].text, "PROGRAM ID IS 4 %d ", i);
////			else
////				sprintf(display_prograns[4].prg_lines[i].text, "Charni road Charni road ******%d ", i);
////		}
////		else
////			sprintf(display_prograns[4].prg_lines[i].text, "Grant road grant road grant road %d ", i);
////		//strcpy(display_prograns[4].prg_lines[i].text, temp);
////	}
//
//	display_prograns[4].prg_lines[0].route_no = 40;
//	display_prograns[4].prg_lines[0].eta = 3;
//
//	display_prograns[4].prg_lines[1].route_no = 201;
//	display_prograns[4].prg_lines[1].eta = 7;
//
//	display_prograns[4].prg_lines[2].route_no = 440;
//	display_prograns[4].prg_lines[2].eta = 12;
//
//	display_prograns[4].prg_lines[3].route_no = 462;
//	display_prograns[4].prg_lines[3].eta = 9;
//
//	for(uint8_t i = 0; i< 7; i++)
//	{
//		display_prograns[4].prg_lines[i].type = column;
//		display_prograns[4].prg_lines[i].graphics = 0;//i+1;
//
//		display_prograns[4].prg_lines[i].text[0]=0x09;
//		display_prograns[4].prg_lines[i].text[1]=0x08;
//		display_prograns[4].prg_lines[i].text[2]=0x09;
//		display_prograns[4].prg_lines[i].text[3]=0x38;
//		display_prograns[4].prg_lines[i].text[4]=0x09;
//		display_prograns[4].prg_lines[i].text[5]=0x4d;
//		display_prograns[4].prg_lines[i].text[6]=0x09;
//		display_prograns[4].prg_lines[i].text[7]=0x1f;
//		display_prograns[4].prg_lines[i].text[8]=0x01;
//		display_prograns[4].prg_lines[i].text[9]=0x20;
//		display_prograns[4].prg_lines[i].text[10]=0x09;
//		display_prograns[4].prg_lines[i].text[11]=0x2b;
//		display_prograns[4].prg_lines[i].text[12]=0x09;
//		display_prograns[4].prg_lines[i].text[13]=0x4a;
//		display_prograns[4].prg_lines[i].text[14]=0x09;
//		display_prograns[4].prg_lines[i].text[15]=0x30;
//		display_prograns[4].prg_lines[i].text[16]=0x09;
//		display_prograns[4].prg_lines[i].text[17]=0x4d;
//		display_prograns[4].prg_lines[i].text[18]=0x09;
//		display_prograns[4].prg_lines[i].text[19]=0x1f;
//		display_prograns[4].prg_lines[i].text[20]='\0';
//	}
//
//
//	/*
//	 *  For program id 5
//	 */
//
//	//load Dummy Data
//	display_prograns[5].prg_id = 5;
//	display_prograns[5].prg_status = FALSE;
//	display_prograns[5].prg_mode = online;
//	display_prograns[5].prg_repeat_cnt = 4;
//	display_prograns[5].prg_bright = 50;
//	display_prograns[5].prg_sec = 15;
//	display_prograns[5].dis_lang = ml;
//	display_prograns[5].prg_font = EnglishBig;
//	//display_prograns[5].prg_font = EnglishSmall;
//	display_prograns[5].prg_exp_tm = 999999;//60;
//
//	display_prograns[5].prg_lines[0].route_no = 40;
//	display_prograns[5].prg_lines[0].eta = 3;
//
//	display_prograns[5].prg_lines[1].route_no = 201;
//	display_prograns[5].prg_lines[1].eta = 7;
//
//	display_prograns[5].prg_lines[2].route_no = 440;
//	display_prograns[5].prg_lines[2].eta = 12;
//
//	display_prograns[5].prg_lines[3].route_no = 462;
//	display_prograns[5].prg_lines[3].eta = 9;
//
//	for(uint8_t i = 0; i< 7; i++)
//	{
//		display_prograns[5].prg_lines[i].type = column;
//		display_prograns[5].prg_lines[i].graphics = 0;//i+1;
//
//		display_prograns[5].prg_lines[i].text[0] = 0x0d;
//		display_prograns[5].prg_lines[i].text[1] = 0x08;
//		display_prograns[5].prg_lines[i].text[2] = 0x0d;
//		display_prograns[5].prg_lines[i].text[3] = 0x38;
//		display_prograns[5].prg_lines[i].text[4] = 0x0d;
//		display_prograns[5].prg_lines[i].text[5] = 0x4d;
//		display_prograns[5].prg_lines[i].text[6] = 0x0d;
//		display_prograns[5].prg_lines[i].text[7] = 0x31;
//		display_prograns[5].prg_lines[i].text[8] = 0x0d;
//		display_prograns[5].prg_lines[i].text[9] = 0x4d;
//		display_prograns[5].prg_lines[i].text[10] = 0x0d;
//		display_prograns[5].prg_lines[i].text[11] = 0x31;
//		display_prograns[5].prg_lines[i].text[12] = 0x0d;
//		display_prograns[5].prg_lines[i].text[13] = 0x4d;
//		display_prograns[5].prg_lines[i].text[14] = 0x01;
//		display_prograns[5].prg_lines[i].text[15] = 0x20;
//		display_prograns[5].prg_lines[i].text[16] = 0x0d;
//		display_prograns[5].prg_lines[i].text[17] = 0x2b;
//		display_prograns[5].prg_lines[i].text[18] = 0x0d;
//		display_prograns[5].prg_lines[i].text[19] = 0x4b;
//		display_prograns[5].prg_lines[i].text[20] = 0x0d;
//		display_prograns[5].prg_lines[i].text[21] = 0x7c;
//		display_prograns[5].prg_lines[i].text[22] = 0x0d;
//		display_prograns[5].prg_lines[i].text[23] = 0x1f;
//		display_prograns[5].prg_lines[i].text[24] = 0x0d;
//		display_prograns[5].prg_lines[i].text[25] = 0x4d;
//		display_prograns[5].prg_lines[i].text[26] = 0x0d;
//		display_prograns[5].prg_lines[i].text[27] = 0x1f;
//		display_prograns[5].prg_lines[i].text[28] = 0x0d;
//		display_prograns[5].prg_lines[i].text[29] = 0x4d;
//		display_prograns[5].prg_lines[i].text[30] = '\0';
//
//	}
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[5].prg_lines[i].type = simple;
////		display_prograns[5].prg_lines[i].graphics = i+1;
////		//char *temp = "Vaibhav ARde";
////
////		if(i==0)
////			sprintf(display_prograns[5].prg_lines[i].text, "PROGRAM ID IS 0 %d ", 5);
////		else if(i%2 ==0)
////		{
////			if(i%4==0)
////				sprintf(display_prograns[5].prg_lines[i].text, "PROGRAM ID IS 5 PROGRAM ID IS 5 PROGRAM ID IS 5%d ", i);
////			else
////				sprintf(display_prograns[5].prg_lines[i].text, "MAHALAXMI %d ", i);
////		}
////		else
////			sprintf(display_prograns[5].prg_lines[i].text, "LOWER PAREL%d ", i);
////		//strcpy(display_prograns[5].prg_lines[i].text, temp);
////	}
//
//
//
//	/*
//	 *  For program id 6
//	 */
//
//
////load Dummy Data
//	display_prograns[6].prg_id = 6;
//	display_prograns[6].prg_status = FALSE;
//	display_prograns[6].prg_mode = online;
//	display_prograns[6].prg_repeat_cnt = 6;
//	display_prograns[6].prg_bright = 65;
//	display_prograns[6].prg_sec = 8;
//	display_prograns[6].dis_lang = en;
//	display_prograns[6].prg_font = EnglishBig;
//	//display_prograns[6].prg_font = EnglishSmall;
//	display_prograns[6].prg_exp_tm = 0;//200;
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[6].prg_lines[i].type = simple;
////		display_prograns[6].prg_lines[i].graphics = i+1;
////		//char *temp = "Vaibhav ARde";
////		if(i==0)
////			sprintf(display_prograns[6].prg_lines[i].text, "PROGRAM ID IS 0 %d ", 6);
////		else if(i%2 ==0)
////		{
////			if(i%4==0)
////				sprintf(display_prograns[6].prg_lines[i].text, "PROGRAM ID IS 6%d ", i);
////			else
////				sprintf(display_prograns[6].prg_lines[i].text, "prabhadevi %d ", i);
////		}
////		else
////			sprintf(display_prograns[6].prg_lines[i].text, "DADAR  DADAR DADAR DADAR %d ", i);
////		//strcpy(display_prograns[6].prg_lines[i].text, temp);
////	}
//
//	/*
//	 *  For program id 7
//	 */
//
//
////load Dummy Data
//	display_prograns[7].prg_id = 7;
//	display_prograns[7].prg_status = FALSE;
//	display_prograns[7].prg_mode = online;
//	display_prograns[7].prg_repeat_cnt = 3;
//	display_prograns[7].prg_bright = 70;
//	display_prograns[7].prg_sec = 5;
//	display_prograns[7].dis_lang = en;
//	display_prograns[7].prg_font = EnglishBig;
//	//display_prograns[7].prg_font = EnglishSmall;
//	display_prograns[7].prg_exp_tm = 0;//40;
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[7].prg_lines[i].type = simple;
////		display_prograns[7].prg_lines[i].graphics = i+1;
////		//char *temp = "Vaibhav ARde";
////		if(i==0)
////			sprintf(display_prograns[7].prg_lines[i].text, "PROGRAM ID IS 0 %d ", 7);
////		else if(i%2 ==0)
////		{
////			if(i%4==0)
////				sprintf(display_prograns[7].prg_lines[i].text, "PROGRAM ID IS 7%d ", i);
////			else
////				sprintf(display_prograns[7].prg_lines[i].text, "MATUNGA ROAD %d ", i);
////		}
////		else
////			sprintf(display_prograns[7].prg_lines[i].text, "MAHIM%d ", i);
////		//strcpy(display_prograns[7].prg_lines[i].text, temp);
////	}
//	/*
//	 *  For program id 8
//	 */
//
////load Dummy Data
//	display_prograns[8].prg_id = 8;
//	display_prograns[8].prg_status = FALSE;
//	display_prograns[8].prg_mode = online;
//	display_prograns[8].prg_repeat_cnt = 1;
//	display_prograns[8].prg_bright = 85;
//	display_prograns[8].prg_sec = 20;
//	display_prograns[8].dis_lang = en;
//	display_prograns[8].prg_font = EnglishBig;
//	//display_prograns[8].prg_font = EnglishSmall;
//	display_prograns[8].prg_exp_tm = 0;//300;
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[8].prg_lines[i].type = simple;
////		display_prograns[8].prg_lines[i].graphics = i+1;
////		//char *temp = "Vaibhav ARde";
////
////		if(i==0)
////			sprintf(display_prograns[8].prg_lines[i].text, "PROGRAM ID IS 0 %d ", 8);
////		else if(i%2 ==0)
////		{
////			if(i%4==0)
////				sprintf(display_prograns[8].prg_lines[i].text, "PROGRAM ID IS 8%d ", i);
////			else
////				sprintf(display_prograns[8].prg_lines[i].text, "BANDRA BANDRA BANDRA BANDRA%d ", i);
////		}
////		else
////			sprintf(display_prograns[8].prg_lines[i].text, "KHAR ROAD %d ", i);
////		//strcpy(display_prograns[8].prg_lines[i].text, temp);
////	}
//
//	/*
//	 *  For program id 9
//	 *
//	 */
//
//
////load Dummy Data
//	display_prograns[9].prg_id = 9;
//	display_prograns[9].prg_status = FALSE;
//	display_prograns[9].prg_mode = offline;
//	display_prograns[9].prg_repeat_cnt = 3;
//	display_prograns[9].prg_bright = 60;
//	display_prograns[9].prg_sec = 20;
//	display_prograns[9].dis_lang = en;
//	display_prograns[9].prg_font = EnglishBig;
//	//display_prograns[9].prg_font = EnglishSmall;
//	display_prograns[9].prg_exp_tm = 0;//123;
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[9].prg_lines[i].type = simple;
////		display_prograns[9].prg_lines[i].graphics = i+1;
////		//char *temp = "Vaibhav ARde";
////		if(i==0)
////			sprintf(display_prograns[9].prg_lines[i].text, "PROGRAM ID IS 0 %d ", 9);
////		else if(i%2 ==0)
////		{
////			if(i%4==0)
////				sprintf(display_prograns[9].prg_lines[i].text, "PROGRAM ID IS 9%d ", i);
////			else
////				sprintf(display_prograns[9].prg_lines[i].text, "Vile parle Vile parle Vile parle%d ", i);
////		}
////		else
////			sprintf(display_prograns[9].prg_lines[i].text, "Andheri %d ", i);
////		//strcpy(display_prograns[9].prg_lines[i].text, temp);
////	}
//
//	/*
//	 *  For program id 10
//	 */
//
//
////load Dummy Data
//	display_prograns[10].prg_id = 10;
//	display_prograns[10].prg_status = FALSE;
//	display_prograns[10].prg_mode = online;
//	display_prograns[10].prg_repeat_cnt = 13;
//	display_prograns[10].prg_bright = 60;
//	display_prograns[10].prg_sec = 10;
//	display_prograns[10].dis_lang = en;
//	//display_prograns[10].prg_font = EnglishBig;
//	display_prograns[10].prg_font = EnglishSmall;
//	display_prograns[10].prg_exp_tm = 0;//80;
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[10].prg_lines[i].type = simple;
////		display_prograns[10].prg_lines[i].graphics = i+1;
////		//char *temp = "Vaibhav ARde";
////		if(i==0)
////			sprintf(display_prograns[10].prg_lines[i].text, "PROGRAM ID IS 0 %d ", 10);
////		else if(i%2 ==0)
////		{
////			if(i%4==0)
////				sprintf(display_prograns[10].prg_lines[i].text, "PROGRAM ID IS 10 %d ", i);
////			else
////				sprintf(display_prograns[10].prg_lines[i].text, " Jogeshwari %d ", i);
////		}
////		else
////			sprintf(display_prograns[10].prg_lines[i].text, "RAM MANDIR RAM MANDIR RAM MANDIR RAM MANDIR%d", i);
////		//strcpy(display_prograns[10].prg_lines[i].text, temp);
////	}
//
//
//	/*
//	 *  For program id 11
//	 */
//
//
////load Dummy Data
//	display_prograns[11].prg_id = 11;
//	display_prograns[11].prg_status = FALSE;
//	display_prograns[11].prg_mode = online;
//	display_prograns[11].prg_repeat_cnt = 9;
//	display_prograns[11].prg_bright = 90;
//	display_prograns[11].prg_sec = 20;
//	display_prograns[11].dis_lang = en;
//	display_prograns[11].prg_font = EnglishBig;
//	//display_prograns[11].prg_font = EnglishSmall;
//	display_prograns[11].prg_exp_tm = 0;//123;
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[11].prg_lines[i].type = column;
////		display_prograns[11].prg_lines[i].graphics = i+1;
////		//char *temp = "Vaibhav ARde";
////		if(i==0)
////			sprintf(display_prograns[11].prg_lines[i].text, "PROGRAM ID IS 0 %d ", 11);
////		else if(i%2 ==0)
////		{
////			if(i%4==0)
////				sprintf(display_prograns[11].prg_lines[i].text, "PROGRAM ID IS 11 %d ", i);
////			else
////				sprintf(display_prograns[11].prg_lines[i].text, "GOREGAON %d ", i);
////		}
////		else
////			sprintf(display_prograns[11].prg_lines[i].text, "MALAD %d", i);
////		//strcpy(display_prograns[11].prg_lines[i].text, temp);
////	 }
//
//	/*
//	 *  For program id 12
//	 */
//
////load Dummy Data
//	display_prograns[12].prg_id = 12;
//	display_prograns[12].prg_status = FALSE;
//	display_prograns[12].prg_mode = online;
//	display_prograns[12].prg_repeat_cnt = 7;
//	display_prograns[12].prg_bright = 80;
//	display_prograns[12].prg_sec = 10;
//	display_prograns[12].dis_lang = en;
//	display_prograns[12].prg_font = EnglishBig;
//	//display_prograns[12].prg_font = EnglishSmall;
//	display_prograns[12].prg_exp_tm = 0;//450;
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[12].prg_lines[i].type = simple;
////		display_prograns[12].prg_lines[i].graphics = i+1;
////		//char *temp = "Vaibhav ARde";
////		if(i==0)
////			sprintf(display_prograns[12].prg_lines[i].text, "PROGRAM ID IS 0 %d ", 12);
////		else if(i%2 ==0)
////		{
////			if(i%4==0)
////				sprintf(display_prograns[12].prg_lines[i].text, "PROGRAM ID IS 12%d ", i);
////			else
////				sprintf(display_prograns[12].prg_lines[i].text, "kandivali%d ", i);
////		}
////		else
////			sprintf(display_prograns[12].prg_lines[i].text, "borivali borivali borivali %d ", i);
////		//strcpy(display_prograns[12].prg_lines[i].text, temp);
////	}
//
//	/*
//	 *  For program id 13
//	 */
//
//
////load Dummy Data
//	display_prograns[13].prg_id = 13;
//	display_prograns[13].prg_status = FALSE;
//	display_prograns[13].prg_mode = online;
//	display_prograns[13].prg_repeat_cnt = 15;
//	display_prograns[13].prg_bright = 60;
//	display_prograns[13].prg_sec = 13;
//	display_prograns[13].dis_lang = en;
//	//display_prograns[13].prg_font = EnglishBig;
//	display_prograns[13].prg_font = EnglishSmall;
//	display_prograns[13].prg_exp_tm = 0;//350;
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[13].prg_lines[i].type = simple;
////		display_prograns[13].prg_lines[i].graphics = i+1;
////		//char *temp = "Vaibhav ARde";
////		if(i==0)
////			sprintf(display_prograns[13].prg_lines[i].text, "PROGRAM ID IS  %d ", 13);
////		else if(i%2 ==0)
////		{
////			if(i%4==0)
////				sprintf(display_prograns[13].prg_lines[i].text, "PROGRAM ID IS 13%d ", i);
////			else
////				sprintf(display_prograns[13].prg_lines[i].text, "DAHISAR%d ", i);
////		}
////		else
////			sprintf(display_prograns[13].prg_lines[i].text, "MIRA ROAD MIRA ROAD MIRA ROAD%d", i);
////		//strcpy(display_prograns[0].prg_lines[i].text, temp);
////	}
//
//	/*
//	 *  For program id 14
//	 */
//
////load Dummy Data
//	display_prograns[14].prg_id = 14;
//	display_prograns[14].prg_status = FALSE;
//	display_prograns[14].prg_mode = online;
//	display_prograns[14].prg_repeat_cnt = 3;
//	display_prograns[14].prg_bright = 60;
//	display_prograns[14].prg_sec = 20;
//	display_prograns[14].dis_lang = en;
//	//display_prograns[14].prg_font = EnglishBig;
//	display_prograns[14].prg_font = EnglishSmall;
//	display_prograns[14].prg_exp_tm = 0;//123654;
////	for(uint8_t i = 0; i< 7; i++)
////	{
////		display_prograns[14].prg_lines[i].type = simple;
////		display_prograns[14].prg_lines[i].graphics = i+1;
////		//char *temp = "Vaibhav ARde";
////		if(i==0)
////			sprintf(display_prograns[14].prg_lines[i].text, "PROGRAM ID IS %d ", 14);
////		else if(i%2 ==0)
////		{
////			if(i%4==0)
////				sprintf(display_prograns[14].prg_lines[i].text, "PROGRAM ID IS %d ", i);
////			else
////				sprintf(display_prograns[14].prg_lines[i].text, "BHAYANDAR%d ", i);
////		}
////		else
////			sprintf(display_prograns[14].prg_lines[i].text, "NAIGAON%d ", i);
////		//strcpy(display_prograns[0].prg_lines[i].text, temp);
////	}

}


esp_err_t load_initial_program(void)
{
	time_t current_epoch_time;
	for(uint8_t i =0; i< MAX_DISPLAY_PROGRAMS; i++)
	{
		if(display_prograns[i].prg_status == TRUE)
		{
			time(&current_epoch_time);
			ESP_LOGI(TAG,"currebt Epoch Time = %ld", current_epoch_time);
			if(display_prograns[i].prg_exp_tm > current_epoch_time)
			{
				selected_program = i;
				printf("Initail program id = %d\n", selected_program);
				return ESP_OK;
			}
			else
			{
				display_prograns[i].prg_status = FALSE;
			}

		}
	}
	selected_program = 14;
	return ESP_FAIL;
}


esp_err_t load_next_program(void)
{
	bool got_next_prg = FALSE;
	time_t current_epoch_time;
	uint8_t cnt = 0;
	//ESP_LOGI(TAG,"Loading Next Program");
	while(!got_next_prg)
	{
		selected_program++;
		//ets_printf("Select prg = %d",selected_program);
		if(selected_program >= 14)
			selected_program = 0;
		cnt++;
		if(cnt == 15)
		{
			return ESP_FAIL;

		}

		if(display_prograns[selected_program].prg_status == TRUE)
		{
			time(&current_epoch_time);
			//ESP_LOGI(TAG,"current Epoch Time = %ld", current_epoch_time);

			if(display_prograns[selected_program].prg_exp_tm > current_epoch_time)
			{
				//ESP_LOGI(TAG,"Selected Program = %d", selected_program);
				got_next_prg = FALSE;
				return ESP_OK;
			}
			else
			{
				display_prograns[selected_program].prg_status = FALSE;
			}
		}
	}
	return ESP_FAIL;
}


void Sec_Routine_1Sec(TimerHandle_t xTimer)
{
	/* For 1st program */
	if((current_program_cnt))
	{
		//ESP_LOGI(TAG,"Inside 1Sec Timer");
		current_program_cnt--;
		if(current_program_cnt==0)
		{
			if(load_next_program() == ESP_OK)
			{
//				if(current_program  != selected_program)
//					clear_p6_display_data();


				current_program  = selected_program;
				nothing_to_disp = FALSE;
				timer_pause(DIS_TIMER_GRP, DIS_TIMER_ID);
				clear_p6_display_data();
				Cnstdata_writeProgramIntoDispMem(&display_prograns[current_program]);
				timer_start(DIS_TIMER_GRP, DIS_TIMER_ID);
				current_program_cnt = display_prograns[current_program].prg_sec;
				//ESP_LOGI(TAG, "Successfully Loaded Next program");
			}
			else if(selected_program != 14)
			{
				nothing_to_disp = TRUE;
				selected_program = 14;
				current_program  = selected_program;
				timer_pause(DIS_TIMER_GRP, DIS_TIMER_ID);
				Cnstdata_writeProgramIntoDispMem(&display_prograns[selected_program]);
				timer_start(DIS_TIMER_GRP, DIS_TIMER_ID);
				//timer_pause(SCR_TIMER_GRP, SCR_TIMER_ID);
				//clear_p6_display_data();
			}
		}
	}
	else
	{
		//ESP_LOGI(TAG,"Inside 12Sec Timer");
		if(nothing_to_disp == FALSE)
		{
			if(load_next_program()==ESP_OK)
			{
				//clear_p6_display_data();
				current_program  = selected_program;
				nothing_to_disp = FALSE;
				timer_pause(DIS_TIMER_GRP, DIS_TIMER_ID);
				clear_p6_display_data();
				Cnstdata_writeProgramIntoDispMem(&display_prograns[current_program]);
				timer_start(DIS_TIMER_GRP, DIS_TIMER_ID);
				current_program_cnt = display_prograns[current_program].prg_sec;
				//ESP_LOGI(TAG, "Successfully Loaded Next program");
			}
		}
	}

//	ESP_LOGI(TAG,"Inside 1Sec Timer");
}



TimerHandle_t Sec_timer_1Sec = NULL;

void initialize_program_refresh_routine(void)
{
	Sec_timer_1Sec = xTimerCreate("1_sec_timer", pdMS_TO_TICKS(1000), pdTRUE, (void *)1, Sec_Routine_1Sec);
	xTimerStart(Sec_timer_1Sec , 0);
}

void terminate_program_refresh_routine(void)
{
	xTimerStop(Sec_timer_1Sec , 0);
}

void restart_program_refresh_routine(void)
{
	xTimerStart(Sec_timer_1Sec , 0);
}


/*
 * Function to Display Error Message
 */
void display_Message(uint8_t error_no)
{
//	ESP_LOGE(TAG,"Displaying Error message");
	timer_pause(DIS_TIMER_GRP, DIS_TIMER_ID);
	clear_p6_display_data();
	for(uint8_t j=0; j<15; j++)
	{
		for(uint8_t i=0; i<7; i++)
		{
			memset(display_prograns[j].prg_lines[i].text, 0, 256);
		}
	}
	load_dummy_data();
	switch(error_no)
	{
	case ETHERNET_MODULE_ERROR:
		Cnstdata_writeLinetiDispMem(" Error", 0, englishbig);
		Cnstdata_writeLinetiDispMem("Ethernet Error", 1, englishbig);
		break;
	case DID_NOT_GET_IP_ADDRESS:
		Cnstdata_writeLinetiDispMem(" Error", 0, englishbig);
		Cnstdata_writeLinetiDispMem("No Internet", 1, englishbig);
		break;
	case GSM_MODULE_ERROR:
		Cnstdata_writeLinetiDispMem(" Error", 0, englishbig);
		Cnstdata_writeLinetiDispMem("No Connection", 1, englishbig);
		break;
	case SERVER_ERROR:
		ESP_LOGE(TAG,"Displaying Error message");
		Cnstdata_writeLinetiDispMem(" Error", 0, englishbig);
		Cnstdata_writeLinetiDispMem("Server Error", 1, englishbig);
		break;
	case FOTA_SUCESS:
		ESP_LOGE(TAG,"Displaying Error message");
		Cnstdata_writeLinetiDispMem(" FOTA Update", 0, englishbig);
		Cnstdata_writeLinetiDispMem("Successful", 1, englishbig);
		break;
	default:
		break;
	}
	for(uint8_t h=0;h<no_row;h++)
		duplicate_the_disp_array(h);
	timer_start(DIS_TIMER_GRP, DIS_TIMER_ID);
	//timer_pause(SCR_TIMER_GRP, SCR_TIMER_ID);
}
