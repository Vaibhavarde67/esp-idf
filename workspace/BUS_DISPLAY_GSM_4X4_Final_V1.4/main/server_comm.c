/*
 * server_comm.c
 *
 *  Created on: Feb 10, 2023
 *      Author: Embin 7
 */

#include "server_comm.h"

static const char *TAG = "Server Communication";

int rec_prg_id;
//char *output_buffer;  // Buffer to store response of http request from event handler
//int output_len;


/*
 * Function to process the Receivd Responce using JSON library
 */



/*
 * Function to detect Occurance of a substring in string
 */
bool check_for_presence_string(char *str1, char *str2)
{
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	for (int i = 0; i <= len1 - len2; i++)
	{
		int j;
		for (j = 0; j < len2; j++)
		{
			if (str1[i + j] != str2[j])
				break;
		}
		if (j == len2)
			return TRUE;
	}
	return FALSE;
}



/*
 * Function to detect Occurance of a substring and return the start position
 */
int find_pos_for_substring(char *str1, char *str2)
{
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	for (int i = 0; i <= len1 - len2; i++)
	{
		int j;
		for (j = 0; j < len2; j++)
		{
			if (str1[i + j] != str2[j])
				break;
		}
		if (j == len2)
			return i;
	}
	return -1;
}


/*
 * Function to detect Occurance of a substring and return the end position
 */

int find_pos_for_substring_endref(char *str1, char *str2)
{
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	for (int i = 0; i <= len1 - len2; i++)
	{
		int j;
		for (j = 0; j < len2; j++)
		{
			if (str1[i + j] != str2[j])
				break;
		}
		if (j == len2)
			return (i+len2);
	}
	return -1;
}

/*
 * Function to detect Occurance of a substring and return the start position
 * Only catch is that it start checking from start position provided while calling
 */

int find_pos_for_substring_from_pos(int start_pos, char *str1, char *str2)
{
	int len1 = strlen(str1);
	int len2 = strlen(str2);
	for (int i = start_pos; i <= len1 - len2; i++)
	{
		int j;
		for (j = 0; j < len2; j++)
		{
			if (str1[i + j] != str2[j])
				break;
		}
		if (j == len2)
			return i;
	}
	return -1;
}

/*
 * Function to create substring using start pos and end pos
 */

void create_substring_from_pos(char * dest, char *src, int start, int end)
{
	for(int i = start; i<end; i++)
	{
		dest[i-start]=src[i];
	}
}



