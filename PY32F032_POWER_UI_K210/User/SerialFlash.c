#include <main.h>
#include "SerialFlash.h"
#include "context/guicontext.h"
#include "usermain.h"

#define CMD_RDID 0x9F
#define CMD_READ 0x03
#define CMD_WREN 0x06
#define CMD_PP   0x02
#define CMD_RDSR 0x05
#define CMD_SE   0x20//0xD8
#define STATUS_WIP 0x01

#define W25X_ChipErase			            0xC7

#define ENABLE_FLASH_DMA  1


SPI_HandleTypeDef FlashSpiHandle;								//FOR P25QXX
#if ENABLE_FLASH_DMA
DMA_HandleTypeDef HdmaCh2;
DMA_HandleTypeDef HdmaCh3;
#endif
/**
SPI Flash 

PA5 ------> SPI1_SCK
PA6 ------> SPI1_MISO
PA7 ------> SPI1_MOSI
PA4 ------> SPI1_NSS
SPI1
***/

//extern prototypes
void Error_Handler(void);

//private variables
//DMA handle for reading pixels from SPI peripheral
//extern DMA_HandleTypeDef hdma_spi1_rx;
//extern DMA_HandleTypeDef hdma_spi1_tx;

//Status flag. Non-zero when receiving data
static volatile uint8_t isReceivingData = 0;

/* Chip Select macro definition */


#define SPI_FLASHID							SPI1
#define FLASHDMA_SPI1			

#define FLASH_CS_GPIO_PORT                  FLASH_CS_PORT
#define FLASH_CS_PIN1      	  				FLASH_CS_PIN
#define FLASH_CS_LOW()                     {FLASH_CS_GPIO_PORT->BRR = FLASH_CS_PIN1;}//HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_PIN, GPIO_PIN_RESET)
#define FLASH_CS_HIGH()                    {FLASH_CS_GPIO_PORT->BSRR = FLASH_CS_PIN1;}//HAL_GPIO_WritePin(FLASH_CS_GPIO_PORT, FLASH_CS_PIN, GPIO_PIN_SET)

#define SPI_FLASH   						FlashSpiHandle.Instance

#define __SPI_8BITS_DATA_SEND(x)			{*((__IO uint8_t *)&SPI_FLASHID->DR) = x;}  // *((__IO uint8_t*)&LCD_SPI->DR)

