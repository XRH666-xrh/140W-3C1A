#ifndef _USERMAIN_H_
#define _USERMAIN_H_

#include "main.h"

//printf 
#ifndef USER_PRINTF
#define USER_PRINTF   0      //需要打印功能开启
#endif

#define LCD_M133 0
#define LCD_M154 1
#define LCD_M085 2
#define LCD_M089 3
#define LCD_M072 4

#define LCD_MODEL LCD_M133

#define ST77xx_MADCTL_MY  0x80
#define ST77xx_MADCTL_MX  0x40
#define ST77xx_MADCTL_MV  0x20
#define ST77xx_MADCTL_ML  0x10
#define ST77xx_MADCTL_RGB 0x00
#define ST77xx_MADCTL_BGR 0x08
#define ST77xx_MADCTL_MH  0x04

#define ST7789 0
#define ST7735 1

#if (LCD_MODEL == LCD_M133 || LCD_MODEL == LCD_M154)
#define LCD_IC ST7789
#define LCD_WIDTH 240
#define LCD_HEIGHT  240
#define LCD_Y_OFFSET 0
#define LCD_X_OFFSET 0
#define LCD_COLOR_MODE ST77xx_MADCTL_RGB
#define LCD_DIRECTION 0 // 0: 0deg, 1:90deg, 2:180deg, 3: 270deg
#elif (LCD_MODEL == LCD_M085)
#define LCD_IC ST7735
#define LCD_WIDTH 128
#define LCD_HEIGHT  128
#define LCD_Y_OFFSET 3
#define LCD_X_OFFSET 2
#define LCD_COLOR_MODE ST77xx_MADCTL_BGR
#define LCD_DIRECTION  ST77xx_MADCTL_MX | ST77xx_MADCTL_MY
#elif (LCD_MODEL == LCD_M096)
#define LCD_IC ST7735
#define LCD_WIDTH 160
#define LCD_HEIGHT  80
#define LCD_Y_OFFSET 0
#define LCD_X_OFFSET 0
#define USE_HORIZONTAL 0 // 0: 0deg, 1:90deg, 2:180deg, 3: 270deg
#elif (LCD_MODEL == LCD_M072)
#define LCD_IC ST7735
#define LCD_WIDTH 160
#define LCD_HEIGHT  60
#define LCD_Y_OFFSET 0
#define LCD_X_OFFSET 0
#define USE_HORIZONTAL 0 // 0: 0deg, 1:90deg, 2:180deg, 3: 270deg
#endif


//LCD IO Define
/***
RESET:    PA12
DC		  PA11
CS		  PA15
BL  	  PA9
SPI:      SPI2
CLK:      PA0
MOSI/DAT: PA10
****/

#define LCD_SPI_CLOCK_EN()    __HAL_RCC_SPI2_CLK_ENABLE()                   

#define LCD_RST_PORT		GPIOA
#define LCD_RST_PIN			GPIO_PIN_12

#define LCD_CMD_PORT		GPIOA
#define LCD_CMD_PIN			GPIO_PIN_11

#define LCD_CS_PORT			GPIOA
#define LCD_CS_PIN			GPIO_PIN_15

#define LCD_BL_PORT			GPIOA
#define LCD_BL_PIN			GPIO_PIN_9

#define LCD_PW_PORT			GPIOA
#define LCD_PW_PIN			GPIO_PIN_8

#define LCD_CLK_PORT		GPIOA
#define LCD_CLK_PIN			GPIO_PIN_0

#define LCD_DAT_PORT		GPIOA
#define LCD_DAT_PIN			GPIO_PIN_10

#define LCD_SPI				SPI2

#define LCD_SPI_AF          GPIO_AF0_SPI2


#define RST_LOW()	HAL_GPIO_WritePin(LCD_RST_PORT,LCD_RST_PIN,GPIO_PIN_RESET)
#define RST_HI()	HAL_GPIO_WritePin(LCD_RST_PORT,LCD_RST_PIN,GPIO_PIN_SET)


#define LCD_OFF() 	HAL_GPIO_WritePin(LCD_PW_PORT,LCD_PW_PIN,GPIO_PIN_SET)
#define LCD_ON() 	HAL_GPIO_WritePin(LCD_PW_PORT,LCD_PW_PIN,GPIO_PIN_RESET)
#define LED_OFF() HAL_GPIO_WritePin(LCD_BL_PORT,LCD_BL_PIN,GPIO_PIN_SET)
#define LED_ON() HAL_GPIO_WritePin(LCD_BL_PORT,LCD_BL_PIN,GPIO_PIN_RESET)

#define SWAP(x) (x>>8)|(x<<8)

//RS
#define SET_CMD()	HAL_GPIO_WritePin(LCD_CMD_PORT,LCD_CMD_PIN,GPIO_PIN_RESET)
#define SET_DATA()	HAL_GPIO_WritePin(LCD_CMD_PORT,LCD_CMD_PIN,GPIO_PIN_SET)


#define CLR_CS()	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_RESET)
#define SET_CS()	HAL_GPIO_WritePin(LCD_CS_PORT,LCD_CS_PIN,GPIO_PIN_SET)


#define TFT_SPI_Write_Byte(num)		{\
	while(((LCD_SPI->SR) & (SPI_FLAG_TXE)) != (SPI_FLAG_TXE));\
	 *((__IO uint8_t*)&LCD_SPI->DR) = num;\
	 while(((LCD_SPI->SR) & SPI_FLAG_BSY) == SPI_FLAG_BSY); }

/******************************************************************************************/

//ExFlash IO Define	 
/***
SPI:      SPI1
CS		  PB6
CLK  	  PB3
MISO:     PB5
MOSI: 	  PB4

****/
#define FLASH_SPI_CLOCK_EN()  __HAL_RCC_SPI1_CLK_ENABLE() 	 
	 
#define FLASH_CS_PORT		GPIOB
#define FLASH_CS_PIN		GPIO_PIN_6

#define FLASH_CLK_PORT		GPIOB
#define FLASH_CLK_PIN		GPIO_PIN_3

#define FLASH_MOSI_PORT		GPIOB
#define FLASH_MOSI_PIN		GPIO_PIN_4	 
	 
#define FLASH_MISO_PORT		GPIOB
#define FLASH_MISO_PIN		GPIO_PIN_5		 
	 
#define FLASH_SPI			SPI1	 

#define FLASH_SPI_AF        GPIO_AF0_SPI1

#define FLASH_CS_H() 		HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_SET);
#define FLASH_CS_L()		HAL_GPIO_WritePin(FLASH_CS_PORT, FLASH_CS_PIN, GPIO_PIN_RESET);


void rotate_ui(uint8_t dir);


#endif