void process_received_prg(char *data)
{
	int og_size = strlen(data);
	int pos_st = 0, pos_end = 0;

	//ESP_LOGE(TAG,"Original Size = %d",og_size);
	//char *temp_data = heap_caps_malloc(og_size,NULL);
	//char temp_data[og_size];
	//ESP_LOGI(TAG,"Available free Size21 = %d",esp_get_free_internal_heap_size());

	char *temp_data = (char*)malloc(og_size);
	//ESP_LOGI(TAG,"Available free Size22 = %d",esp_get_free_internal_heap_size());
	uint8_t rec_prg_id =0;

	//ESP_LOGE(TAG,"Copied String = %s",temp);

	//sprintf(temp_data,"%s", data);	This is the reason for that memory leak

	if(check_for_presence_string(data,"prg_id"))
	{
		pos_st = find_pos_for_substring_endref(data, "prg_id\":");
		pos_end = find_pos_for_substring_from_pos(pos_st,data, ",");
		create_substring_from_pos(temp_data, data, pos_st, pos_end);
		rec_prg_id = strtol(temp_data, NULL, 10);
		//ESP_LOGI(TAG,"PRG ID = %d",rec_prg_id);
	}



	if((rec_prg_id>0)&&(rec_prg_id<16))
	{
		//Extracting Program Status
		if(check_for_presence_string(data,"status"))
		{
			pos_st = find_pos_for_substring_endref(data, "status\":");
			pos_end = find_pos_for_substring_from_pos(pos_st,data, ",");
			create_substring_from_pos(temp_data, data, pos_st, pos_end);
			display_prograns[rec_prg_id].prg_status = strtol(temp_data, NULL, 10);
			//ESP_LOGI(TAG,"Status = %d",display_prograns[rec_prg_id].prg_status);
			if(display_prograns[rec_prg_id].prg_status==TRUE)
				nothing_to_disp = FALSE;
		}

//		//Extracting Program mode
		if(check_for_presence_string(data,"mode"))
		{
			pos_st = find_pos_for_substring_endref(data, "mode\":");
			pos_end = find_pos_for_substring_from_pos(pos_st,data, ",");
			create_substring_from_pos(temp_data, data, pos_st, pos_end);
			display_prograns[rec_prg_id].prg_mode = strtol(temp_data, NULL, 10);
			//ESP_LOGI(TAG,"Mode = %d",display_prograns[rec_prg_id].prg_mode);
		}

		//Extracting Brightness
		if(check_for_presence_string(data,"bright"))
		{
			pos_st = find_pos_for_substring_endref(data, "bright\":");
			pos_end = find_pos_for_substring_from_pos(pos_st,data, ",");
			create_substring_from_pos(temp_data, data, pos_st, pos_end);
			display_prograns[rec_prg_id].prg_bright = strtol(temp_data, NULL, 10);
			//ESP_LOGI(TAG,"Brightness = %d",display_prograns[rec_prg_id].prg_bright);
		}

		//Extracting Program Seconds
		if(check_for_presence_string(data,"prg_sec"))
		{
			pos_st = find_pos_for_substring_endref(data, "prg_sec\":");
			pos_end = find_pos_for_substring_from_pos(pos_st,data, ",");
			create_substring_from_pos(temp_data, data, pos_st, pos_end);
			display_prograns[rec_prg_id].prg_sec = strtol(temp_data, NULL, 10);
			//ESP_LOGI(TAG,"Program Seconds = %d",display_prograns[rec_prg_id].prg_sec);
		}

		//Extracting Repeat Count
		if(check_for_presence_string(data,"repeat_cnt"))
		{
			pos_st = find_pos_for_substring_endref(data, "repeat_cnt\":");
			pos_end = find_pos_for_substring_from_pos(pos_st,data, ",");
			create_substring_from_pos(temp_data, data, pos_st, pos_end);
			display_prograns[rec_prg_id].prg_repeat_cnt = strtol(temp_data, NULL, 10);
			//ESP_LOGI(TAG,"Program Repeat Count = %d",display_prograns[rec_prg_id].prg_repeat_cnt);
		}

		//Extracting Expire Time
		if(check_for_presence_string(data,"expire_at"))
		{
			pos_st = find_pos_for_substring_endref(data, "expire_at\":");
			pos_end = find_pos_for_substring_from_pos(pos_st,data, ",");
			create_substring_from_pos(temp_data, data, pos_st, pos_end);
			display_prograns[rec_prg_id].prg_exp_tm = strtol(temp_data, NULL, 10);
			//ESP_LOGI(TAG,"Program Expire Time = %llu",display_prograns[rec_prg_id].prg_exp_tm);
		}

		//Extracting Language
		if(check_for_presence_string(data,"lang"))
		{
			pos_st = find_pos_for_substring_endref(data, "lang\":");
			pos_end = find_pos_for_substring_from_pos(pos_st,data, ",");
			create_substring_from_pos(temp_data, data, pos_st, pos_end);

			if(temp_data[1]=='e')
				display_prograns[rec_prg_id].dis_lang=en;
			else if(temp_data[1]=='h')
				display_prograns[rec_prg_id].dis_lang=hi;
			else if(temp_data[1]=='m')
				display_prograns[rec_prg_id].dis_lang=ml;

			//ESP_LOGI(TAG,"Program Language = %d",display_prograns[rec_prg_id].dis_lang);
		}

		//Extracting data aaray
		if(check_for_presence_string(data,"data_array"))
		{

			for(uint8_t i=0; i<7; i++)
			{
				int temp_data2_len = og_size/3;
				char *temp_data2 = (char*)malloc(og_size/3);
				memset(temp_data2, 0, temp_data2_len);
				char line_label[10];
				sprintf(line_label,"line%d\":",i+1);
				//ESP_LOGE(TAG,"Line name = %s",line_label);
				if(check_for_presence_string(data,line_label))
				{

					pos_st = find_pos_for_substring_endref(data, line_label);
					pos_end = find_pos_for_substring_from_pos(pos_st,data, "},")+1;
					create_substring_from_pos(temp_data, data, pos_st, pos_end);
					//ESP_LOGE(TAG,"Line data = %s",temp_data);

					//Extracting type
					if(check_for_presence_string(temp_data,"type"))
					{
						memset(temp_data2, 0, temp_data2_len);
						pos_st = find_pos_for_substring_endref(temp_data,"type\":");
						pos_end = find_pos_for_substring_from_pos(pos_st,temp_data, "}");
						create_substring_from_pos(temp_data2, temp_data, pos_st, pos_end);
						temp_data2[pos_end] = '\0';
						display_prograns[rec_prg_id].prg_lines[i].type = strtol(temp_data2, NULL, 10);
						//ESP_LOGI(TAG,"Line %d Type = %d",i,display_prograns[rec_prg_id].prg_lines[i].type);
					}
					if(display_prograns[rec_prg_id].prg_lines[i].type == 2)
					{
						if(check_for_presence_string(temp_data,"eta"))
						{
							memset(temp_data2, 0, temp_data2_len);
							pos_st = find_pos_for_substring_endref(temp_data,"eta\":");
							pos_end = find_pos_for_substring_from_pos(pos_st,temp_data, ",");
							create_substring_from_pos(temp_data2, temp_data, pos_st, pos_end);
							temp_data2[pos_end] = '\0';
							display_prograns[rec_prg_id].prg_lines[i].eta = strtol(temp_data2, NULL, 10);
							//ESP_LOGI(TAG,"Line %d ETA = %d",i,display_prograns[rec_prg_id].prg_lines[i].eta);
						}
						//Extracting Route No
						if(check_for_presence_string(temp_data, "route_no"))
						{
							memset(temp_data2, 0, temp_data2_len);
							pos_st = find_pos_for_substring_endref(temp_data,"route_no\":")+1;
							pos_end = find_pos_for_substring_from_pos(pos_st,temp_data, "\",");
							create_substring_from_pos(temp_data2, temp_data, pos_st, pos_end);
							temp_data2[pos_end] = '\0';

							//temp_data2 = "40L";
							if(strlen(temp_data2)<8)
							{
								uint8_t n;
								for(n =0; n< strlen(temp_data2); n++)
								{
									display_prograns[rec_prg_id].prg_lines[i].route_no[n] = temp_data2[n];
								}
								display_prograns[rec_prg_id].prg_lines[i].route_no[n] = '\0';
							}
							else
								memset(display_prograns[rec_prg_id].prg_lines[i].route_no, ' ', 4);

							//display_prograns[rec_prg_id].prg_lines[i].route_no = strtol(temp, NULL, 10);
							//ESP_LOGE(TAG,"Line %d Route No = %s",i,display_prograns[rec_prg_id].prg_lines[i].route_no);
							//display_prograns[rec_prg_id].prg_lines[i].route_no = strtol(temp_data2, NULL, 10);
							//ESP_LOGI(TAG,"Start %d End pos No = %d",pos_st,pos_end);
							//ESP_LOGI(TAG,"Line %d Route No = %s",i,temp_data2);
						}
					}
					//Extracting Graphics
					if(check_for_presence_string(temp_data, "graphics"))
					{
						memset(temp_data2, 0, temp_data2_len);
						pos_st = find_pos_for_substring_endref(temp_data,"graphics\":");
						pos_end = find_pos_for_substring_from_pos(pos_st,temp_data, ",");
						create_substring_from_pos(temp_data2, temp_data, pos_st, pos_end);
						temp_data2[pos_end] = '\0';
						display_prograns[rec_prg_id].prg_lines[i].graphics = strtol(temp_data2, NULL, 10);
						//ESP_LOGI(TAG,"Line %d Graphics No = %d",i,display_prograns[rec_prg_id].prg_lines[i].graphics);
					}

					//Extracting Text
					//ESP_LOGE(TAG,"Line data = %s",temp_data);
					if(check_for_presence_string(temp_data, "text"))
					{
						char data[256] = {0};
						char tempe[3]={0};
						int j;
						pos_st = find_pos_for_substring_endref(temp_data,"text\":\"");
						pos_end = find_pos_for_substring_from_pos(pos_st,temp_data, "\",");
						create_substring_from_pos(data, temp_data, pos_st, pos_end);
						data[pos_end]='\0';
						//ESP_LOGI(TAG,"Final String  = %s",data);
						//printf("Final String  = %s\n",data);
						//Clear the content
						memset(display_prograns[rec_prg_id].prg_lines[i].text, 0, 256);

						if(strlen(data)%4==0)
						{
							j=0;
							while( j < strlen(data))
							{
								tempe[0] = data[j];
								tempe[1] = data[j+1];
								int t = (int)strtol(tempe, NULL, 16);
								if(t == 0)
									t=1;

								display_prograns[rec_prg_id].prg_lines[i].text[j/2]=t;
								j+=2;
								//printf("Converted two digit number is = %X\n", t);
							}
//							if((display_prograns[rec_prg_id].dis_lang==en)&&(i%2==0))
//							{
//								uint8_t l;
//								for(l = 0; l<16; l++)
//								{
//									display_prograns[rec_prg_id].prg_lines[i].text[(j/2)+l*2] = 1;
//									display_prograns[rec_prg_id].prg_lines[i].text[(j/2)+l*2+1] = 65;
//								}
//								display_prograns[rec_prg_id].prg_lines[i].text[(j/2)+l*2] = '\0';
//							}
//							else
								display_prograns[rec_prg_id].prg_lines[i].text[j/2] = '\0';
						}
						else
						{
							//ESP_LOGI(TAG,"INVALID DATA Received");
							display_prograns[rec_prg_id].prg_lines[i].text[0] = '\0';
						}
					}
				}
				heap_caps_free(temp_data2);

			}

		}
		//print_prg(rec_prg_id);
	}
	//memset(temp_data, og_size, 0);

	heap_caps_free(temp_data);
	return;
}