static void P25QXX_Gpio_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();    
	
	FLASH_SPI_CLOCK_EN();                    
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;           
	GPIO_InitStruct.Pull = GPIO_PULLUP;                   
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pin = FLASH_CS_PIN;
	HAL_GPIO_Init(FLASH_CS_PORT, &GPIO_InitStruct); 				
	FLASH_CS_HIGH();

	GPIO_InitStruct.Pull 	  = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;

	GPIO_InitStruct.Pin       = FLASH_MOSI_PIN;				//SPI1 MOSI 
	GPIO_InitStruct.Alternate = FLASH_SPI_AF;
	HAL_GPIO_Init(FLASH_MOSI_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin       = FLASH_MISO_PIN;				//SPI1 MISO 
	GPIO_InitStruct.Alternate = FLASH_SPI_AF;
	HAL_GPIO_Init(FLASH_MISO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin       = FLASH_CLK_PIN;				//SPI1 CLK 
	GPIO_InitStruct.Pull 	  = GPIO_PULLDOWN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = FLASH_SPI_AF;
	HAL_GPIO_Init(FLASH_CLK_PORT, &GPIO_InitStruct);

//	printf("init spi Flash gpio \n");
}
/********************************************************************************************************
**函数信息 :
**功能描述 :
**输入参数 :
**输出参数 :
**    备注 :
********************************************************************************************************/
#if ENABLE_FLASH_DMA
static void P25QXX_SPI_DMA_Init(void)
{
	FLASH_SPI_CLOCK_EN();    
	
	FlashSpiHandle.Instance               = FLASH_SPI;                      		  /* SPI1 */
	FlashSpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;  		  /* prescaler :2 */
	FlashSpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;       	  /* full duplex */
	FlashSpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;         		  /* SPI Clock Polarity: low */
	FlashSpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE ;                 /* Data sampling starts at the first clock edge */
	FlashSpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;                /* SPI Data Size is 8 bit */
	FlashSpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;                 /* SPI MSB Transmission */
	FlashSpiHandle.Init.NSS               = SPI_NSS_SOFT;                     /* NSS Software Mode */
	FlashSpiHandle.Init.Mode              = SPI_MODE_MASTER;                  /* Configure as host */         /* SPI Cycle Sample select Disable */
	FlashSpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;               /* TI Mode disable */
	FlashSpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;       /* The CRC check is disabled */
	/* FlashSpiHandle.Init.CRCPolynomial = 1; */                              /* CRC polynomial value */
    HAL_SPI_DeInit(&FlashSpiHandle);
    HAL_SPI_Init(&FlashSpiHandle);
	//======================================================================//
	__HAL_RCC_DMA1_CLK_ENABLE();
	
	HdmaCh2.Instance = DMA1_Channel2;
	HdmaCh2.Init.Direction           = DMA_MEMORY_TO_PERIPH;
	HdmaCh2.Init.PeriphInc           = DMA_PADDR_FIX;
	HdmaCh2.Init.MemInc              = DMA_MADDR_INC;
	HdmaCh2.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	HdmaCh2.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	HdmaCh2.Init.Mode                = DMA_NORMAL;
	HdmaCh2.Init.Priority            = DMA_PRIORITY_0;
	HdmaCh2.Init.ArbMode             = DMA_ARB_POLL;
    HAL_DMA_Init(&HdmaCh2);
    __HAL_LINKDMA(&FlashSpiHandle, hdmatx, HdmaCh2);							// DMA_CH2 关联 SPI1
		
	if(SPI_FLASHID == SPI1){
	 HAL_DMA_ChannelMap(&HdmaCh2, DMA_CHANNEL_MAP_SPI1_TX); 					// SPI1_TX DMA_CH2	
		
	}else{
	HAL_DMA_ChannelMap(&HdmaCh2, DMA_CHANNEL_MAP_SPI2_TX); 					    // SPI2_TX DMA_CH2	
		
	}
	//======================================================================//
	HdmaCh3.Instance = DMA1_Channel3;
	HdmaCh3.Init.Direction           = DMA_PERIPH_TO_MEMORY;
	HdmaCh3.Init.PeriphInc           = DMA_PADDR_FIX;
	HdmaCh3.Init.MemInc              = DMA_MADDR_INC;
	HdmaCh3.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	HdmaCh3.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
	HdmaCh3.Init.Mode                = DMA_NORMAL;
	HdmaCh3.Init.Priority            = DMA_PRIORITY_1;
	HdmaCh3.Init.ArbMode             = DMA_ARB_POLL;
    HAL_DMA_Init(&HdmaCh3);
    __HAL_LINKDMA(&FlashSpiHandle, hdmarx, HdmaCh3);							// DMA_CH3 关联 SPI1
	if(SPI_FLASHID == SPI1){
	HAL_DMA_ChannelMap(&HdmaCh3, DMA_CHANNEL_MAP_SPI1_RX); 					// SPI2_TX DMA_CH2

	}else{
	HAL_DMA_ChannelMap(&HdmaCh3, DMA_CHANNEL_MAP_SPI2_RX); 					// SPI2_RX DMA_CH3

	}
	//======================================================================//
    HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);
    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);	
		
	__HAL_SPI_ENABLE(&FlashSpiHandle);
		
//	FLY_DEBUG("init spi DMA \n");
}
#else
static void P25QXX_SPI_DMA_Init(void)
{
	   	__HAL_RCC_SPI2_CLK_ENABLE();
    FlashSpiHandle.Instance               = SPI2;                      			// SPI2
    FlashSpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; 			// 4分频 48M->12Mhz
    FlashSpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;      			// 全双共
    FlashSpiHandle.Init.CLKPolarity       = SPI_POLARITY_LOW;      				// 空闲时SCK保持低电平
    FlashSpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;          			// 数据采样从第一个时钟边沿开始
    FlashSpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;         			// 数据长度为 8BIT
    FlashSpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;          			// 先发送MSB
    FlashSpiHandle.Init.NSS               = SPI_NSS_SOFT;						// NSS软件模式
    FlashSpiHandle.Init.Mode 			  = SPI_MODE_MASTER;  					// 主机模式
    HAL_SPI_DeInit(&FlashSpiHandle);
    HAL_SPI_Init(&FlashSpiHandle);
	__HAL_SPI_ENABLE(&FlashSpiHandle);
	//=============================
	}
