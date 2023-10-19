/*
 * server_comm.h
 *
 *  Created on: Feb 10, 2023
 *      Author: Embin 7
 */


#ifndef MAIN_SERVER_COMM_H_
#define MAIN_SERVER_COMM_H_

#include "esp_event.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "lwip/err.h"
#include <string.h>

#include "esp_http_client.h"
#include "cJSON.h"

#include "define.h"
#include "graphics.h"

#define MAX_HTTP_OUTPUT_BUFFER 2048
#define MAX_HTTP_TIMOUT 5000

#define MAX_HTTPS_RETRY_COUNT	15

#define MAX_GRPH_RETRY_COUNT	2

esp_http_client_handle_t client;

/*
 * function to send request
 */
esp_err_t send_https_request(void);
/*
 * Function to process the Receivd Responce
 */
//void process_received_responce(char *data);

/**Event handeler for HTTP **/
esp_err_t http_event_handler(esp_http_client_event_t *evt);



void process_received_prg(char *data);

void process_received_graphics(char *data);



/*
 * Function to print Program
 */
void print_prg(int prg_id);

void server_comm_init(void);

esp_err_t update_graphics(void);




#endif /* MAIN_SERVER_COMM_H_ */