void process_received_graphics(char *data)
{
	int og_size = strlen(data);
	int pos_st = 0, pos_end = 0;

	char *temp_data = (char*)malloc(og_size);
	uint8_t rec_grph_id =0;



	for(uint8_t i=1; i<10; i++)
	{
		char *temp_data2 = (char*)malloc(og_size);;
		char grph_label[20]={0};
		rec_grph_id = 0;
		sprintf(grph_label,"graphics%d\":",i);
		if(check_for_presence_string(data,grph_label))
		{
			pos_st = find_pos_for_substring_endref(data,grph_label);
			pos_end = find_pos_for_substring_from_pos(pos_st,data, "}");
			create_substring_from_pos(temp_data, data, pos_st, pos_end+1);
			//Extracting graphics_id
			if(check_for_presence_string(temp_data,"grph_id"))
			{
				pos_st = find_pos_for_substring_endref(temp_data,"grph_id\":");
				pos_end = find_pos_for_substring_from_pos(pos_st,temp_data, ",");
				create_substring_from_pos(temp_data2, temp_data, pos_st, pos_end);
				rec_grph_id = strtol(temp_data2, NULL, 10);
//				ESP_LOGI(TAG,"Received Graphics id = %d",rec_grph_id);

				if((rec_grph_id>0)&&(rec_grph_id<10)&&check_for_presence_string(temp_data,"width")&&check_for_presence_string(temp_data,"graphics_data"))
				{
					//Extracting Width
					pos_st = find_pos_for_substring_endref(temp_data,"width\":");
					pos_end = find_pos_for_substring_from_pos(pos_st,temp_data, "}");
					create_substring_from_pos(temp_data2, temp_data, pos_st, pos_end);
					graphics_data[rec_grph_id-1][0] = strtol(temp_data2, NULL, 10);
//					ESP_LOGI(TAG,"Width of Graphics = %d",graphics_data[rec_grph_id-1][0]);

					//Extracting Graphics data
					pos_st = find_pos_for_substring_endref(temp_data,"graphics_data\":[");
					pos_end = find_pos_for_substring_from_pos(pos_st,temp_data, "],");
					create_substring_from_pos(temp_data2, temp_data, pos_st, pos_end);
					temp_data2[pos_end-pos_st] = ',';
					//ESP_LOGI(TAG,"Received graphics data = %s",temp_data2);

					//Extracting Data
					pos_st = 0;
					uint8_t temp_gr[32] = {0};
					uint8_t j = 0;
					for(j = 0; j<32; j++)
					{
						char temp[5] = {0};
						pos_end = find_pos_for_substring_from_pos(pos_st,temp_data2, ",");
						if(pos_end)
						{
							create_substring_from_pos(temp, temp_data2, pos_st, pos_end);
//							graphics_data[rec_grph_id-1][j+1] = strtol(temp, NULL, 10);
							//ESP_LOGI(TAG,"Extracted Digit = %ld",strtol(temp, NULL, 10));
							temp_gr[j] = strtol(temp, NULL, 10);
							pos_st = pos_end+1;
						}
						else
							break;
					}
					if(j==32)	//means we received valid data
					{
						for(j = 0; j<16; j++)
						{
							graphics_data[rec_grph_id-1][j+1] = (temp_gr[j*2]<<8) | (temp_gr[j*2+1]);
//							ESP_LOGI(TAG,"Extracted Graphics = %x",graphics_data[rec_grph_id-1][j+1]);
						}
						//save_updated_grapcics();
					}
				}

			}

		}
		heap_caps_free(temp_data2);
	}
	heap_caps_free(temp_data);
	return;
}