#endif
uint8_t __readwritebyte(uint8_t data){
	
    while((SPI_FLASH->SR & SPI_FLAG_TXE) != SPI_FLAG_TXE)
			;
   __SPI_8BITS_DATA_SEND(data);
    while((SPI_FLASH->SR & SPI_FLAG_RXNE) != SPI_FLAG_RXNE)
			;
    return SPI_FLASH->DR;
}

	static uint32_t	mFlashSize = 0;
uint32_t readDeviceId(void)
{
    FLASH_CS_LOW();

    uint8_t data0 = __readwritebyte(CMD_RDID);
	
    uint8_t data1 =__readwritebyte(0);
    uint8_t data2 =__readwritebyte(0);
    uint8_t data3 =__readwritebyte(0);

    FLASH_CS_HIGH();
	switch(data3) {
		case 0x15: //25q16 //2M
			mFlashSize = 0x200000;
			break;
		case 0x16: //25q32 //4M
			mFlashSize = 0x400000;
			break;
		case 0x17: //25q64 //8M
			mFlashSize = 0x800000;
			break;
		case 0x18: //25q128 //16M
			mFlashSize = 0x1000000;
			break;
		case 0x19: //25q256 //32M
			mFlashSize = 0x2000000;
			break;
		default : 
			break;
	}
    return data1<<16 | data2<<8 | data3;
}

static uint32_t readStatusRegister()
{
    FLASH_CS_LOW();
    *((__IO uint8_t*)&SPI_FLASH->DR) = CMD_RDSR; //0x05, 0x00
    while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SPI_FLASH->DR) = 0;

    /* Wait until the bus is ready before releasing Chip select */
    while(((SPI_FLASH->SR) & SPI_FLAG_BSY) != RESET) { }
    FLASH_CS_HIGH();

    //combine bytes
    uint8_t data0 = *((__IO uint8_t*)&SPI_FLASH->DR);
    uint8_t data1 = *((__IO uint8_t*)&SPI_FLASH->DR);

		
    return data1;
}

static void sendWriteEnable()
{
     FLASH_CS_LOW();
    *((__IO uint8_t*)&SPI_FLASH->DR) = CMD_WREN; //0x06

    while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    /* Wait until the bus is ready before releasing Chip select */
    while(((SPI_FLASH->SR) & SPI_FLAG_BSY) != RESET) { }
		
    FLASH_CS_HIGH();

    //read dummy byte
    uint8_t data0 = *((__IO uint8_t*)&SPI_FLASH->DR);

}

static void sendSectorErase(uint32_t address24)
{
    FLASH_CS_LOW();
	
    __readwritebyte(0xD8);

    //clock out address
		__readwritebyte(address24 >> 16);
		__readwritebyte(address24 >> 8);
		__readwritebyte(address24&0xff);
    FLASH_CS_HIGH();

}

static void cmd_Erase()
{
	 FLASH_CS_LOW();
	__readwritebyte(W25X_ChipErase);
	SPI_FLASH->DR;

    FLASH_CS_HIGH();
}
static void sendPageProgram(uint32_t address24, const uint8_t* data, uint32_t length)
{
    FLASH_CS_LOW();
    *((__IO uint8_t*)&SPI_FLASH->DR) = CMD_PP;

    //clock out address
    while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SPI_FLASH->DR) = (address24 >> 16) & 0xFF;

    while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SPI_FLASH->DR) = (address24 >> 8) & 0xFF;

    while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SPI_FLASH->DR) = address24 & 0xFF;

    //now write data bytes
    while(length > 0)
    {
        while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
        *((__IO uint8_t*)&SPI_FLASH->DR) = *data;
        data++;
        length--;
    }

    /* Wait until the bus is ready before releasing Chip select */
    while(((SPI_FLASH->SR) & SPI_FLAG_BSY) != RESET) { }

    FLASH_CS_HIGH();

    //read dummy bytes
    while(((SPI_FLASH->SR) & SPI_FLAG_RXNE) == SPI_FLAG_RXNE)
    {
        *((__IO uint8_t*)&SPI_FLASH->DR);
    }
}
static uint8_t dummy;

#if ENABLE_FLASH_DMA
#else
#pragma O2// for most speed PY040 16ms-11ms
#endif

