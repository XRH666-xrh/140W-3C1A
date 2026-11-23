#include "key.h"
//#include "ADKEY.h"
#define KEY_EN 1

/**********************************************************************************
* Local Macro Definition
**********************************************************************************/
#define KEY_PRESS_DEBOUNCE_TIMER 1																	//按键短按去抖时间
#define KEY_LONG_PRESS_DELAY_TIMER 60																//按键长按生效时间
#define KEY_LONG_PRESS_INTERVAL_TIMER (KEY_LONG_PRESS_DELAY_TIMER - 3)	//按键长按发送按键按下间隔时长
#define KEY_DOUBLE_CLICK_SMOOTHING_TIMER 30															//判断是否为双击按键的间隔

/**********************************************************************************
* Local Type Definition
**********************************************************************************/
/**********************************************************************************
* Local static Variable Declaration
**********************************************************************************/

/**********************************************************************************
* Global Variable Declaration
**********************************************************************************/
Key_Str keys[KEY_NUMS] = {0};
/**********************************************************************************
* Local Function Declaration
**********************************************************************************/

#define PWR_KEY_PORT GPIOA
#define PWR_KEY_PIN  GPIO_PIN_1

static inline uint32_t IS_KEY_ON() {return LL_GPIO_IsInputPinSet(PWR_KEY_PORT, PWR_KEY_PIN);}

/***************************************************************************
* Subroutine: g_key_init
* Function:   按键IO初始化
* Input:      无  
* Output:     无		  
* Date:			  
* Modify:
**************************************************************************/
void key_init(void)
{
  GPIO_InitTypeDef  GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE();                                 /* Enable GPIO clock */

  GPIO_InitStruct.Pin = PWR_KEY_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;            /* Push-pull output */
  GPIO_InitStruct.Pull = GPIO_PULLUP;                    /* Enable pull-up */
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;          /* GPIO speed */  
  /* GPIO Initialization */
  HAL_GPIO_Init(PWR_KEY_PORT, &GPIO_InitStruct);

	keys[0].key_stat = IS_KEY_ON;
}

/***************************************************************************
* Subroutine: key_handle
* Function:   按键IO状态处理
* Input:      无  
* Output:     按键状态		  
* Date:			  
* Modify:
**************************************************************************/
KEY_VALUE key_handle(Key_Str *keyPt)
{
	KEY_VALUE ret = KEY_NONE_PRES;
	if(keyPt->key_stat() == 0)
	{
		keyPt->key_cnt++;

		if((!keyPt->key_long_press_flag))
		{
			if((keyPt->key_cnt >= KEY_PRESS_DEBOUNCE_TIMER))
			{
				keyPt->key_cnt  = 0;
				keyPt->key_long_press_flag = KEY_SHORT_PRESS;
				//LOG("key press\r\n");
				return KEY_PRES;
			}
		}
		else
		{
			if(keyPt->key_cnt > KEY_LONG_PRESS_DELAY_TIMER)
			{
				keyPt->key_cnt  = KEY_LONG_PRESS_INTERVAL_TIMER;
				if(keyPt->key_long_press_flag != KEY_LONG_PRESS)
				{
					keyPt->key_long_press_flag = KEY_LONG_PRESS;
					//LOG("key hold press\r\n");
					return KEY_LONG_PRES;
				}
			}
		}	
	}
	else
	{
		if(keyPt->key_long_press_flag & KEY_SHORT_PRESS)
		{
			ret = KEY_SHORT_PRES_RELESE;
			//LOG("KEY_SHORT_PRES_RELESE\r\n");
		}
		else if(keyPt->key_long_press_flag & KEY_LONG_PRESS)
		{
			ret = KEY_LONG_PRES_RELESE;
			//LOG("KEY_LONG_PRES_RELESE\r\n");
		}
		keyPt->key_cnt = 0;
		keyPt->key_long_press_flag = KEY_NONE;
		if(ret)
		{
			return ret;
		}
	}

	return KEY_NONE_PRES;
}



/***************************************************************************
* Subroutine: g_key_process
* Function:   获取按键事件
* Input:      无  
* Output:     按键事件		  
* Date:			  
* Modify:			20210302-添加双击消息功能
**************************************************************************/
void get_key_event(void)
{
	uint8_t i = 0;
	for (i = 0; i < KEY_NUMS; i++)
	{
		KEY_VALUE key_ret, ret = KEY_NONE_PRES;
		Key_Str *keyPt = keys+i;
		key_ret = key_handle(keyPt);

		if(key_ret == KEY_SHORT_PRES_RELESE)
		{
			if(!keyPt->key_double_click_smoothing_cnt)
			{
				keyPt->key_double_click_smoothing_cnt = KEY_DOUBLE_CLICK_SMOOTHING_TIMER;	//按键短按释放消息延迟发送时间

			}
			else
			{//如果此时按键按下释放与上次间隔时间很短，则认为是双击
				keyPt->key_double_click_smoothing_cnt = 0;

				ret = KEY_DOUBLE_CLICK;
			}
		}
		else if(key_ret == KEY_LONG_PRES)
		{
			ret = KEY_LONG_PRES;
		}
		else if (key_ret == KEY_PRES)
		{
			ret = KEY_PRES;
		}
		if (keyPt->key_double_click_smoothing_cnt)
		{
			if(1 == keyPt->key_double_click_smoothing_cnt)
			{//双击按键间隔超时，处理按键短按释放消息
				ret = KEY_SHORT_PRES_RELESE;
			}
			keyPt->key_double_click_smoothing_cnt--;
		}
		keyPt->key_value = ret;
	}
}

