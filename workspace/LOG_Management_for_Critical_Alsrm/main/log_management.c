/*
 * log_management.c
 *
 *  Created on: Jun 19, 2023
 *      Author: Embin 7
 */

#include "log_management.h"



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


void print_log_from_st (log_data_struct *log_data)
{

	printf("Read Epoch time = %ld\n", (long)log_data->epoch_time);
	printf("Read device Id = %d\n", log_data->device_id);
	printf("Read typr = %d\n", log_data->type);
	printf("Read Input Id = %d\n",log_data->ip_id);
}


esp_err_t staore_log_entry(uint16_t log_id, log_data_struct *log_data)
{
	uint8_t data_arr[LOGENTRY_SIZE+2];		//+2 for CRC
	uint16_t crc;
	esp_err_t err;
	nvs_handle_t my_handle;
	size_t required_size = LOGENTRY_SIZE+2;
	char key_str[10];

	if(log_id > MAX_LOG_ENTRY)
		log_id = MAX_LOG_ENTRY;

	memcpy(data_arr,log_data,LOGENTRY_SIZE);

	//Add CRC to data
	crc = calculateCRC(data_arr, 9);
	data_arr[LOGENTRY_SIZE] = crc & 0xFF;
	data_arr[LOGENTRY_SIZE+1] = (crc >> 8) & 0xFF;

	// Open
	err = nvs_open(LOG_NAMESPACE, NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
		return err;

	sprintf(key_str,"lg%d", log_id);
	err = nvs_set_blob(my_handle, key_str, data_arr, required_size);
	if (err != ESP_OK)
		 return err;

	err = nvs_commit(my_handle);
	if (err != ESP_OK)
		return err;

	// Close
	nvs_close(my_handle);
	return ESP_OK;
}


esp_err_t read_log_entry(uint16_t log_id, log_data_struct *log_data)
{
	uint8_t data_arr[LOGENTRY_SIZE+2];		//+2 for CRC
	uint16_t cal_crc, rd_crc;
	esp_err_t err;
	nvs_handle_t my_handle;
	size_t required_size = LOGENTRY_SIZE+2;
	char key_str[10];
	sprintf(key_str,"lg%d", log_id);

	// Open
	err = nvs_open(LOG_NAMESPACE, NVS_READWRITE, &my_handle);
	if (err != ESP_OK)
		return err;

	err = nvs_get_blob(my_handle, key_str, data_arr, &required_size);
	if (err != ESP_OK)
		 return err;

	err = nvs_commit(my_handle);
	if (err != ESP_OK)
		return err;

	// Close
	nvs_close(my_handle);

//	printf("Printing Read Array\n");
//	print_log_st(data_arr);


	cal_crc = calculateCRC(data_arr, LOGENTRY_SIZE);
	rd_crc = data_arr[LOGENTRY_SIZE+1];
	rd_crc = (rd_crc<<8) | data_arr[LOGENTRY_SIZE];


	if(rd_crc!=cal_crc)
		return ESP_FAIL;

	memcpy(log_data,data_arr,LOGENTRY_SIZE);

	return ESP_OK;
}







