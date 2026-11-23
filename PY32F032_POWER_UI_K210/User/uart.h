#ifndef __UART_DRIVER_INTERFACE_H
#define __UART_DRIVER_INTERFACE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#define RB_BUFF_SIZE 200
#define BYTE_RECEIVE_TIMEOUT_MS 10 //10MS FOR BYTE RECIVE TIMEOUT

typedef struct
{
	uint8_t write;
	uint8_t read;
	uint8_t buff[RB_BUFF_SIZE];
} ringbuff_t;


void initUart(void);

extern ringbuff_t rb2;



#endif
