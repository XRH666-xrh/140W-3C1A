/* vim: set ai et ts=4 sw=4: */
#include "main.h"
#include "uart.h"
#include "debug.h"

/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart;

/* Buffer used for reception */
uint8_t aRxBuffer[2];
uint8_t bRxBuffer[2];
ringbuff_t rb2 = {0};
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/


void initUart(void){
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_UART1_CLK_ENABLE();

	
	/**USART GPIO Configuration
	PF0     ------> USART1_TX
	PF1     ------> USART1_RX
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_UART1;
	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

//	GPIO_InitStruct.Pin = GPIO_PIN_0;
//	GPIO_InitStruct.Alternate = GPIO_AF9_UART1;
//	HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

	
	/* USART2³õÊ¼»¯ */
	huart.Instance = UART1;
	huart.Init.BaudRate = 115200; 
	huart.Init.WordLength = UART_WORDLENGTH_8B;
	huart.Init.StopBits = UART_STOPBITS_1;
	huart.Init.Parity = UART_PARITY_NONE;
//  huart.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
//  huart.Init.Mode         = UART_MODE_RX;
//  huart.Init.OverSampling = UART_OVERSAMPLING_16;
//  huart.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	HAL_UART_Init(&huart);
//	__HAL_UART_ENABLE(&huart);

	/* ENABLE NVIC */
	#if 1

	/* Enable USART2 interrupt */
	HAL_NVIC_SetPriority(UART1_IRQn, 0, 1);
	HAL_NVIC_EnableIRQ(UART1_IRQn);
  
  /* Enable RXNE interrupt */
  __HAL_UART_ENABLE_IT(&huart, UART_IT_RXNE);
	//HAL_UART_Receive_IT(&huart, (uint8_t *)aRxBuffer, 1);
	#endif
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if ((rb2.write + 1) % RB_BUFF_SIZE != rb2.read)
	{
		rb2.buff[rb2.write] = aRxBuffer[0];
		rb2.write = (rb2.write+1) % RB_BUFF_SIZE;
	}
	//LOG("%02x\n",aRxBuffer[0]);
	//HAL_UART_Receive_IT(huart, (uint8_t *)aRxBuffer, 1);
}

void UART1_IRQHandler(){
//  HAL_UART_IRQHandler(&huart);
//	HAL_UART_Receive_IT(&huart, (uint8_t *)aRxBuffer, 1);
//	return;
	uint8_t RxData = 0;
	if ((__HAL_UART_GET_FLAG(&huart, UART_FLAG_RXNE) != RESET) && \
		(__HAL_UART_GET_IT_SOURCE(&huart, UART_IT_RXNE) != RESET))
	{
			RxData = huart.Instance->DR;
			if ((rb2.write + 1) % RB_BUFF_SIZE != rb2.read)
			{
				rb2.buff[rb2.write] = RxData;
				rb2.write = (rb2.write+1) % RB_BUFF_SIZE;
			}
			//LOG("%02X\n",RxData);
	}
	//__HAL_UART_CLEAR_FLAG(&huart, UART_FLAG_RXNE);
}