/*
 * Function to process received responce
 */

void process_received_responce(char *data)
{
	int og_size = strlen(data);

	//ESP_LOGE(TAG,"Original Size = %d",og_size);
	if(og_size<50)
		return;

	if(check_for_presence_string(data,"graphics_data"))
	{
		process_received_graphics(data);
		return;
	}

	//char temp_data[og_size];
	char *temp_data = (char*)malloc(og_size);
	//ESP_LOGI(TAG,"Available free Size11 = %d",esp_get_free_internal_heap_size());
	//char *temp_data = (char*)malloc(og_size); //[og_size];
	int pos_st = 0, pos_end = 0;

	//sprintf(temp_data,"%s", data);
	if(check_for_presence_string(data,"pg1")&&check_for_presence_string(data,"pg2"))
	{
//		data = strstr(data, "pg1");
		pos_st = find_pos_for_substring(data, "pg1");
		pos_end = find_pos_for_substring(data, "pg2");
		//strxfrm(temp_data, data, pos);
		memset(temp_data, 0, og_size);
		create_substring_from_pos(temp_data, data, pos_st, pos_end);
		//temp_data[pos_end+1] = '\0';
		//ESP_LOGE(TAG,"Copied String1 = %s",temp_data);
		process_received_prg(temp_data);
		//data = strstr(data, "pg2");
	}
	if(check_for_presence_string(data,"pg2")&&check_for_presence_string(data,"pg3"))
	{
		pos_st = pos_end;
		pos_end = find_pos_for_substring(data, "pg3");
		//strxfrm(temp_data, data, pos);
		memset(temp_data, 0, og_size);
		create_substring_from_pos(temp_data, data, pos_st, pos_end);
		//temp_data[pos_end+1] = '\0';
		//ESP_LOGE(TAG,"Copied String2 = %s",temp_data);
		process_received_prg(temp_data);
		//data = strstr(data, "pg3");
	}
	if(check_for_presence_string(data,"pg3")&&check_for_presence_string(data,"rst_cntrl"))
	{
		pos_st = pos_end;
		pos_end = find_pos_for_substring(data, "rst_cntrl");
		memset(temp_data, 0, og_size);
		create_substring_from_pos(temp_data, data, pos_st, pos_end);
		process_received_prg(temp_data);
	}
	//memset(temp_data, 0, og_size);
	//temp_data = strstr(data, "pg3");
	//ESP_LOGE(TAG,"Copied String3 = %s",data);
	//process_received_prg(temp_data);
	//memset(temp_data, 0, og_size);
//
	if(check_for_presence_string(data, "rst_cntrl"))
	{
		memset(temp_data, 0, og_size);
		//temp_data = strstr(data, "rst_cntrl");
		pos_st = find_pos_for_substring_endref(data, "rst_cntrl\":");
		pos_end = find_pos_for_substring_from_pos(pos_st,data, "}");
		create_substring_from_pos(temp_data, data, pos_st, pos_end);

//		char temp[3];
//		if((pos2-pos1)<4)
//		{
//			for(uint8_t i = pos1+1; i<pos2; i++)
//			{
//				temp[i-pos1-1] = data[i];
//			}
//		}
//		temp[2] = '\0';
		ESP_LOGI(TAG,"Reset Action = %ld",strtol(temp_data, NULL, 10));
		if(strtol(temp_data, NULL, 10))
		{
			esp_restart();
		}
	}
	heap_caps_free(temp_data);

	//ESP_LOGI(TAG,"Available free Size12 = %d",esp_get_free_internal_heap_size());
	return;
}



