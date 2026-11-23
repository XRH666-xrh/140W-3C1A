#include "system.h"


/***********************************************
**@ 函数名称:SystemClock_Init
**@ 功能描述:系统时钟初始化
**@ 输入参数:void
**@ 输出参数:void
**@ 返 回 值:void
***********************************************/
void SystemClock_Init(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/* Oscillator configuration */
	RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI; /* Select oscillator HSI, LSI */
#if defined(RCC_HSE_SUPPORT)
	RCC_OscInitStruct.OscillatorType      |= RCC_OSCILLATORTYPE_HSE; /* Select oscillator HSE */
#endif /* RCC_HSE_SUPPORT */
#if defined(RCC_LSE_SUPPORT)
	RCC_OscInitStruct.OscillatorType      |= RCC_OSCILLATORTYPE_LSE; /* Select oscillator LSE */
#endif /* RCC_LSE_SUPPORT */
	RCC_OscInitStruct.HSIState            = RCC_HSI_ON;              /* Enable HSI */
	RCC_OscInitStruct.HSIDiv              = RCC_HSI_DIV1;            /* HSI 1 frequency division */
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_8MHz; /* Configure HSI clock 8MHz */
#if defined(RCC_HSE_SUPPORT)
	RCC_OscInitStruct.HSEState            = RCC_HSE_OFF;             /* Close HSE */
	/*RCC_OscInitStruct.HSEFreq             = RCC_HSE_24_32MHz;*/
#endif /* RCC_HSE_SUPPORT */
	RCC_OscInitStruct.LSIState            = RCC_LSI_OFF;             /* Close LSI */
	#if defined(RCC_LSE_SUPPORT)
	RCC_OscInitStruct.LSEState            = RCC_LSE_OFF;             /* Close LSE */
	/*RCC_OscInitStruct.LSEDriver           = RCC_LSEDRIVE_MEDIUM;*/
	#endif /* RCC_LSE_SUPPORT */
	RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;              /* Enable PLL */
	RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLL_MUL9;
	RCC_OscInitStruct.PLL.PLLDiv          = RCC_PLL_DIV1;
	/* Configure oscillator */
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		APP_ErrorHandler();
	}

	/* Clock source configuration */
	RCC_ClkInitStruct.ClockType     = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK; /* Choose to configure clock HCLK, SYSCLK, PCLK */
	RCC_ClkInitStruct.SYSCLKSource  = RCC_SYSCLKSOURCE_PLLCLK; /* Select PLL as the system clock */
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;         /* AHB clock 1 division */
	RCC_ClkInitStruct.APBCLKDivider = RCC_HCLK_DIV1;           /* APB clock 1 division */
	/* Configure clock source */
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		APP_ErrorHandler();
	}
	
}






