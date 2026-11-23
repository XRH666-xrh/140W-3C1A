/*
 * usermain.c
 *
 *  Created on: Jun 12, 2020
 *      Author: 钟
 */

#include "main.h"
#include "devmodule.h"
#include "context/guicontext.h"
#include "event/event.h"
#include "timer/timer.h"
#include "graphics/video.h"
#include "context/resource.h"
#include "ui_page.h"
#include "usermain.h"
#include "SerialFlash.h"
#include "display.h"
#include "devmodule.h"
#include "key.h"
#include "pm.h"
const S_DEV* uartdev;		
const S_DEV* uart1dev;	


static uint32_t tick5ms = 0;
static uint32_t tick10ms = 0;
static uint32_t tick50ms = 0;
static uint32_t tick100ms = 0;
static uint32_t tick500ms = 0;
static uint32_t tick1s = 0;
static uint32_t tick60s = 0;

extern const flythings_flash	m_flash;

void my_printf_null(const char *arg,...){};
	
const flythings_callback_t cbs={
	.init_event = event_init,
	.init_timer = timer_init,
	.init_video = video_init,
	.deinit_event = event_deinit,
	.deinit_timer = timer_deinit,
	.deinit_video = video_deinit,
	.cb_on_pageopen = on_page_open,
	.cb_on_animation_play_end = on_animation_play_end,
	.cb_on_button_touch_event = on_button_touch_event,
	.cb_on_progress_changed = on_progress_changed,
	.cb_on_timer = on_timer,
	.poll_timer = timer_check,
	.poll_event = event_poll,
	.flip_video = video_flip,
	#if USER_PRINTF
	.fly_debug = my_printf_null
	#else
	.fly_debug = my_printf_null
	#endif
};


/********************************************************************************************************
**函数信息 :
**功能描述 :
**输入参数 :
**输出参数 :
**    备注 :
********************************************************************************************************/
static void P25QXX_Gpio_Init(void)
{
   
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();                    
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;           
	GPIO_InitStruct.Pull = GPIO_PULLUP;                   
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;         
	GPIO_InitStruct.Pin = FLASH_CS_PIN;
	HAL_GPIO_Init(FLASH_CS_PORT, &GPIO_InitStruct); 				
	FLASH_CS_H();

	GPIO_InitStruct.Pull 	   = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;

	GPIO_InitStruct.Pin       = FLASH_MOSI_PIN;				//SPI2 MOSI 
	GPIO_InitStruct.Alternate = FLASH_SPI_AF;
	HAL_GPIO_Init(FLASH_MOSI_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin       = FLASH_MISO_PIN;				//SPI2 MISO 
	GPIO_InitStruct.Alternate = FLASH_SPI_AF;
	HAL_GPIO_Init(FLASH_MISO_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin       = FLASH_CLK_PIN;				//SPI2 CLK 
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Alternate = FLASH_SPI_AF;
	HAL_GPIO_Init(FLASH_CLK_PORT, &GPIO_InitStruct);
}

uint32_t flash_id = 0;
/**
GUI 动态内存分配大小
**/

const uint32_t mem_buff_conunt = 3072;
uint32_t _s_mem_buf[3072 / sizeof(uint32_t)];


// lcd power qe
void user_init(void){
	
#if USER_PRINTF		
	uartdev = loadDev(&dev_uartupgrade);
 	printf("load uart \r\n");
	printf("led on\n");
#endif	
	key_init();
	pm_init();
	HAL_Delay(100);
	tick10ms = HAL_GetTick();
	tick50ms = HAL_GetTick();
	tick100ms = HAL_GetTick();
	tick500ms = HAL_GetTick();
	tick1s = HAL_GetTick();
	tick60s = HAL_GetTick();
	tick5ms = HAL_GetTick();
	P25QXX_Gpio_Init();
	
	gui_context_setspi_tft(LCD_SPI);
	gui_context_setgpio_tftcs(LCD_CS_PORT ,LCD_CS_PIN);
	gui_context_setgpio_tftdc(LCD_CMD_PORT, LCD_CMD_PIN);
	
	gui_context_setgpio_flashcs(FLASH_CS_PORT, FLASH_CS_PIN);

	gui_context_setspi_flash(FLASH_SPI);

	video_info_t vi = { LCD_WIDTH, LCD_HEIGHT, 16, 0 };
	rotate_ui(0);

#if INTERNAL_FLASH
	if(gui_context_init(&vi,&cbs,&i_flash)){
#else
	m_flash.flash_init();
	flash_id = fy_flash_readid();
	if(gui_context_init(&vi,&cbs,&m_flash)){
#endif
#if USER_PRINTF			
		printf("open page");
#endif	
	gui_context_open_page(0);
	}
//ST7735_Init();
#if USER_PRINTF		
	printf(" init end \n");
	uartdev->procMsg(DEVMSG_TIMER10ms,0,0);
#endif		
	display_init();
	update_display(100);
	LED_ON();
}

void rotate_ui(uint8_t dir)
{
	uint32_t x, y;
	switch(dir)
	{
		case 0:
		{
			x = 0;
			y = 0;
			break;
		}
		case 1:
		{
			x = 0;
			y = 0;
			break;
		}
		case 2:
		{
			x = 0;
			y = 80;
			break;
		}
		case 3:
		{
			x = 80;
			y = 0;
			break;
		}
	}

	gui_context_setarg(E_SETARG_YINDEX,(void*)y);
	gui_context_setarg(E_SETARG_XINDEX,(void*)x);
}

void user_loop(void)
{
	uint32_t interval_now = HAL_GetTick();
	static uint32_t displayInterval = 0;
	pm_process();
#if USER_PRINTF
	static uint32_t flashInterval = 0;
	if (interval_now - flashInterval >= 110)
	{
		uartdev->procMsg(1,0,0);
		flashInterval = interval_now;
	}
#else
#endif
	if (interval_now - displayInterval >= 10)
	{

		update_display(10);
		displayInterval = interval_now;
	}
	gui_context_run();
}
