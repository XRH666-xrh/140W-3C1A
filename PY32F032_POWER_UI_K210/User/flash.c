/**
  ******************************************************************************
  * @file    flash.c
  * @author  MCU Application Team
  * @brief   Main program body
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"

/* Private define ------------------------------------------------------------*/
#define FLASH_USER_START_ADDR     0x0800f000
#define FLASH_USER_LENGTH         0x90
#define PAGE_MASK                 (FLASH_PAGE_SIZE-1)

/* Private user code ---------------------------------------------------------*/
uint8_t buf[FLASH_PAGE_SIZE] = {0};

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_FlashErase(void);
static void APP_FlashProgramPage(uint32_t addr, uint32_t *dat);


uint8_t FlashReadByteConfig(void)
{
	uint8_t dat;
	uint16_t save_offset = 0;
	while(save_offset < FLASH_USER_LENGTH)
	{
		dat = HW8_REG(FLASH_USER_START_ADDR + save_offset);
		if (dat & 0x80)
		{
			// bit8 = valid
			return dat;
		}
		save_offset++;
	}
	return 0; // invalid
}

static void FLASHReadPage(uint16_t page_start)
{
	uint8_t buf_offset = 0;
	while (buf_offset < FLASH_PAGE_SIZE)
	{
		(*(uint32_t*)(buf+buf_offset)) = HW32_REG(FLASH_USER_START_ADDR+page_start+buf_offset);
		buf_offset += 4;
	}
}

uint8_t FlashUpdateByteConfig(uint8_t dat)
{
  /* Enable SYSCFG and PWR clocks */
  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

	uint16_t save_offset = 0;
	while(save_offset < FLASH_USER_LENGTH)
	{
		if (HW8_REG(FLASH_USER_START_ADDR + save_offset) & 0x80)
		{
			break;
		}
		save_offset++;
	}
	if (save_offset >= FLASH_USER_LENGTH -1)
	{
		/* ALL SAVE ADDRESS ARE FULL, ERASE FLASH AND WRITE FROM START*/
		APP_FlashErase();
		memset(buf, 0xff, FLASH_PAGE_SIZE);
		buf[0] = dat;
		APP_FlashProgramPage(FLASH_USER_START_ADDR, (uint32_t*)buf);
		save_offset = 0;
	}
	else
	{
		uint8_t page_start = save_offset & ~PAGE_MASK;
		uint8_t page_offset = save_offset & PAGE_MASK;
		// readout flash
		FLASHReadPage(page_start);
		buf[page_offset++] &= ~0x80; // set old data as invalid and move to next write byte
		if (page_offset == FLASH_PAGE_SIZE)
		{
			APP_FlashProgramPage(FLASH_USER_START_ADDR+page_start, (uint32_t*)buf);
			page_start += FLASH_PAGE_SIZE;
			page_offset = 0; // write next page
			memset(buf, 0xff, FLASH_PAGE_SIZE);
		}
		buf[page_offset] = dat;
		APP_FlashProgramPage(FLASH_USER_START_ADDR+page_start, (uint32_t*)buf);
		save_offset = page_start + page_offset;
	}
	return 0;
}

/**
  * @brief  FLASH erase FULL User data sector
  * @param  None
  * @retval None
  */
static void APP_FlashErase(void)
{
  /* Unlock FLASH */
  LL_FLASH_Unlock(FLASH);
  /* Wait Busy=0 */
  while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);

  /* Enable EOP */
  LL_FLASH_EnableIT_EOP(FLASH);

  /* Enable Sector Erase */
  LL_FLASH_EnableSectorErase(FLASH);

  /* Set Erase Address */
  LL_FLASH_SetEraseAddress(FLASH,FLASH_USER_START_ADDR);

  /* Wait Busy=0 */
  while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);

  /* Wait EOP=1 */
  while(LL_FLASH_IsActiveFlag_EOP(FLASH)==0);

  /* Clear EOP */
  LL_FLASH_ClearFlag_EOP(FLASH);

  /* Disable EOP */
  LL_FLASH_DisableIT_EOP(FLASH);

  /* Disable Sector Erase */
  LL_FLASH_DisableSectorErase(FLASH);

  /* Lock FLASH */
  LL_FLASH_Lock(FLASH);
}

/**
  * @brief  FLASH programming a uint32_t
  * @param  None
  * @retval None
  */

static void APP_FlashProgramPage(uint32_t addr, uint32_t *dat)
{
  uint32_t flash_program_start = addr ;                                /* Start address of user write flash */
  uint32_t *src = (uint32_t *)dat;                                                     /* Pointer to array */

  /* Unlock FLASH */
  LL_FLASH_Unlock(FLASH);
    /* Wait Busy=0 */
    while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);
    
    /* Enable EOP */
    LL_FLASH_EnableIT_EOP(FLASH);

    /* Enable Program */
    LL_FLASH_EnablePageProgram(FLASH);

    /* Page Program */
    LL_FLASH_PageProgram(FLASH,flash_program_start,src);
    
    /* Wait Busy=0 */
    while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);
    
    /* Wait EOP=1 */
    while(LL_FLASH_IsActiveFlag_EOP(FLASH)==0);
    
    /* Clear EOP */
    LL_FLASH_ClearFlag_EOP(FLASH);
   
    /* Disable EOP */
    LL_FLASH_DisableIT_EOP(FLASH);

    /* Disable Program */
    LL_FLASH_DisablePageProgram(FLASH);

  /* Lock FLASH */
  LL_FLASH_Lock(FLASH);
}