void readData(uint32_t address24, uint8_t* buffer, uint32_t length)
{
FLASH_CS_LOW();
//	if(length > 512){
//		FLY_DEBUG("read data size eror:%d \n",length);
//	}
    *((__IO uint8_t*)&SPI_FLASH->DR) = CMD_READ;

    //clock out address
    *((__IO uint8_t*)&SPI_FLASH->DR) = (address24 >> 16) & 0xFF;

    while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SPI_FLASH->DR) = (address24 >> 8) & 0xFF;

    while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SPI_FLASH->DR) = address24 & 0xFF;

    switch (length)
    {
        default:
            while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
						dummy =	*((__IO uint8_t*)&SPI_FLASH->DR);
            *((__IO uint8_t*)&SPI_FLASH->DR) = 0 ;
        case 3:
            while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
            dummy =	*((__IO uint8_t*)&SPI_FLASH->DR);
            *((__IO uint8_t*)&SPI_FLASH->DR) = 0;
        case 2:
            while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
						dummy =	*((__IO uint8_t*)&SPI_FLASH->DR);
            *((__IO uint8_t*)&SPI_FLASH->DR) = 0;
        case 1:
            while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
            dummy =	*((__IO uint8_t*)&SPI_FLASH->DR);
            *((__IO uint8_t*)&SPI_FLASH->DR) = 0;
        case 0:
            break;
    }
    switch (length)
    {
        case 1:
        dummy =	*((__IO uint8_t*)&SPI_FLASH->DR);
        case 2:
        dummy =	*((__IO uint8_t*)&SPI_FLASH->DR);
        case 3:
        dummy =	*((__IO uint8_t*)&SPI_FLASH->DR);
        default:
            break;
    }

    uint8_t* const buf_end = buffer + length - 4;
    while ((buf_end - buffer) > 3)
    {
        while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE)
					;
        *buffer++ = *((__IO uint8_t*)&SPI_FLASH->DR);
				*((__IO uint8_t*)&SPI_FLASH->DR) = 0;
				while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE)
					;
							*buffer++ = *((__IO uint8_t*)&SPI_FLASH->DR);
				*((__IO uint8_t*)&SPI_FLASH->DR) = 0;
				while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE)
					;
							*buffer++ = *((__IO uint8_t*)&SPI_FLASH->DR);
				*((__IO uint8_t*)&SPI_FLASH->DR) = 0;
				while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE)
					;
							*buffer++ = *((__IO uint8_t*)&SPI_FLASH->DR);
				*((__IO uint8_t*)&SPI_FLASH->DR) = 0;
    }

    while (buffer < buf_end)
    {
        while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
        *buffer++ = *((__IO uint8_t*)&SPI_FLASH->DR);
	*((__IO uint8_t*)&SPI_FLASH->DR) = 0;
    }

    /* Wait until the bus is ready before releasing Chip select */
    while(((SPI_FLASH->SR) & SPI_FLAG_BSY) != RESET) { }

    const int rest = length < 4 ? length : 4;
    for (int i = 0; i<rest; i++)
    {
        *buffer++ = *((__IO uint8_t*)&SPI_FLASH->DR);
    }
	isReceivingData = 0;
    FLASH_CS_HIGH();
}
//void P25QXX_Send_CMD(uint8_t instruction, uint8_t *pRxData, uint32_t address, uint32_t addressSize, uint8_t csflags)
//{
//	uint8_t TempBuff[16];
//	uint16_t transfurlength;
//	// 拉低cs
//	FLASH_CS_L;
//	TempBuff[0] = instruction;
//	if (addressSize == 0) // 不发送地址 9f 06 使能
//	{
//		transfurlength = 1;
//	}
//	if (addressSize == SPI_8BYT_ADD)
//	{
//		TempBuff[1] = address;
//		transfurlength = 2;
//	}
//	else if (addressSize == SPI_16BYT_ADD)
//	{
//		TempBuff[1] = address >> 8;
//		TempBuff[2] = address;
//		transfurlength = 3;
//	}
//	else if (addressSize == SPI_24BYT_ADD)
//	{
//		TempBuff[1] = address >> 16;
//		TempBuff[2] = address >> 8;
//		TempBuff[3] = address;
//		transfurlength = 4;
//	}

//	/*SPI Polling方式传输*/
//	HAL_SPI_TransmitReceive_DMA(&FlashSpiHandle, (uint8_t *)TempBuff, (uint8_t *)pRxData, transfurlength);
//	while (FlashSpiHandle.State != HAL_SPI_STATE_READY){} /* 等待传输结束 */

