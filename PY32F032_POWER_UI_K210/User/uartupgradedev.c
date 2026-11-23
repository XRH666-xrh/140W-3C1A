#include "main.h"
#include "stdio.h"
#include "string.h"
#include "common.h"
#include "devmodule.h"
#include "ring.h"
#include "SerialFlash.h"
#include "usermain.h"
#include "pm.h"
#include "display.h"

#if USER_PRINTF
#define BUFFSIZE 	530
#define ONEFRAME	512

#define baud 115200


static u8 rxbuff[BUFFSIZE];
RingQueue ring;
static u8 headtab[18] = {0};



#define UARTID	UART1

UART_HandleTypeDef UartHandle;
#define DEBUG_UART &UartHandle


//CRC16_CCITT校验
unsigned int Rx_CRC_CCITT(unsigned char *puchMsg, unsigned int usDataLen)
{
	unsigned char i = 0;
	unsigned short wCRCin = 0x0000;
	unsigned short wCPoly = 0x1021;
	unsigned char wChar = 0;
	while (usDataLen--)
	{
		wChar = *(puchMsg++);
		wCRCin ^= (wChar << 8);
		for(i = 0; i < 8; i++)
		{
			if (wCRCin & 0x8000)
				wCRCin = (wCRCin << 1) ^ wCPoly;
			else
				wCRCin = wCRCin << 1;
		}
	}return (wCRCin);
}

void I_UART_Transmit(UART_HandleTypeDef* uartid, uint8_t* data,uint8_t len, uint8_t timout){
	HAL_UART_Transmit(uartid,data,len,timout);

}
/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
		HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 100);

    return (ch);
}

//判断文件是否超出Flash大小
static uint8_t _is_OverFlashSize(uint32_t filesize) {
#define X25QXX_KEEP_SIZE  	0x40000		/* X25qxx retains 256K size */
	//读取Flash型号,依据最低1字节判断Flash内存大小
	uint8_t flash_id = (uint8_t)readDeviceId();
	uint32_t flash_size = 0x1FF000; //2M
	switch(flash_id) {
		case 0x15: //25q16 //2M
			flash_size = 0x1FF000;
			break;
		case 0x16: //25q32 //4M
			flash_size = 0x3FF000;
			break;
		case 0x17: //25q64 //8M
			flash_size = 0x7FF000;
			break;
		case 0x18: //25q128 //16M
			flash_size = 0xFFF000;
			break;
		case 0x19: //25q256 //32M
			flash_size = 0x1FFF000;
			break;
		default : 
			break;
	}
	//判断文件是否超出Flash大小
	if(filesize > (flash_size-X25QXX_KEEP_SIZE)) {
		printf("file size over\n");
		return 1;
	}
	return 0;
}