void process_received_str(char *data)
{
	int og_size = strlen(data);
	//ESP_LOGE(TAG,"Original Size = %d",og_size);
	//char *temp = heap_caps_malloc(og_size,NULL);
	char temp_data[og_size];
	uint8_t rec_prg_id =0;

	//ESP_LOGE(TAG,"Copied String = %s",temp);

	char *temp;

	sprintf(temp_data,"%s", data);
	if(check_for_presence_string(temp_data,"prg_id"))
	{
		temp = strstr(temp_data, "prg_id");
		temp = strtok(temp, ",");
		temp = strtok(temp, ":");
		temp = strtok(0, ":");
		//ESP_LOGI(TAG,"Result = %s",temp);
		rec_prg_id = strtol(temp, NULL, 10);
		ESP_LOGI(TAG,"PRG ID = %d",rec_prg_id);
	}

	if((rec_prg_id>0)&&(rec_prg_id<16))
	{
		//Extracting Program Status
		sprintf(temp_data,"%s", data);
		if(check_for_presence_string(temp_data,"status"))
		{
			temp = strstr(temp_data, "status");
			temp = strtok(temp, ",");
			temp = strtok(temp, ":");
			temp = strtok(0, ":");
			display_prograns[rec_prg_id].prg_status = strtol(temp, NULL, 10);
			ESP_LOGI(TAG,"Status = %d",display_prograns[rec_prg_id].prg_status);
			if(display_prograns[rec_prg_id].prg_status==TRUE)
				nothing_to_disp = FALSE;
		}

		//Extracting Program mode
		sprintf(temp_data,"%s", data);
		if(check_for_presence_string(temp_data,"mode"))
		{
			temp = strstr(temp_data, "mode");
			temp = strtok(temp, ",");
			temp = strtok(temp, ":");
			temp = strtok(0, ":");
			display_prograns[rec_prg_id].prg_mode = strtol(temp, NULL, 10);
			ESP_LOGI(TAG,"Mode = %d",display_prograns[rec_prg_id].prg_mode);
		}

		//Extracting Brightness
		sprintf(temp_data,"%s", data);
		if(check_for_presence_string(temp_data,"bright"))
		{
			temp = strstr(temp_data, "bright");
			temp = strtok(temp, ",");
			temp = strtok(temp, ":");
			temp = strtok(0, ":");
			display_prograns[rec_prg_id].prg_bright = strtol(temp, NULL, 10);
			ESP_LOGI(TAG,"Brightness = %d",display_prograns[rec_prg_id].prg_bright);
		}

		//Extracting Program Seconds
		sprintf(temp_data,"%s", data);
		if(check_for_presence_string(temp_data,"prg_sec"))
		{
			temp = strstr(temp_data, "prg_sec");
			temp = strtok(temp, ",");
			temp = strtok(temp, ":");
			temp = strtok(0, ":");
			display_prograns[rec_prg_id].prg_sec = strtol(temp, NULL, 10);
			ESP_LOGI(TAG,"Program Seconds = %d",display_prograns[rec_prg_id].prg_sec);
		}

		//Extracting Repeat Count
		sprintf(temp_data,"%s", data);
		if(check_for_presence_string(temp_data,"repeat_cnt"))
		{
			temp = strstr(temp_data, "repeat_cnt");
			temp = strtok(temp, ",");
			temp = strtok(temp, ":");
			temp = strtok(0, ":");
			display_prograns[rec_prg_id].prg_repeat_cnt = strtol(temp, NULL, 10);
			ESP_LOGI(TAG,"Program Repeat Count = %d",display_prograns[rec_prg_id].prg_repeat_cnt);
		}

		//Extracting Expire Time
		sprintf(temp_data,"%s", data);
		if(check_for_presence_string(temp_data,"expire_at"))
		{
			temp = strstr(temp_data, "expire_at");
			temp = strtok(temp, ",");
			temp = strtok(temp, ":");
			temp = strtok(0, ":");
			display_prograns[rec_prg_id].prg_exp_tm = 1696549817;//strtol(temp, NULL, 10);
			ESP_LOGI(TAG,"Program Expire Time = %llu",display_prograns[rec_prg_id].prg_exp_tm);
		}

		//Extracting Language
		sprintf(temp_data,"%s", data);
		if(check_for_presence_string(temp_data,"lang"))
		{
			temp = strstr(temp_data, "lang");
			temp = strtok(temp, ",");
			temp = strtok(temp, ":");
			temp = strtok(0, ":");
			if(temp[1]=='e')
				display_prograns[rec_prg_id].dis_lang=en;
			else if(temp[1]=='h')
				display_prograns[rec_prg_id].dis_lang=hi;
			else if(temp[1]=='m')
				display_prograns[rec_prg_id].dis_lang=ml;

			ESP_LOGI(TAG,"Program Language = %d",display_prograns[rec_prg_id].dis_lang);
		}

		//Extracting data aaray
		sprintf(temp_data,"%s", data);
		//strcpy(temp_data, data);

		if(check_for_presence_string(temp_data,"data_array"))
		{
			//temp = strtok(temp_data, "}}");
			//ESP_LOGE(TAG,"Data Array = %s",temp_data);
			for(uint8_t i=0; i<7; i++)
			{
				char line_label[6];
				sprintf(line_label,"line%d",i+1);
				sprintf(temp_data,"%s", data);
				//ESP_LOGE(TAG,"Line name = %s",line_label);
				if(check_for_presence_string(temp_data,line_label))
				{
					temp = strstr(temp_data, line_label);
					temp = strtok(temp, "}");
					//temp = strtok(0, "}");
					//ESP_LOGE(TAG,"Line data = %s",temp);
					char temp2[strlen(temp)];
					sprintf(temp_data,"%s", temp);
					sprintf(temp2,"%s", temp_data);

					//Extracting type
					if(check_for_presence_string(temp_data,"type"))
					{
						temp = strstr(temp_data, "type");
						temp = strtok(temp, ",");
						temp = strtok(temp, ":");
						temp = strtok(0, ":");
						display_prograns[rec_prg_id].prg_lines[i].type = strtol(temp, NULL, 10);
						ESP_LOGI(TAG,"Line %d Type = %d",i,display_prograns[rec_prg_id].prg_lines[i].type);
					}
					if(display_prograns[rec_prg_id].prg_lines[i].type == 2)
					{
						if(check_for_presence_string(temp_data,"eta"))
						{
							temp = strstr(temp_data, "eta");
							temp = strtok(temp, ",");
							temp = strtok(temp, ":");
							temp = strtok(0, ":");
							display_prograns[rec_prg_id].prg_lines[i].eta = strtol(temp, NULL, 10);
							ESP_LOGI(TAG,"Line %d ETA = %d",i,display_prograns[rec_prg_id].prg_lines[i].eta);
						}
						//Extracting Route No
						sprintf(temp_data,"%s", temp2);
						if(check_for_presence_string(temp_data, "route_no"))
						{
							temp = strstr(temp_data, "route_no");
							temp = strtok(temp, ",");
							temp = strtok(temp, ":");
							temp = strtok(0, ":");
							//display_prograns[rec_prg_id].prg_lines[i].route_no = strtol(temp, NULL, 10);
							ESP_LOGI(TAG,"Line %d Route No = %s",i,display_prograns[rec_prg_id].prg_lines[i].route_no);
						}
					}
					//Extracting Graphics
					sprintf(temp_data,"%s", temp2);
					if(check_for_presence_string(temp_data, "graphics"))
					{
						temp = strstr(temp_data, "graphics");
						temp = strtok(temp, ",");
						temp = strtok(temp, ":");
						temp = strtok(0, ":");
						display_prograns[rec_prg_id].prg_lines[i].graphics = strtol(temp, NULL, 10);
						ESP_LOGI(TAG,"Line %d Graphics No = %d",i,display_prograns[rec_prg_id].prg_lines[i].graphics);
					}

					//Extracting Text
					sprintf(temp_data,"%s", temp2);
					ESP_LOGE(TAG,"Line data = %s",temp_data);
					if(check_for_presence_string(temp_data, "text"))
					{
						char data[256];
						char tempe[3]={0};
						int j;
						temp = strstr(temp_data, "text");
						temp = strtok(temp, ",");
						temp = strtok(temp, ":");
						temp = strtok(0, ":");
						//sprintf(data,"%s",cJSON_Print(cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_GetObjectItem(root,"data_array"), item_name),"text")));
						sprintf(data,"%s",temp);
						//Removing the double quote in received data
						j=1;
						while((j<strlen(data))&&(data[j]!=0x22))
						{
							data[j-1]=data[j];
							j++;
						}
						data[j-1]='\0';
						printf("Final String  = %s\n",data);
						//Clear the content
						memset(display_prograns[rec_prg_id].prg_lines[i].text, 0, 256);

						if(strlen(data)%4==0)
						{
							j=0;
							while( j < strlen(data))
							{
								tempe[0] = data[j];
								tempe[1] = data[j+1];
								int t = (int)strtol(tempe, NULL, 16);
								if(t == 0)
									t=1;

								display_prograns[rec_prg_id].prg_lines[i].text[j/2]=t;
								j+=2;
								//printf("Converted two digit number is = %X\n", t);
							}
							display_prograns[rec_prg_id].prg_lines[i].text[j/2] = '\0';
						}
						else
						{
							display_prograns[rec_prg_id].prg_lines[i].text[0] = '\0';
						}
					}
				}

			}
		}
		//print_prg(rec_prg_id);
	}

}

