/*
 * SDL_video.c
 *
 *  Created on: Nov 29, 2020
 *      Author: ZKSWE Develop Team
 */
//#include "py32f0xx.h"                   // Device header
#include "main.h"
#include "video.h"
#include "graphics/gfx.h"
#include "usermain.h"
#if 1


// 打开屏幕DMA控制，
//#define ENABLE_LCD_DMA	

#ifdef ENABLE_LCD_DMA
DMA_HandleTypeDef HdmaCh1;
#define SpiLcdHandle Spi2Handle
#endif



SPI_HandleTypeDef Spi2Handle; 								// FOR LCD


/********************************************************************************
Description:    LCD IO口配置
Input:
Output:
Return:
*********************************************************************************/
static void LcdGPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStruct={0};

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = LCD_BL_PIN; 				//BL  
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(LCD_BL_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = LCD_PW_PIN; 				//POWER  
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(LCD_PW_PORT, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pin = LCD_CMD_PIN; 				//DC  
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(LCD_CMD_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStruct.Pin = LCD_RST_PIN;				//RES 
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(LCD_RST_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pull = GPIO_PULLUP;			
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pin = LCD_CS_PIN;				//NSS  
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	HAL_GPIO_Init(LCD_CS_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pin = LCD_CLK_PIN; 				//SCK  
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = LCD_SPI_AF;
	HAL_GPIO_Init(LCD_CLK_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pin = LCD_DAT_PIN;				//MOSI 
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = LCD_SPI_AF;
	HAL_GPIO_Init(LCD_DAT_PORT, &GPIO_InitStruct);	
	
}

static void LCD_SPI_DMA_Init(void)
{
	
	LCD_SPI_CLOCK_EN();

	Spi2Handle.Instance = LCD_SPI;									 // SPI2
	Spi2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; // 4分频 48M->12M
	Spi2Handle.Init.Direction = SPI_DIRECTION_2LINES;			 // 全双共
	Spi2Handle.Init.CLKPolarity = SPI_POLARITY_LOW;				 // 空闲时SCK保持低电平
	Spi2Handle.Init.CLKPhase = SPI_PHASE_1EDGE;					 // 数据采样从第一个时钟边沿开始
	Spi2Handle.Init.DataSize = SPI_DATASIZE_8BIT;				 // 数据长度为 8BIT
	Spi2Handle.Init.FirstBit = SPI_FIRSTBIT_MSB;				 // 先发送MSB
	Spi2Handle.Init.NSS = SPI_NSS_SOFT;							 // 硬件控制NSS(使能SPI后则自动拉低,需要关闭SPE(SPI)才能恢复高) SPI_NSS_SOFT;       							// NSS软件模式(使能软件从设备管理)
	Spi2Handle.Init.Mode = SPI_MODE_MASTER;						 // 主机模式
	Spi2Handle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; // 关闭CRC发送
	HAL_SPI_DeInit(&Spi2Handle);
	HAL_SPI_Init(&Spi2Handle);
	
	__HAL_SPI_ENABLE(&Spi2Handle);

	#ifdef ENABLE_LCD_DMA
	
	//======================================================================//
	__HAL_RCC_DMA1_CLK_ENABLE();
	HdmaCh1.Instance 					= DMA1_Channel1;					// DMA_CH2
	HdmaCh1.Init.Direction  			= DMA_MEMORY_TO_PERIPH;				// 内存往外设方向
	HdmaCh1.Init.PeriphInc  			= DMA_PADDR_FIX;					// 外设增量模式 关闭
	HdmaCh1.Init.MemInc     			= DMA_MADDR_INC;					// 内存增量模式 开启
	HdmaCh1.Init.PeriphDataAlignment 	= DMA_PDATAALIGN_HALFWORD;				// 外设数据格式: BYTE
	HdmaCh1.Init.MemDataAlignment    	= DMA_MDATAALIGN_HALFWORD;				// 内存数据格式: BYTE
	HdmaCh1.Init.Mode                	= DMA_NORMAL;						// 禁止循环模式
	HdmaCh1.Init.Priority            	= DMA_PRIORITY_1;				// 优先级: 中
	HAL_DMA_Init(&HdmaCh1);
	__HAL_LINKDMA(&SpiLcdHandle, hdmatx, HdmaCh1);							// DMA_CH2 关联 SPI1


	//HAL_DMA_ChannelMap(&HdmaCh1, DMA_CHANNEL_MAP_SPI1_WR); 					// SPI1_TX DMA_CH2

	if(LCD_SPI == SPI1){
	 HAL_DMA_ChannelMap(&HdmaCh1, DMA_CHANNEL_MAP_SPI1_TX); 					// SPI1_TX DMA_CH2	
		
	}else{
	HAL_DMA_ChannelMap(&HdmaCh1, DMA_CHANNEL_MAP_SPI2_TX); 					    // SPI2_TX DMA_CH2	
		
	}

	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
		#endif
}
#ifdef ENABLE_LCD_DMA
volatile uint8_t lcdDMABusy = 0;
void LcdDMASend(const uint16_t* data,int len){
    /* Reset the threshold bit */
 //   CLEAR_BIT(Spi1Handle.Instance->CR2, SPI_CR2_LDMATX | SPI_CR2_LDMARX);

		lcdDMABusy = 1;
    /* Set RX Fifo threshold according the reception data length: 8bit */
    SET_BIT(SPI1->CR2, SPI_CR2_CLRTXFIFO);

		
    __HAL_SPI_DISABLE(&SpiLcdHandle);

    /******** TX ****************/
    /* Disable the peripheral */
    __HAL_DMA_DISABLE(&HdmaCh1);

    /* Clear all flags */
    //__HAL_DMA_CLEAR_FLAG(&HdmaCh1, (DMA_ISR_GIF1 << (HdmaCh1.ChannelIndex)));

    /* Configure DMA Channel data length */
    HdmaCh1.Instance->CNDTR = len;

    /* Configure DMA Channel destination address */
    HdmaCh1.Instance->CPAR = (uint32_t)&(SPI1->DR);

    /* Configure DMA Channel source address */
    HdmaCh1.Instance->CMAR = (uint32_t)data;

    /* Enable the Peripheral */
    __HAL_DMA_ENABLE(&HdmaCh1);

    /* Enable Tx DMA Request */
    SET_BIT(SpiLcdHandle.Instance->CR2, SPI_CR2_TXDMAEN);

      __HAL_DMA_DISABLE_IT(&HdmaCh1, DMA_IT_HBT | DMA_IT_TE);
      __HAL_DMA_ENABLE_IT(&HdmaCh1, (DMA_IT_TC));
			
    /* Enable SPI peripheral */
    __HAL_SPI_ENABLE(&SpiLcdHandle);
	
	//	printf("dma send start\n");
		
}
void LCD_DMACallback(){
	uint32_t flag_it = DMA1->ISR;
		uint32_t source_it = HdmaCh1.Instance->CCR;

	//			HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,0);
  /* Half Transfer Complete Interrupt management ******************************/
		if ((0U != (DMA1->ISR & (DMA_FLAG_TC1 << (HdmaCh1.ChannelIndex)))) && (0U != (HdmaCh1.Instance->CCR & DMA_IT_TC)))
    {
        /* Disable the transfer complete and error interrupt */
        __HAL_DMA_DISABLE_IT(&HdmaCh1, DMA_IT_TE | DMA_IT_TC);
        /* Clear the transfer complete flag */
				__HAL_DMA_CLEAR_FLAG(&HdmaCh3, __HAL_DMA_GET_TC_FLAG_INDEX(&HdmaCh1));
				 CLEAR_BIT(SpiLcdHandle.Instance->CR2, SPI_CR2_TXDMAEN);
        lcdDMABusy = 0;
		//printf("dma send end1\n");
    } else if (( RESET != (flag_it & (DMA_FLAG_TE1 << HdmaCh1.ChannelIndex))) && (RESET != (source_it & DMA_IT_TE)))
		{
			/* When a DMA transfer error occurs */
			/* A hardware clear of its EN bits is performed */
			/* Disable ALL DMA IT */
			__HAL_DMA_DISABLE_IT(&HdmaCh1, (DMA_IT_TC | DMA_IT_HBT | DMA_IT_TE));

			/* Clear all flags */
			//HdmaCh1.DmaBaseAddress->IFCR = (DMA_ISR_GIF1 <<HdmaCh1.ChannelIndex);
        lcdDMABusy = 0;

	//	printf("dma send end2\n");
		}
}
void waitDmaSendEnd(){
	while(lcdDMABusy);
}
#else
// 中断服务程序调用
void LCD_DMACallback(){

}
#endif

void LCD_WR_REG(uint8_t CMD)
{
	CLR_CS();
	SET_CMD();
	TFT_SPI_Write_Byte(CMD);
	SET_CS();
}

void LCD_WR_DATA(uint8_t DATA)
{
	CLR_CS();
	SET_DATA();
	TFT_SPI_Write_Byte(DATA);
	SET_CS();
}
void LCD_WR_DATA16(uint16_t DATA)
{
	SET_DATA();
	CLR_CS();

	TFT_SPI_Write_Byte(DATA>>8);
	TFT_SPI_Write_Byte(DATA&0xff);
	SET_CS();
}

#define delay_ms 				HAL_Delay
#define WriteData				LCD_WR_DATA
#define WriteComm 				LCD_WR_REG
#define TFT_WR_DATA8			LCD_WR_DATA
#define TFT_WR_REG 				LCD_WR_REG

#define LCD_wramcmd 0X2C
#define LCD_setxcmd 0X2A
#define LCD_setycmd 0X2B
void LCD_Clear(uint16_t color)
{
	LCD_Fill(0,0,LCD_WIDTH,LCD_HEIGHT,color);
}  

uint8_t direct = 0;

bool video_init() {
	
	LcdGPIOInit();
	LCD_SPI_DMA_Init();
	
	SET_CS();
	LED_OFF();
	LCD_ON();
	RST_LOW();
	HAL_Delay(20);
	RST_HI();
	HAL_Delay(20);
 
	TFT_WR_REG(0x11);   //Sleep out         
	HAL_Delay(120);      //Delay 120ms 

	video_rotate(direct);

	// 设置列地址（240x240 为例）
	LCD_WR_REG(0x2A);
	TFT_WR_DATA8(0x00);
	TFT_WR_DATA8(0x00);
	TFT_WR_DATA8((LCD_WIDTH - 1)>>8);
	TFT_WR_DATA8((LCD_WIDTH - 1)&0xff);

	// 设置行地址（240x240 为例）
	LCD_WR_REG(0X2B);
	TFT_WR_DATA8(0x00);
	TFT_WR_DATA8(0x00);
	TFT_WR_DATA8((LCD_HEIGHT - 1)>>8);
	TFT_WR_DATA8((LCD_HEIGHT - 1)&0xff);

	TFT_WR_REG(0x3A);     
	TFT_WR_DATA8(0x05);   

	TFT_WR_REG(0xB2);     
	TFT_WR_DATA8(0x1F);   
	TFT_WR_DATA8(0x1F);   
	TFT_WR_DATA8(0x00);   
	TFT_WR_DATA8(0x33);   
	TFT_WR_DATA8(0x33);  

	TFT_WR_REG(0xB7);    
	TFT_WR_DATA8(0x00);  //VGH=12.2V,VGL=-7.16V 

	TFT_WR_REG(0xBB);     
	TFT_WR_DATA8(0x3F);  

	TFT_WR_REG(0xC0);               
	TFT_WR_DATA8(0x2C); 

	TFT_WR_REG(0xC2);     
	TFT_WR_DATA8(0x01);   

	TFT_WR_REG(0xC3);     
	TFT_WR_DATA8(0x0d); //4.3V 

	TFT_WR_REG(0xC4);     
	TFT_WR_DATA8(0x20);   //VDV, 0x20:0v

	TFT_WR_REG(0xC6);     
	TFT_WR_DATA8(0x13);     

	TFT_WR_REG(0xD0);     
	TFT_WR_DATA8(0xA4); 
	TFT_WR_DATA8(0xA1); 
	
	TFT_WR_REG(0xE0);
	TFT_WR_DATA8(0xF0);
	TFT_WR_DATA8(0x06);
	TFT_WR_DATA8(0x0D);
	TFT_WR_DATA8(0x0B);
	TFT_WR_DATA8(0x0A);
	TFT_WR_DATA8(0x07);
	TFT_WR_DATA8(0x2E);
	TFT_WR_DATA8(0x43);
	TFT_WR_DATA8(0x45);
	TFT_WR_DATA8(0x38);
	TFT_WR_DATA8(0x14);
	TFT_WR_DATA8(0x13);
	TFT_WR_DATA8(0x25);
	TFT_WR_DATA8(0x29);

	TFT_WR_REG(0xE1);
	TFT_WR_DATA8(0xF0);
	TFT_WR_DATA8(0x07);
	TFT_WR_DATA8(0x0A);
	TFT_WR_DATA8(0x08);
	TFT_WR_DATA8(0x07);
	TFT_WR_DATA8(0x23);
	TFT_WR_DATA8(0x2E);
	TFT_WR_DATA8(0x33);
	TFT_WR_DATA8(0x44);
	TFT_WR_DATA8(0x3A);
	TFT_WR_DATA8(0x16);
	TFT_WR_DATA8(0x17);
	TFT_WR_DATA8(0x26);
	TFT_WR_DATA8(0x2C);  

	TFT_WR_REG(0x21);     //Display on

	LCD_Clear(0x00);
	HAL_Delay(50);
	TFT_WR_REG(0x29);     //Display on
	LED_ON();		//开背光	
	return true;
}

void video_rotate(uint8_t dir)
{
	const uint8_t r[4] = {LCD_COLOR_MODE ,ST77xx_MADCTL_MX|ST77xx_MADCTL_MV|LCD_COLOR_MODE, ST77xx_MADCTL_MX|ST77xx_MADCTL_MY|LCD_COLOR_MODE, ST77xx_MADCTL_MY|ST77xx_MADCTL_MV|LCD_COLOR_MODE};
	TFT_WR_REG(0x36);
	direct = dir;
	TFT_WR_DATA8(r[dir]);
}

void video_deinit() {
	LED_OFF();
	LCD_OFF();
}

void LCD_Backlight(bool on) {
	if (on) LED_ON();
	else LED_OFF();
}

void video_flip() {
	
}
#endif