static void processUpgrade(){
	u16 RxCRC16 = 0;
	u16 MyCRC16 = 0;
	u8 ErrorCount = 0;
	u16 datalen = 0;
	uint8_t readbuff[BUFFSIZE];
	u8 flash_succ[3] = {0xa2, 0x00, 0x00};
	u8 flash_rese[3] = {0xe0, 0x00, 0x00};
	u8 flash_erro[3] = {0xe1, 0x00, 0x00};

	DeQueue(&ring,readbuff,14);
	//for(int i =0;i < 14;i++){
//		I_UART_Transmit(DEBUG_UART, readbuff, 14, 5);
//	}
	RxCRC16 = (readbuff[12]<<8)|readbuff[13];
	memcpy(&headtab[4],readbuff,12);
	//CRC校验
	MyCRC16 = Rx_CRC_CCITT(headtab,16);
	if(RxCRC16 != MyCRC16) {	//CRC校验失败
		I_UART_Transmit(DEBUG_UART, flash_erro, 3, 1);
		//printf("my crc:%x \n",MyCRC16);
		return;
	}
	uint32_t filesize = readbuff[8]<<24|readbuff[9]<<16|readbuff[10]<<8|readbuff[11];
	uint32_t startaddr = 0;
	uint16_t updatetimeout = 0;
	//printf("start update file ,please input file:%d!!!\n",filesize);
	
	//判断文件是否超出Flash大小
	if(_is_OverFlashSize(filesize) != 0) return;
	
	fy_flash_erase_block(0,filesize);
	startaddr = 0;
	filesize +=startaddr;

	//printf("flash ready\n");
	int bytes = QueueCount(&ring);
	DeQueue(&ring,readbuff,bytes);
	memset(readbuff,0,530);
	I_UART_Transmit(DEBUG_UART, flash_succ, 3, 1);	//FLASH擦除成功开始传输文件
	while(1){
		updatetimeout++;
		if(updatetimeout > 3000){
			printf("update timeout\n");
			break;
		}
		int bytes = QueueCount(&ring);
		if(bytes > 0){
			if(((startaddr + bytes) < filesize+8)&& (bytes < 520)){
				HAL_Delay(1);
				continue;
			}
			updatetimeout = 0;
			//printf("[%d]",bytes);
			if(bytes > 520){
				DeQueue(&ring,readbuff,520);
				datalen = (readbuff[4]<<8)|readbuff[5];
				RxCRC16 = (readbuff[datalen+6]<<8)|readbuff[datalen+7];
				MyCRC16 = Rx_CRC_CCITT(&readbuff[2],datalen+4);
				if(RxCRC16 != MyCRC16) {	//CRC校验失败
					ErrorCount++;
					if(ErrorCount > 3) {
						flash_erro[1] = readbuff[2];
						flash_erro[2] = readbuff[3];
						I_UART_Transmit(DEBUG_UART, flash_erro, 3, 1);	//传输错误
						break;
					} else {
						flash_rese[1] = readbuff[2];
						flash_rese[2] = readbuff[3];
						I_UART_Transmit(DEBUG_UART, flash_rese, 3, 1);	//传输重传
					}
				} else {
					ErrorCount = 0;
					fy_flash_page_write(startaddr, &readbuff[6], 256);
					fy_flash_page_write(startaddr+256, &readbuff[6+256], 256);
					startaddr+=512;
					flash_succ[1] = readbuff[2];
					flash_succ[2] = readbuff[3];
					I_UART_Transmit(DEBUG_UART, flash_succ, 3, 1);	//传输成功
				}
				//printf("-%d\n",startaddr);
			}else{
				DeQueue(&ring,readbuff,bytes);
				datalen = (readbuff[4]<<8)|readbuff[5];
				RxCRC16 = (readbuff[datalen+6]<<8)|readbuff[datalen+7];
				MyCRC16 = Rx_CRC_CCITT(&readbuff[2],datalen+4);

				if(RxCRC16 != MyCRC16) {	//CRC校验失败
					ErrorCount++;
					if(ErrorCount > 3) {
						flash_erro[1] = readbuff[2];
						flash_erro[2] = readbuff[3];
						I_UART_Transmit(DEBUG_UART, flash_erro, 3, 1);	//传输错误
						break;
					} else {
						flash_rese[1] = readbuff[2];
						flash_rese[2] = readbuff[3];
						I_UART_Transmit(DEBUG_UART, flash_rese, 3, 1);	//传输重传
					}
				} else {
					ErrorCount = 0;
					if(datalen > 256) {
						fy_flash_page_write(startaddr, &readbuff[6], 256);
						fy_flash_page_write(startaddr+256, &readbuff[6+256], datalen-256);
					}else {
						fy_flash_page_write(startaddr, &readbuff[6], datalen);
					}
					startaddr+=datalen;
					flash_succ[1] = readbuff[2];
					flash_succ[2] = readbuff[3];
					I_UART_Transmit(DEBUG_UART, flash_succ, 3, 1);	//传输成功
				}
				//printf("%d\n",startaddr);
			}
			if(startaddr >= filesize){
				flash_succ[1] = 0xff;
				flash_succ[2] = 0xff;
				I_UART_Transmit(DEBUG_UART, flash_succ, 3, 1);	//传输成功
				//printf("update sucess!!!\n");
				break;
			}
		}else{
			HAL_Delay(1);
		}
	}
}

static void processUpgrade_simple(){
	uint32_t updatetimeout = 0;
	uint32_t startaddr = 0;
	uint8_t readbuff[BUFFSIZE];
	DeQueue(&ring,readbuff,4);
	uint32_t filesize = readbuff[0]<<24|readbuff[1]<<16|readbuff[2]<<8|readbuff[3];
	printf("start simpleupdate:%d \n",filesize);

	fy_flash_erase_block(0,filesize);
	//SPI_Flash_ChipEaser();
	printf("format flash sucess\n");

	while(1){
		updatetimeout++;
		if(updatetimeout > 3000){
			printf("update timeout\n");
			break;
		}
		int bytes = QueueCount(&ring);
		if(bytes > 0){
			updatetimeout = 0;
			if(((startaddr + bytes) < filesize)&& (bytes < 256)){
				HAL_Delay(1);
				continue;
			}
			printf("[%d]",bytes);
			if(bytes >= 256){
				DeQueue(&ring,readbuff,256);
				fy_flash_page_write(startaddr, readbuff, 256);
				startaddr+=256;
				printf("-%d\n",startaddr);
			}else{
				//printf(".");
				DeQueue(&ring,readbuff,bytes);
				fy_flash_page_write(startaddr, readbuff, bytes);
				startaddr+=bytes;
			}
			if(startaddr >= filesize){
				printf("update sucess!!!\n");
				break;
			}
		}else{
			HAL_Delay(1);
		}
	}
}

