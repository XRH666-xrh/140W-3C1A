/***************************************************************************
* File name    :	key.h
* Function     :	°´¼ü½Ó¿Ú
* Author       : 	Mcu System Team
* Date         :  26-02-2021
* Version      :  V1.0.0
* ModifyRecord :
****************************************************************************/
#ifndef _KEY_H
#define _KEY_H

/***************************************************************************
* Include Header Files
***************************************************************************/
#include "main.h"

/**************************************************************************
* Global Macro Definition
***************************************************************************/

/**************************************************************************
* Global Type Definition
***************************************************************************/

typedef enum 
{
	KEY_NONE = 0, 
	KEY_SHORT_PRESS = 0x01, 
	KEY_LONG_PRESS = 0x02, 
} KEY_STATUS_FLAG;


typedef enum
{
	KEY_NONE_PRES,
	KEY_PRES,								//KEY press
	KEY_LONG_PRES,					//KEY long press
	KEY_SHORT_PRES_RELESE,	//KEY short release
	KEY_LONG_PRES_RELESE,		//KEY long release 
	KEY_DOUBLE_CLICK,				
}KEY_VALUE;

typedef struct
{
	uint8_t key_cnt;
	KEY_VALUE key_value;
	uint8_t key_double_click_smoothing_cnt;
	uint8_t updata_cnt;
	KEY_STATUS_FLAG key_long_press_flag;
	uint32_t(*key_stat)(void);
}Key_Str;

/**************************************************************************
* Global Variable Declaration
***************************************************************************/

#define KEY_NUMS 1
extern Key_Str keys[];
/**************************************************************************
* Global Function Declaration
***************************************************************************/
void key_init(void);
void get_key_event(void);

#endif