//	if (csflags == 0)
//	{
//		FLASH_CS_H;
//	}
//}
#if ENABLE_FLASH_DMA
//static uint8_t* reqreadbuff;
//static uint8_t DmaTempBuff[512+1];
//static uint16_t dmaReadLen;
static uint8_t dumydata=0xff;
#pragma O2// for most speed PY040 16ms-11ms
void readDataDMA(uint32_t address24, uint8_t* buffer, uint32_t length)
{
	
     // Pull Flash CS pin low
   isReceivingData = 1;

    FLASH_CS_LOW();
#if 1
      *((__IO uint8_t*)&SPI_FLASH->DR) = CMD_READ;
    //clock out address
    while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SPI_FLASH->DR) = (address24 >> 16) & 0xFF;

    while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SPI_FLASH->DR) = (address24 >> 8) & 0xFF;

    while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    *((__IO uint8_t*)&SPI_FLASH->DR) = address24 & 0xFF;

 
    /* Wait until the bus is ready before releasing Chip select */
//    while(((SPI_FLASH->SR) & SPI_FLAG_BSY) != RESET) { }

    while(((SPI_FLASH->SR) & SPI_FLAG_TXE) != SPI_FLAG_TXE);
    while((SPI_FLASH->SR & SPI_FLAG_BSY) != RESET) { }
		
		while(((SPI_FLASH->SR) & SPI_FLAG_RXNE) == SPI_FLAG_RXNE){
			
			dummy =  *((__IO uint8_t*)&SPI_FLASH->DR);
		}
//		*((__IO uint8_t*)&SPI_FLASH->DR);
//		*((__IO uint8_t*)&SPI_FLASH->DR);
//		*((__IO uint8_t*)&SPI_FLASH->DR);
//		*((__IO uint8_t*)&SPI_FLASH->DR);
#else
	uint8_t DummyBuff[16];
		
		 P25QXX_Send_CMD(P25X_ReadData,DummyBuff, address24, SPI_24BYT_ADD, 1);

//    if (HAL_SPI_Receive_DMA(&FlashSpiHandle,(uint8_t *)buffer, length) != HAL_OK)
//    {
//        APP_ErrorHandler();
//    }
//    while (FlashSpiHandle.State != HAL_SPI_STATE_READY) {} /* 等待传输结束 */
//		isReceivingData = 0;
//        FLASH_CS_HIGH();
#endif
		#if 1
		__disable_irq();
		
//    __HAL_SPI_DISABLE(&FlashSpiHandle);
	
		CLEAR_BIT(SPI_FLASH->CR2, SPI_CR2_TXDMAEN);
		CLEAR_BIT(SPI_FLASH->CR2, SPI_CR2_RXDMAEN);
    /* Reset the threshold bit */
    //CLEAR_BIT(FlashSpiHandle.Instance->CR2, SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN);

		HdmaCh2.DmaBaseAddress->IFCR = (DMA_ISR_TCIF2);
		HdmaCh3.DmaBaseAddress->IFCR = (DMA_ISR_TCIF3);	
	
    /******** TX ****************/
    /* Disable the peripheral */
    __HAL_DMA_DISABLE(&HdmaCh2);
 /* Disable the peripheral */
    __HAL_DMA_DISABLE(&HdmaCh3);	

    /* Clear all flags */
//    __HAL_DMA_CLEAR_FLAG(&HdmaCh2, (DMA_ISR_TCIF2 << (HdmaCh2.ChannelIndex)));

    /* Configure DMA Channel data length */
    HdmaCh2.Instance->CNDTR = length;

    /* Configure DMA Channel destination address */
    HdmaCh2.Instance->CPAR = (uint32_t)&(SPI_FLASH->DR);

    /* Configure DMA Channel source address */
    HdmaCh2.Instance->CMAR = (uint32_t)&dumydata;

//    /* Enable the Peripheral */
//    __HAL_DMA_ENABLE(&HdmaCh2);

    /* Enable Tx DMA Request */
    SET_BIT(FlashSpiHandle.Instance->CR2, SPI_CR2_TXDMAEN);
	
    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(&HdmaCh2);	
		
 /******** RX ****************/
//    /* Disable the peripheral */
//    __HAL_DMA_DISABLE(&HdmaCh3);

    /* Clear all flags */