extern __IO uint32_t uwTick;
static void updatePM(uint8_t recivedNew){
	uint16_t rxcrc = 0, calccrc, timeout = 0, bytes;
	while(bytes < 0x3a && timeout++<5)
	{
		bytes = QueueCount(&ring);
		if (bytes < 0x3c)
		{
			HAL_Delay(1);
			//LOG("not enough bytes: %d %d %d\r\n", bytes, timeout, uwTick);
		}
	}
	DeQueue(&ring, device.buff, 0x3a);
	//LOG_HEX(device.buff, 0x30);
	DeQueue(&ring, (uint8_t*)&rxcrc, 2);
	calccrc = Rx_CRC_CCITT(device.buff, 0x3a);
	if (calccrc == rxcrc) recived_new = 1;
	else
	{
		//recived_new = 1;
		LOG("crc wrong: %04x, %04x\r\n", calccrc, rxcrc);
	}
}

static void processCmd(uint16_t cmd,uint16_t cmdlen){
	if(cmd == 0x00){
		processUpgrade();
	}else if(cmd == 0x01){
		processUpgrade_simple();
	}else if(cmd == 0x08){
		updatePM(1);
	}
}
static void procUartUpdateReq(){
    u8 checksum = 0;
	u8 drops = 0;
	u8 readbuff[8];
    while(1){
		int bytes = QueueCount(&ring);
		if(bytes < 1){
			return ;
		}
		checksum = 0;
		DeQueue(&ring,readbuff,1);
		if(readbuff[0] != 0xaa){
			printf("not head0:%x \n",readbuff[0]);
			drops++;
			continue;
		}
		if (drops) LOG("drops %d unmatch head data %d\r\n", drops, uwTick);
		checksum += 0xaa;
		DeQueue(&ring,readbuff,1);
		if(readbuff[0] != 0x55){
			printf("not head1:%x \n",readbuff[0]);
			continue;
		}
		DeQueue(&ring,readbuff,4);
		memcpy(headtab,readbuff,4);
		int cmd = (readbuff[0] << 8) | readbuff[1];
		int len = (readbuff[2] << 8) | readbuff[3];
	//	printf("#");
		//I_UART_Transmit(DEBUG_UART, readbuff, 4, 1);	//传输成功
		
		processCmd(cmd,len);
    }
}

void UART1_IRQHandler(){
		 uint8_t RxData = 0;

	  if ((__HAL_UART_GET_FLAG(&UartHandle, UART_FLAG_RXNE) != RESET) && \
      (__HAL_UART_GET_IT_SOURCE(&UartHandle, UART_IT_RXNE) != RESET))
  {
        RxData = UARTID->DR;// UART_ReceiveData(UART2);
			//	printf("%c",RxData);
				EnQueueSignal(&ring,RxData);
    }
		

		
}


static void initdev(){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_UART1_CLK_ENABLE();

	
	InitQueue(&ring,rxbuff,BUFFSIZE);
	/**USART GPIO Configuration
	PF0     ------> USART1_TX
	PF1     ------> USART1_RX
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_UART1;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Alternate = GPIO_AF9_UART1;

	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	
	/* USART2初始化 */
	UartHandle.Instance = UARTID;
	UartHandle.Init.BaudRate = baud; 
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits = UART_STOPBITS_1;
	UartHandle.Init.Parity = UART_PARITY_NONE;
	HAL_UART_Init(&UartHandle);
//	__HAL_UART_ENABLE(&UartHandle);

	/* ENABLE NVIC */
	#if 1
	
	HAL_NVIC_SetPriority(UART1_IRQn,0,1);
	HAL_NVIC_EnableIRQ(UART1_IRQn );
  
  /* Enable RXNE interrupt */
  __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_RXNE);
	#endif
	
}
static void deinitdev(){

}

static void procMsg(int msgtype,int p1,int p2){
    switch(msgtype){
        case DEVMSG_TIMER10ms:
        	procUartUpdateReq();
			break;
		case DEVMSG_WRITE:
			break;
		case DEVMSG_TIMER50ms:
			break;
		case DEVMSG_TIMER100ms:
			//I_UART_Transmit(DEBUG_UART,"test uart",9,HAL_MAX_DELAY);
			break;
    }
}
const S_DEV dev_uartupgrade={
    initdev,
    deinitdev,
    NULL,
    NULL,
    procMsg
};


#endif 
