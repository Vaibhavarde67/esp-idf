#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include "nvs_flash.h"
#include "nvs.h"
#include "lwip/err.h"

#include "critical_alarm_data_structure.h"



uint16_t calculateCRC(uint8_t *rcvdArray, uint16_t bufferSize)
{
	uint16_t calculatedCRC, temp, flag;
  uint16_t i,j;

  calculatedCRC = 0xFFFF;
  for (i = 0; i < bufferSize; i++)
  {
    calculatedCRC = calculatedCRC ^ rcvdArray[i];
    for (j = 1; j <= 8; j++)
    {
      flag = calculatedCRC & 0x0001;
      calculatedCRC >>= 1;
      if (flag)
        calculatedCRC ^= 0xA001;
    }
  }
  // Reverse byte order.
  temp = calculatedCRC >> 8;
  calculatedCRC = (calculatedCRC << 8) | temp ;
  calculatedCRC &= 0xFFFF ;
  // the returned value is already swapped
  // crcLo byte is first & crcHi byte is last
	return calculatedCRC;

}


esp_err_t read_io_board_data(uint8_t io_bord_num)
{
	uint8_t data_arr[IO_BORD_DATA_SIZE+2];		//+2 for CRC
	uint16_t cal_crc, rd_crc;
	esp_err_t err;
	nvs_handle_t my_handle;
	size_t required_size = IO_BORD_DATA_SIZE+2;
	char key_str[10];

	if(io_bord_num > MAX_NO_OF_IO_BOARD)
		return ESP_FAIL;

	sprintf(key_str,"iobrd%d", io_bord_num);


	// Open
	err = nvs_open(IO_BORD_SETNG_NAMESPACE, NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
		return err;

	//printf("Here1\n");

	err = nvs_get_blob(my_handle, key_str, data_arr, &required_size);
	if (err != ESP_OK)
		 return err;
	//printf("Here2\n");
	// Close
	nvs_close(my_handle);

//	printf("Printing Read Array\n");
//	print_log_st(data_arr);


	cal_crc = calculateCRC(data_arr, IO_BORD_DATA_SIZE);
	rd_crc = data_arr[IO_BORD_DATA_SIZE+1];
	rd_crc = (rd_crc<<8) | data_arr[IO_BORD_DATA_SIZE];


	if(rd_crc!=cal_crc)
		return ESP_FAIL;
	//printf("Here3\n");
	memcpy(&io_board_input_data[io_bord_num-1],data_arr,IO_BORD_DATA_SIZE);

	return ESP_OK;
}




esp_err_t store_io_board_data(uint8_t io_bord_num)
{
	uint8_t data_arr[IO_BORD_DATA_SIZE+2];		//+2 for CRC
	uint16_t crc;
	esp_err_t err;
	nvs_handle_t my_handle;
	size_t required_size = IO_BORD_DATA_SIZE+2;
	char key_str[10];

	if(io_bord_num > MAX_NO_OF_IO_BOARD)
		return ESP_FAIL;

	memcpy(data_arr,&io_board_input_data[io_bord_num-1],IO_BORD_DATA_SIZE);

	//Add CRC to data
	crc = calculateCRC(data_arr, IO_BORD_DATA_SIZE);
	data_arr[IO_BORD_DATA_SIZE] = crc & 0xFF;
	data_arr[IO_BORD_DATA_SIZE+1] = (crc >> 8) & 0xFF;

	// Open
	err = nvs_open(IO_BORD_SETNG_NAMESPACE, NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
		return err;



	sprintf(key_str,"iobrd%d", io_bord_num);
	err = nvs_set_blob(my_handle, key_str, data_arr, required_size);
	if (err != ESP_OK)
		 return err;

	err = nvs_commit(my_handle);
	if (err != ESP_OK)
		return err;

	// Close
	nvs_close(my_handle);
	return read_io_board_data(io_bord_num);
}


esp_err_t read_communication_details(void)
{
	uint8_t data_arr[IO_BVOARD_COMM_DET_SIZE+2];		//+2 for CRC
	uint16_t cal_crc, rd_crc;
	esp_err_t err;
	nvs_handle_t my_handle;
	size_t required_size = IO_BVOARD_COMM_DET_SIZE+2;
	char key_str[10];

	sprintf(key_str,"iobrdcom");

	// Open
	err = nvs_open(IO_BORD_SETNG_NAMESPACE, NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
		return err;

	err = nvs_get_blob(my_handle, key_str, data_arr, &required_size);
	if (err != ESP_OK)
		 return err;

	// Close
	nvs_close(my_handle);

//	printf("Printing Read Array\n");
//	print_log_st(data_arr);


	cal_crc = calculateCRC(data_arr, IO_BVOARD_COMM_DET_SIZE);
	rd_crc = data_arr[IO_BVOARD_COMM_DET_SIZE+1];
	rd_crc = (rd_crc<<8) | data_arr[IO_BVOARD_COMM_DET_SIZE];


	if(rd_crc!=cal_crc)
		return ESP_FAIL;

	memcpy(&io_board_comm_info,data_arr,IO_BVOARD_COMM_DET_SIZE);
	return ESP_OK;
}


esp_err_t store_communication_details(void)
{
	uint8_t data_arr[IO_BVOARD_COMM_DET_SIZE+2];		//+2 for CRC
	uint16_t crc;
	esp_err_t err;
	nvs_handle_t my_handle;
	size_t required_size = IO_BVOARD_COMM_DET_SIZE+2;
	char key_str[10];

	memcpy(data_arr,&io_board_comm_info,IO_BVOARD_COMM_DET_SIZE);

	//Add CRC to data
	crc = calculateCRC(data_arr, IO_BVOARD_COMM_DET_SIZE);
	data_arr[IO_BVOARD_COMM_DET_SIZE] = crc & 0xFF;
	data_arr[IO_BVOARD_COMM_DET_SIZE+1] = (crc >> 8) & 0xFF;

	// Open
	err = nvs_open(IO_BORD_SETNG_NAMESPACE, NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
		return err;

	sprintf(key_str,"iobrdcom");
	err = nvs_set_blob(my_handle, key_str, data_arr, required_size);
	if (err != ESP_OK)
		 return err;

	err = nvs_commit(my_handle);
	if (err != ESP_OK)
		return err;

	// Close
	nvs_close(my_handle);
	return read_communication_details();
}


esp_err_t store_io_board_setting(uint8_t type_setting)
{
	esp_err_t err;
	nvs_handle_t my_handle;

	// Open
	err = nvs_open(IO_BORD_SETNG_NAMESPACE, NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
		return err;

	switch(type_setting)
	{
	case WIFI_SETTING:
	{
		uint8_t data_arr[WIFI_DATA_SIZE];
		memcpy(data_arr,&wifi_data,WIFI_DATA_SIZE);
		err = nvs_set_blob(my_handle, "wifi_data", data_arr, WIFI_DATA_SIZE);
		if (err != ESP_OK)
			 return err;
		break;
	}
	case SCHEDULING_INFO:
	{
		uint8_t data_arr[SCHEDULING_INFO_SIZE];
		memcpy(data_arr,&scheduling_info,SCHEDULING_INFO_SIZE);
		err = nvs_set_blob(my_handle, "sch_inf", data_arr, SCHEDULING_INFO_SIZE);
		if (err != ESP_OK)
			 return err;
		break;
	}

	case DEVICE_NAME_SETTING:
	{
		err = nvs_set_blob(my_handle, "dvc_name", device_name, sizeof(device_name));
		if (err != ESP_OK)
			 return err;
		break;
	}

	case IO_BOARD_COUNT_SETTING:
	{
		nvs_set_u8(my_handle, "io_brd_cnt", present_io_board_count);
		break;
	}

	case ESCALATION_TIME_SETTING:
	{
		nvs_set_u8(my_handle, "esc_min", ecsalation_mins);
		break;
	}

	}


	err = nvs_commit(my_handle);
	if (err != ESP_OK)
		return err;

	// Close
	nvs_close(my_handle);
	return ESP_OK;
}


esp_err_t read_io_board_setting(uint8_t type_setting)
{
	esp_err_t err;
	uint8_t count;
	nvs_handle_t my_handle;

	// Open
	err = nvs_open(IO_BORD_SETNG_NAMESPACE, NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
		return ESP_FAIL;

	switch(type_setting)
	{
	case WIFI_SETTING:
	{
		uint8_t data_arr[WIFI_DATA_SIZE];
		size_t required_size = WIFI_DATA_SIZE;
		err = nvs_get_blob(my_handle, "wifi_data", data_arr, &required_size);
		if (err != ESP_OK)
			 return err;
		memcpy(&wifi_data, data_arr, WIFI_DATA_SIZE);
		break;
	}
	case SCHEDULING_INFO:
	{
		uint8_t data_arr[SCHEDULING_INFO_SIZE];
		size_t required_size = SCHEDULING_INFO_SIZE;
		err = nvs_get_blob(my_handle, "sch_inf", data_arr, &required_size);
		if (err != ESP_OK)
			 return err;
		memcpy(&scheduling_info, data_arr, SCHEDULING_INFO_SIZE);
		break;
	}

	case DEVICE_NAME_SETTING:
	{
		size_t required_size = sizeof(device_name);
		err = nvs_get_blob(my_handle, "dvc_name", device_name, &required_size);
		if (err != ESP_OK)
			 return err;
		break;
	}

	case IO_BOARD_COUNT_SETTING:
	{
		nvs_get_u8(my_handle, "io_brd_cnt", &present_io_board_count);
		break;
	}

	case ESCALATION_TIME_SETTING:
	{
		nvs_get_u8(my_handle, "esc_min", &ecsalation_mins);
		break;
	}

	}

	// Close
	nvs_close(my_handle);
	return ESP_OK;
}


/*
 * Storing Default data in Critical Alarm DATA Structure
 */
void load_default_data(void)
{
	//loading default IO BOARD INPUT DATA
	uint8_t retry_cn =0;

	for(uint8_t i =0; i< MAX_NO_OF_IO_BOARD; i++)
	{
		retry_cn = 0;
		sprintf(io_board_input_data[i].io_board_name, "IO BOARD NUM %d", i);
		for(uint8_t j =0; j< NO_OF_ANALOG_IP; j++)
		{
			io_board_input_data[i].analog_input[j].limit = 4095;
			sprintf(io_board_input_data[i].analog_input[j].analog_ip_name, "IO%d ANALOG IP%d", i, j);

		}

		for(uint8_t j = 0; j< NO_OF_DIGITAL_IP; j++)
		{
			sprintf(io_board_input_data[i].digital_input[j].digital_ip_name, "IO%d DIGITAL IP%d", i, j);
		}

		for(uint8_t j = 0; j< NO_OF_RS485_SENSOR; j++)
		{
			sprintf(io_board_input_data[i].rs485_input[j].humidity_sensor.humidity_name, "IO%d SENSOR%d HUM", i, j);
			io_board_input_data[i].rs485_input[j].humidity_sensor.limit = 770+j;
			sprintf(io_board_input_data[i].rs485_input[j].temperature_sensor.temperature_name, "IO%d SENSOR%d TEM", i, j);
			io_board_input_data[i].rs485_input[j].temperature_sensor.limit = 390+j;
		}

		while(store_io_board_data(i+1) == ESP_FAIL)
		{
			retry_cn++;
			if(retry_cn > 5)
				break;
		}
	}

	//Loading default communication data
	retry_cn = 0;
	io_board_comm_info.email_srv_info.port_number = 587;
	strcpy(io_board_comm_info.email_srv_info.sender_email_server,"smtp.googlemail.com");
	strcpy(io_board_comm_info.email_srv_info.sender_email_id, "example@gmail.com");
	strcpy(io_board_comm_info.email_srv_info.sender_email_password, "dummy@123");
	strcpy(io_board_comm_info.email_srv_info.sender_name, "Synopsis");

	for(uint8_t i = 0; i< MAX_RECEIVER_EMAILS; i++)
	{
		strcpy(io_board_comm_info.rec_email_info.receiver_email[i], "example@gmail.com");
	}

	for(uint8_t i = 0; i< MAX_LEVEL1_PERSONS; i++)
	{
		strcpy(io_board_comm_info.mobile_no_info.level1_mobiles[i], "8237620541");
	}
	for(uint8_t i = 0; i< MAX_LEVEL2_PERSONS; i++)
	{
		strcpy(io_board_comm_info.mobile_no_info.level2_mobiles[i], "8237620542");
	}
	for(uint8_t i = 0; i< MAX_LEVEL3_PERSONS; i++)
	{
		strcpy(io_board_comm_info.mobile_no_info.level3_mobiles[i], "8237620543");
	}

	while(store_communication_details() == ESP_FAIL)
	{
		retry_cn++;
		if(retry_cn > 5)
			break;
	}

	present_io_board_count = 4;
	store_io_board_setting(IO_BOARD_COUNT_SETTING);

	ecsalation_mins = 5;
	store_io_board_setting(ESCALATION_TIME_SETTING);

	strcpy(wifi_data.wifi_ssid, "WIFI SSID");
	strcpy(wifi_data.wifi_pswd, "PASSWORD");
	store_io_board_setting(WIFI_SETTING);

	strcpy(device_name, "Kritical Alarm");
	store_io_board_setting(DEVICE_NAME_SETTING);

	scheduling_info.hour = 15;
	scheduling_info.Minutes = 40;
	memset(scheduling_info.days, 0, 7);
	strcpy(device_name, "Kritical Alarm");
	store_io_board_setting(SCHEDULING_INFO);
}



void init_critical_alarm_setting_data(void)
{
	//To detect empty NVS and store default graphics on it
	nvs_stats_t nvs_stats;
	nvs_get_stats("nvs", &nvs_stats);
//	printf("Used entries = %d\n",nvs_stats.used_entries);
//	printf("Total entries = %d\n",nvs_stats.total_entries);
//	printf("Namespace Count = %d\n",nvs_stats.namespace_count);
	if(nvs_stats.used_entries==0)
	{
		printf("Loading default Setting Data\n");
		load_default_data();
		return;
	}
	read_io_board_setting(IO_BOARD_COUNT_SETTING);

	uint8_t retry_cn = 0;
	for(uint8_t i = 0; i< present_io_board_count; i++)
	{
		retry_cn = 0;
		while(read_io_board_data(i+1) == ESP_FAIL)
		{
			retry_cn++;
			if(retry_cn > 5)
				break;
		}
	}
	retry_cn = 0;
	while(read_communication_details() == ESP_FAIL)
	{
		retry_cn++;
		if(retry_cn > 5)
			break;
	}


	retry_cn = 0;
	while(read_io_board_setting(SCHEDULING_INFO) == ESP_FAIL)
	{
		retry_cn++;
		if(retry_cn > 5)
			break;
	}

	retry_cn = 0;
	while(read_io_board_setting(WIFI_SETTING) == ESP_FAIL)
	{
		retry_cn++;
		if(retry_cn > 5)
			break;
	}


	read_io_board_setting(ESCALATION_TIME_SETTING);

	read_io_board_setting(DEVICE_NAME_SETTING);

}
/*
 * Storing the IO_BOARD_INPUT_DATA
 */
esp_err_t store_ip_data( uint8_t io_board_num)
{
	printf("Size of Input data = %d\n", sizeof(io_board_input_data[0]));
	return ESP_OK;
}



void print_critical_alarm_setting_data(void)
{
	//Printing IO BOARD INPUT DATA
	for(uint8_t i =0; i< present_io_board_count; i++)
	{
		printf("IO Board Name --> %s\n",io_board_input_data[i].io_board_name);
		for(uint8_t j =0; j< NO_OF_ANALOG_IP; j++)
		{
			printf("Analog INPUT %d Name --> %s\n", j, io_board_input_data[i].analog_input[j].analog_ip_name);
			printf("Limit = %d\n" ,io_board_input_data[i].analog_input[j].limit);

		}

		for(uint8_t j = 0; j< NO_OF_DIGITAL_IP; j++)
		{
			printf("DIGITAL INPUT %d Name --> %s\n", j, io_board_input_data[i].digital_input[j].digital_ip_name);
		}

		for(uint8_t j = 0; j< NO_OF_RS485_SENSOR; j++)
		{
			printf("Humidity Sensor%d Name --> %s\n", j, io_board_input_data[i].rs485_input[j].humidity_sensor.humidity_name);
			printf("Limit = %d\n", io_board_input_data[i].rs485_input[j].humidity_sensor.limit);
			printf("Temperature Sensor%d Name --> %s\n",j, io_board_input_data[i].rs485_input[j].temperature_sensor.temperature_name);
			printf("Limit = %d\n", io_board_input_data[i].rs485_input[j].temperature_sensor.limit);
		}
	}

	//Printing communication data
	printf("SRV_EMAIL_PORT = %d\n",io_board_comm_info.email_srv_info.port_number);
	printf("Sender Email Server --> %s\n",io_board_comm_info.email_srv_info.sender_email_server);
	printf("Sender Email ID --> %s\n",io_board_comm_info.email_srv_info.sender_email_id);
	printf("Sender Email Password --> %s\n",io_board_comm_info.email_srv_info.sender_email_password);
	printf("Sender Name --> %s\n",io_board_comm_info.email_srv_info.sender_name);

	for(uint8_t i = 0; i< MAX_RECEIVER_EMAILS; i++)
	{
		printf("Receiver Email No %d %s\n", i, io_board_comm_info.rec_email_info.receiver_email[i]);
	}

	for(uint8_t i = 0; i< MAX_LEVEL1_PERSONS; i++)
	{
		printf("Level1 Mobile No %d --> %s\n", i, io_board_comm_info.mobile_no_info.level1_mobiles[i]);
	}
	for(uint8_t i = 0; i< MAX_LEVEL2_PERSONS; i++)
	{
		printf("Level2 Mobile No %d --> %s\n", i, io_board_comm_info.mobile_no_info.level2_mobiles[i]);
	}
	for(uint8_t i = 0; i< MAX_LEVEL3_PERSONS; i++)
	{
		printf("Level3 Mobile No %d --> %s\n", i, io_board_comm_info.mobile_no_info.level3_mobiles[i]);
	}

	printf("Wifi SSID --> %s\n",wifi_data.wifi_ssid);
	printf("WIFI Password --> %s\n",wifi_data.wifi_pswd);

	printf("Device Name --> %s\n",device_name);

	printf("Scheduling Hour --> %d\n",scheduling_info.hour);
	printf("Scheduling Minutes --> %d\n",scheduling_info.Minutes);
	printf("Scheduling Days --> %d, %d, %d, %d, %d, %d, %d\n", scheduling_info.days[0], scheduling_info.days[1], scheduling_info.days[2], scheduling_info.days[3], scheduling_info.days[4], scheduling_info.days[5], scheduling_info.days[6]);

	printf("Number of IO Boards --> %d\n", present_io_board_count);
	printf("Escalation Minutes --> %d\n", ecsalation_mins);
}


void app_main(void)
{
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		// NVS partition was truncated and needs to be erased
		// Retry nvs_flash_init
		ESP_ERROR_CHECK(nvs_flash_erase());
		err = nvs_flash_init();
	}
	sleep(10);
	init_critical_alarm_setting_data();

	print_critical_alarm_setting_data();

    while (true) {
        printf("Hello from app_main!\n");
        sleep(100);
    }
}