//    __HAL_DMA_CLEAR_FLAG(&HdmaCh3, (DMA_ISR_TCIF3 << (HdmaCh3.ChannelIndex )));

    /* Configure DMA Channel data length */
    HdmaCh3.Instance->CNDTR = length;

    /* Configure DMA Channel destination address */
    HdmaCh3.Instance->CPAR = (uint32_t)&(SPI_FLASH->DR);

    /* Configure DMA Channel source address */
    HdmaCh3.Instance->CMAR = (uint32_t)buffer;

    /* Enable Rx DMA Request */
    SET_BIT(FlashSpiHandle.Instance->CR2, SPI_CR2_RXDMAEN);


    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(&HdmaCh3);

  __HAL_DMA_DISABLE_IT(&HdmaCh3, DMA_IT_HBT | DMA_IT_BTC | DMA_IT_TE);
  __HAL_DMA_ENABLE_IT(&HdmaCh3, (DMA_IT_TC));
		
//    /* Enable SPI peripheral */
//    __HAL_SPI_ENABLE(&FlashSpiHandle);
	
	__enable_irq();
	//	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,1);
	#endif
	
}
// for most speed PY040 16ms-11ms

#pragma O2// for most speed PY040 16ms-11ms
void DataReader_DMACallback()
{
//		printf("==>dma  end:%x \n",DMA1->ISR);
//	
//	return ;
   /* Transfer Complete Interrupt management ***********************************/
  //  if ((0U != (DMA1->ISR & (DMA_FLAG_GL2)))/* && (0U != (HdmaCh3.Instance->CCR & DMA_IT_TC))*/)
   // {
	
			uint32_t flag_it = DMA1->ISR;
		uint32_t source_it = HdmaCh3.Instance->CCR;

	//			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,0);
  /* Half Transfer Complete Interrupt management ******************************/
//		if ((0U != (DMA1->ISR & (DMA_FLAG_TC1 << (HdmaCh3.ChannelIndex)))) && (0U != (HdmaCh3.Instance->CCR & DMA_IT_TC)))
	if ((0U != (DMA1->ISR & (DMA_FLAG_TC3))))
    {
		 FLASH_CS_HIGH();	
				//memcpy(reqreadbuff,&DmaTempBuff[1],dmaReadLen);
        /* Disable the transfer complete and error interrupt */
        __HAL_DMA_DISABLE_IT(&HdmaCh3, DMA_IT_TE | DMA_IT_TC);
        /* Clear the transfer complete flag */
    __HAL_DMA_CLEAR_FLAG(&HdmaCh3, __HAL_DMA_GET_TC_FLAG_INDEX(&HdmaCh3));
				 CLEAR_BIT(FlashSpiHandle.Instance->CR2, SPI_CR2_RXDMAEN);
				 CLEAR_BIT(FlashSpiHandle.Instance->CR2, SPI_CR2_TXDMAEN);
				
				
        // Wait until the bus is not busy before changing configuration
        // SPI is busy in communication or Tx buffer is not empty
        //while(((SPI_FLASH->SR) & SPI_FLAG_BSY) != RESET) { }
				//Spi1Handle.State = HAL_SPI_STATE_READY;
       //FLASH_CS_GPIO_Port->BSRR = FLASH_CS_Pin;
//        FLASH_CS_HIGH();
        isReceivingData = 0;
    } 
	//	else if (( RESET != (flag_it & (DMA_FLAG_TE1 << HdmaCh3.ChannelIndex))) && (RESET != (source_it & DMA_IT_TE)))
//		{
//			/* When a DMA transfer error occurs */
//			/* A hardware clear of its EN bits is performed */
//			/* Disable ALL DMA IT */
//			__HAL_DMA_DISABLE_IT(&HdmaCh3, (DMA_IT_TC | DMA_IT_HBT | DMA_IT_BTC | DMA_IT_TE));

//			/* Clear all flags */
//			HdmaCh3.DmaBaseAddress->IFCR = (DMA_ISR_TCIF3 <<HdmaCh3.ChannelIndex);

//		}
	//	DMA1->IFCR = 0xff0; //clear dma2,1

}
#else
void DataReader_DMACallback(){

}
#endif


//void SerialFlash_ReadData(uint32_t address24, uint8_t* buffer, uint32_t length)
//{
//    readData(address24, buffer, length);
//}

//void SerialFlash_StartDMAReadData(uint32_t address24, uint8_t* buffer, uint32_t length)
//{
//   // readDataDMA(address24, buffer, length);
//    readData(address24, buffer, length);
//}

volatile uint32_t statusReg = 0;