/**Event handeler for HTTP **/
esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
	static char *output_buffer;  // Buffer to store response of http request from event handler
	static int output_len;       // Stores number of bytes read
	static int buffer_len;
	switch(evt->event_id)
	{
	 case HTTP_EVENT_ERROR:
		 ESP_LOGE(TAG, "HTTP_EVENT_ERROR");
	     break;

	 case HTTP_EVENT_ON_CONNECTED:
		 ESP_LOGE(TAG, "HTTP_EVENT_ON_CONNECTED");
	     break;

	 case HTTP_EVENT_HEADER_SENT:
		 ESP_LOGE(TAG, "HTTP_EVENT_HEADER_SENT");
	     break;

	 case HTTP_EVENT_ON_HEADER:
		 ESP_LOGE(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
	     break;

	 case HTTP_EVENT_ON_DATA:
		 //ESP_LOGE(TAG, "HTTP_EVENT_ON_DATA = %.*s" ,evt->data_len, (char*) evt->data );
		 /*
		  * Check for chunk Encoding and according to read responce
		  * */
		 if (!esp_http_client_is_chunked_response(evt->client))
		 {
			 // If user_data buffer is configured, copy the response into the buffer
		     if (evt->user_data)
		     {
		    	 memcpy(evt->user_data + output_len, evt->data, evt->data_len);
		     }
		      else
		      {
		    	  if (output_buffer == NULL)
		    	  {
		    		  //ESP_LOGI(TAG,"Available free Size11 = %d",esp_get_free_internal_heap_size());
		    		  output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
		    		  buffer_len = esp_http_client_get_content_length(evt->client);
		              output_len = 0;
		              if (output_buffer == NULL)
		              {
		            	  ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
		                  return ESP_FAIL;
		              }
		          }
		          memcpy(output_buffer + output_len, evt->data, evt->data_len);
		      }
		      output_len += evt->data_len;
		 }
		 break;


	 case HTTP_EVENT_ON_FINISH:
		 ESP_LOGE(TAG, "HTTP_EVENT_ON_FINISH");
	     if (output_buffer != NULL)
	     {
	    	 // Response is accumulated in output_buffer. the below line to print the accumulated response
	    	 //ESP_LOGE(TAG, "Responce Length = %d", output_len);
	    	// ESP_LOGE(TAG, "Received Responce = %.*s" ,output_len, (char*) output_buffer );

	    	 //ESP_LOGI(TAG,"Available free Size at Start = %d",esp_get_free_internal_heap_size());
	    	 //process_received_responce(output_buffer);
	    	 //process_received_str(output_buffer);
	    	 process_received_responce(output_buffer);

	         heap_caps_free(output_buffer);
	         //ESP_LOGI(TAG,"Available free Size12 = %d",esp_get_free_internal_heap_size());
	         output_buffer = NULL;

	     }
	     output_len = 0;
	     break;

	 case HTTP_EVENT_DISCONNECTED:
		 ESP_LOGE(TAG, "HTTP_EVENT_DISCONNECTED");

	     int mbedtls_err = 0;
//	     if (output_buffer != NULL)
//		 {
//			 heap_caps_free(output_buffer);
//			 //ESP_LOGE(TAG,"Cleared the output Buffer");
//			 output_buffer = NULL;
//		 }
	     esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
	     if (err != 0)
	     {
	    	 if (output_buffer != NULL)
	    	 {
	    		 heap_caps_free(output_buffer);
	    		 //ESP_LOGE(TAG,"Cleared the output Buffer");
	             output_buffer = NULL;
	         }
	         output_len = 0;
	         ESP_LOGE(TAG, "Last esp error code: 0x%x", err);
	         ESP_LOGE(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
	     }
	     break;

	 default:
		 break;
	}
	return ESP_OK;
}



void server_comm_init(void)
{
	//ets_printf("Available free Size at Start= %d\n",esp_get_free_internal_heap_size())
	esp_http_client_config_t http_config = {
											 .url = "https://smarttvm.in:9000/api/GetUpdateProgramData",
											 .method = HTTP_METHOD_GET,
											 .cert_pem = NULL,//(char *)server_cert_httpbin_pem_start,
											 .buffer_size= MAX_HTTP_OUTPUT_BUFFER,
											 .timeout_ms = MAX_HTTP_TIMOUT,
											 .event_handler = http_event_handler};
	client = esp_http_client_init(&http_config);
//	esp_http_client_set_header(client,"device_id","24:0A:C4:80:1F:68");
	esp_http_client_set_header(client,"Device-id",mac_str);
	//esp_http_client_set_header(client,"Device-id","a0:b7:65:db:ed:28");		//For testing

}

/*HTTP Request Send*/
esp_err_t send_https_request(void)
{
//	//ets_printf("Available free Size at Start= %d\n",esp_get_free_internal_heap_size())
	esp_err_t err;
//	esp_http_client_config_t http_config = {
//											 .url = "https://smarttvm.in:9000/api/GetUpdateProgramData",
//											 .method = HTTP_METHOD_GET,
//											 .cert_pem = NULL,//(char *)server_cert_httpbin_pem_start,
//											 .buffer_size= MAX_HTTP_OUTPUT_BUFFER,
//											 .timeout_ms = MAX_HTTP_TIMOUT,
//											 .event_handler = http_event_handler};
//	esp_http_client_handle_t client = esp_http_client_init(&http_config);
////	esp_http_client_set_header(client,"device_id","24:0A:C4:80:1F:68");
//	esp_http_client_set_header(client,"Device-id",mac_str);
	err = esp_http_client_perform(client);
	//esp_http_client_close(client);
	//esp_http_client_cleanup(client);
	return err;
	//free(client);
	//ets_printf("Available free Size at End= %d\n",esp_get_free_internal_heap_size());

}


esp_err_t update_graphics(void)
{
	static uint8_t cn = 0;
	esp_http_client_config_t http_config = {
											 .url = "https://smarttvm.in:9000/api/GetGraphicsData",
											 .method = HTTP_METHOD_GET,
											 .cert_pem = NULL,//(char *)server_cert_httpbin_pem_start,
											 .buffer_size= MAX_HTTP_OUTPUT_BUFFER,
											 .timeout_ms = MAX_HTTP_TIMOUT,
											 .event_handler = http_event_handler};
	esp_http_client_handle_t client2 = esp_http_client_init(&http_config);
	esp_http_client_set_header(client2,"Device-id",mac_str);
	esp_err_t err;

	while (1)
	{
		err = esp_http_client_perform(client2);
		if((err == ESP_OK)||(cn >= MAX_GRPH_RETRY_COUNT))
		{
			esp_http_client_close(client2);
			esp_http_client_cleanup(client2);

			if(err == ESP_OK)
				return ESP_OK;
			else
				return ESP_FAIL;
		}
		else
			cn++;

		TIMERG0.wdt_wprotect = TIMG_WDT_WKEY_VALUE;
		TIMERG0.wdt_feed = 1;
		TIMERG0.wdt_wprotect = 0;
		vTaskDelay(100);
	}




}


void print_prg(int prg_id)
{
printf("Program id is %d\n", display_prograns[prg_id].prg_id);
printf("Repeat count is %d\n", display_prograns[prg_id].prg_repeat_cnt);
printf("Program second is %d\n", display_prograns[prg_id].prg_sec);
printf("Status is %d\n", display_prograns[prg_id].prg_status);
printf("Mode is %d\n", display_prograns[prg_id].prg_mode);
printf("Language is %d\n", display_prograns[prg_id].dis_lang);
printf("Expiry time is %lld\n", display_prograns[prg_id].prg_exp_tm);
printf("Brightness is %d\n", display_prograns[prg_id].prg_bright);
printf("\n\n");

for(uint8_t i =0; i<4; i++)
{
	printf("Line = %d\n",i+1);
	printf("Type is %d\n", display_prograns[prg_id].prg_lines[i].type);
	printf("ETA is %d\n", display_prograns[prg_id].prg_lines[i].eta);
	printf("Route number is %s\n", display_prograns[prg_id].prg_lines[i].route_no);
	printf("Graphics is %d\n", display_prograns[prg_id].prg_lines[i].graphics);
	for(uint8_t j=0;(display_prograns[prg_id].prg_lines[i].text[j]!='\0');j++)
	{
		printf("at %d = %x\n",j,display_prograns[prg_id].prg_lines[i].text[j]);
	}
	//printf("Text is %s\n", display_prograns[rec_prg_id].prg_lines[i].text);
	printf("\n");
}
}