void SerialFlash_ChipErase(){
	uint32_t address = 0x000000;
	sendWriteEnable();
	cmd_Erase();
	uint32_t status;
	do
	{
		status = readStatusRegister();
	} while (status & STATUS_WIP);
}
void SerialFlash_EraseSize(uint32_t startaddr,uint32_t size){
	uint32_t address = startaddr;
	uint32_t sizeOfBitmaps = address+size;
	do
	{
		sendWriteEnable();
		sendSectorErase(address);

		//wait until WIP is cleared
		uint32_t status;
		do
		{
			status = readStatusRegister();
		} while (status & STATUS_WIP);

		address += 0x8000;
		if(address >= (mFlashSize-0x8000)){
//			FLY_DEBUG("protect area.. \n");
			break;
		}
	} while (address < sizeOfBitmaps);
}
void SerialFlash_ProgramInfo(uint32_t addr,uint8_t* data,uint32_t size){
	sendWriteEnable();
	sendPageProgram(addr, data, size);
	//wait until WIP is cleared
	uint32_t status;
	do
	{
		status = readStatusRegister();
	} while (status & STATUS_WIP);
}


uint32_t SerialFlash_IsReceivingData(void)
{
	
	//isReceivingData = (Spi1Handle.State != HAL_SPI_STATE_READY);
  return isReceivingData;
}

void SerialFlash_WaitForReceiveDone(void)
{
//	 while (Spi1Handle.State != HAL_SPI_STATE_READY)
////  {}
   while (isReceivingData);
//	
//while (Spi1Handle.State != HAL_SPI_STATE_READY) {} /* 等待传输结束 */
//		FLASH_CS_HIGH();
}

void fly_flash_init(void){
	//__HAL_SPI_ENABLE(&Spi1Handle);
	//SerialFlash_DMAInit();
	P25QXX_Gpio_Init();
	P25QXX_SPI_DMA_Init();
	#if ENABLE_FLASH_DMA
	#else
	printf("Disable Flash DMA \n");
	#endif
//    __HAL_SPI_ENABLE(&FlashSpiHandle);
//	FLY_DEBUG("spi Flash init end \n");
}
uint32_t fy_flash_readid(void)
{
	return readDeviceId();
}
void fy_flash_read(uint32_t address24, uint8_t* buffer, uint32_t length)
{
	
//	#if ENABLE_FLASH_DMA
//	if(length < 32){
//		
//		readData(address24,buffer,length);
//	}else{
//		reqreadbuff  = buffer;
//		dmaReadLen = length;
//		if(dmaReadLen > sizeof(DmaTempBuff)){
//			printf("Flash DMA Read too large:%d\n",length);
//			readData(address24,buffer,length);
//			return ;
//		}
//		
//		//			memcpy(reqreadbuff,&DmaTempBuff[1],dmaReadLen);
//		readDataDMA(address24,DmaTempBuff,length+1);
//		SerialFlash_WaitForReceiveDone();
//	}
//	#else
		readData(address24,buffer,length);
//	#endif
}

void fy_flash_readdma(uint32_t address24, uint8_t* buffer, uint32_t length)
{
//	readData(address24,buffer,length);
	readDataDMA(address24,buffer,length);
}

void fy_flash_erase(){
	SerialFlash_ChipErase();
	//spi_flash_bulk_erase();
}
void fy_flash_erase_block(uint32_t start,uint32_t sizeOfBitmaps){
  // spi_flash_sector_erase_size(start,sizeOfBitmaps);
	SerialFlash_EraseSize(start,sizeOfBitmaps);
}
void fy_flash_page_write(uint32_t addr,uint8_t* data,uint32_t size){
	//spi_flash_page_write(data,addr,size);
	SerialFlash_ProgramInfo(addr,data,size);
}

uint32_t fy_flash_isreceiveing(void)
{
//  return isReceivingData;
	return SerialFlash_IsReceivingData();
}

void fy_flash_waitreceive_done(void)
{
	SerialFlash_WaitForReceiveDone();
}

void fy_flash_dmainit(void)
{
  
}


const flythings_flash	m_flash ={
  .flash_init = fly_flash_init,
  .flash_deinit = fy_flash_dmainit,
  .flash_readdata = fy_flash_read,
  .flash_readdata_dma = fy_flash_readdma,
  .flash_wait_dma_end = fy_flash_waitreceive_done, 
  .flash_isreading = fy_flash_isreceiveing
};

