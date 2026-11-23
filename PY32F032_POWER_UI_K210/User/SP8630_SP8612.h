#ifndef __DEVICE_SLAVE_INTERFACE_H
#define __DEVICE_SLAVE_INTERFACE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "uart.h"

typedef enum
{
	PROTOCOL_INVALID = 0,
	PROTOCOL_NA,
	PROTOCOL_BC,
	PROTOCOL_BC_OUT,	 //默认BC输出
	PROTOCOL_QC,
	PROTOCOL_SCP_AFC,
	PROTOCOL_PD_SINK,
	PROTOCOL_PD_SRC,
	PROTOCOL_VOOC_SRC,
	PROTOCOL_UFCS,
	PROTOCOL_MPPT,
}ENUM_PROTOCOL_MODE;
#define IS_ADJ_PROTOCOL(protocol) ((protocol>>7)&1==1)

#define PM_PORT_MODE_IDLE                  0
#define PM_PORT_MODE_SINK                  0x01
#define PM_PORT_MODE_SOURCE                0x10
#define PM_PORT_SINK_MARK                  0x0F
#define PM_PORT_SOURCE_MARK                0xF0
#define PM_PORT_MODE_MARK                  0x7F /* Port2_Mode的D7位用作小电流模式状态 */

typedef enum
{
	PM_NTC_PROTECT_NONE = 0,
	PM_NTC_H1_DEGREE_REDUCE, 	/* 高温降功率1档 */
	PM_NTC_H2_DEGREE_REDUCE, 	/* 高温降功率2档 */
	PM_NTC_H3_DEGREE_REDUCE, 	/* 高温降功率3档 */
	PM_NTC_PROTECT_EFFECT,		/* 过温保护 */
}PM_NTC_PROTECT_STATUS;

#define IIC_ADDR 0X74
#pragma pack(1)

typedef enum
{
	DSI_KEY_NONE_PRES,
	DSI_KEY_PRES,								//KEY press
	DSI_KEY_DOUBLE_CLICK,				//KEY double click
	DSI_KEY_THREE_PRESS,
	DSI_KEY_FOUR_PRESS,
	DSI_KEY_FIVE_PRESS,
	DSI_KEY_LONG_PRES,					//KEY long press
}DSI_KEY_VALUE;

typedef enum 
{
	Port1_Mode = 0x00				                ,/* 0x00 */
	Port2_Mode							                ,/* 0x01 */
	Port3_Mode							                ,/* 0x02 */
	Port4_Mode							                ,/* 0x03 */
	Bridge0_Abnormal_Status     						,/* 0x04 */
	Bridge1_Abnormal_Status     						,/* 0x05 */	
	Bridge2_Abnormal_Status     						,/* 0x06 */
	Bridge3_Abnormal_Status     						,/* 0x07 */	
	Bridge0_VoltageL			      						,/* 0x08 */
	Bridge0_VoltageH			      						,/* 0x09 */
	Bridge0_CurrentL			      						,/* 0x0A */
	Bridge0_CurrentH			      						,/* 0x0B */
	Bridge1_VoltageL			      						,/* 0x0C */
	Bridge1_VoltageH			      						,/* 0x0D */
	Bridge1_CurrentL			      						,/* 0x0E */
	Bridge1_CurrentH			      						,/* 0x0F */
	Bridge2_VoltageL			      						,/* 0x10 */
	Bridge2_VoltageH			      						,/* 0x11 */
	Bridge2_CurrentL			      						,/* 0x12 */
	Bridge2_CurrentH			      						,/* 0x13 */
	Bridge3_VoltageL			      						,/* 0x14 */
	Bridge3_VoltageH			      						,/* 0x15 */
	Bridge3_CurrentL			      						,/* 0x16 */
	Bridge3_CurrentH			      						,/* 0x17 */
	Bridge0_WorkMode			      						,/* 0x18 */
	Bridge1_WorkMode			      						,/* 0x19 */	
	Bridge2_WorkMode			      						,/* 0x1A */
	Bridge3_WorkMode			      						,/* 0x1B */	
	Port1_Protocol					                ,/* 0x1C */
	Port2_Protocol					                ,/* 0x1D */
	Port3_Protocol					                ,/* 0x1E */
	Port4_Protocol					                ,/* 0x1F */
	Port1_Chg_Power_Sp			                ,/* 0x20 */
	Port2_Chg_Power_Sp			                ,/* 0x21 */
	Port3_Chg_Power_Sp			                ,/* 0x22 */
	Port4_Chg_Power_Sp			                ,/* 0x23 */
	Port1_Charger_Power 		                ,/* 0x24 */
	Port2_Charger_Power 		                ,/* 0x25 */
	Port3_Charger_Power 		                ,/* 0x26 */
	Port4_Charger_Power 		                ,/* 0x27 */
	Port1_Source_CurL 			                ,/* 0x28 */
	Port1_Source_CurH 			                ,/* 0x29 */
	Port2_Source_CurH 			                ,/* 0x2A */
	Port2_Source_CurL 			                ,/* 0x2B */
	Port3_Source_CurH 			                ,/* 0x2C */
	Port3_Source_CurL 			                ,/* 0x2D */
	Port4_Source_CurH 			                ,/* 0x2E */
	Port4_Source_CurL 			                ,/* 0x2F */
	Ports_EN_State					                ,/* 0x30 */
	BatVol_Ntc_Status 			                ,/* 0x31 */
	Ntc_Temp								                ,/* 0x32 */
	Coulomb_Counter 				                ,/* 0x33 */
	VBAT_VoltageL			                      ,/* 0x34 */
	VBAT_VoltageH			                      ,/* 0x35 */
	IBAT_CurrentL			                      ,/* 0x36 */
	IBAT_CurrentH			                      ,/* 0x37 */
	Chip_SWD_EN 						                ,/* 0x38 */
	Code_Version						                ,/* 0x39 */
	Port1_UFP_EN						                ,/* 0x3A */
	Port1_DFP_EN						                ,/* 0x3B */
	Port1_PD_OBJ_SEL				                ,/* 0x3C */
	Port1_Bridge_Num				                ,/* 0x3D */
	Port2_UFP_EN						                ,/* 0x3E */
	Port2_DFP_EN						                ,/* 0x3F */
	Port2_PD_OBJ_SEL				                ,/* 0x40 */
	Port2_Bridge_Num				                ,/* 0x41 */
	Port3_UFP_EN						                ,/* 0x42 */
	Port3_DFP_EN						                ,/* 0x43 */
	Port3_PD_OBJ_SEL				                ,/* 0x44 */
	Port3_Bridge_Num				                ,/* 0x45 */
	Port4_UFP_EN						                ,/* 0x46 */
	Port4_DFP_EN						                ,/* 0x47 */
	Port4_PD_OBJ_SEL				                ,/* 0x48 */
	Port4_Bridge_Num				                ,/* 0x49 */
	Bridge0_WorkPort_Num	                  ,/* 0x4A */
	Bridge1_WorkPort_Num	                  ,/* 0x4B */
	Bridge2_WorkPort_Num	                  ,/* 0x4C */
	Bridge3_WorkPort_Num	                  ,/* 0x4D */
	Bat_Status                              ,/* 0x4E */
	PortManage_Event			                  ,/* 0x4F */
	Key_Event_Deal				                  ,/* 0x50 */
	Sink_VID_L							                ,/* 0x51 */
	Sink_VID_H							                ,/* 0x52 */
	IIC_REG_SIZE,
} DSI_REG_ADDR;

typedef struct
{
	uint8_t			Port1_Mode						      ;/* 0x00 *//* 读寄存器 */
	uint8_t			Port2_Mode						      ;/* 0x01 */
	uint8_t			Port3_Mode						      ;/* 0x02 */	
	uint8_t			Port4_Mode						      ;/* 0x03 */
	uint8_t			Bridge0_Abnormal_Status     ;/* 0x04 */
	uint8_t			Bridge1_Abnormal_Status     ;/* 0x05 */	
	uint8_t			Bridge2_Abnormal_Status     ;/* 0x06 */
	uint8_t			Bridge3_Abnormal_Status     ;/* 0x07 */	
	uint16_t			Bridge0_Voltage			      ;/* 0x08 */
	//uint8_t			Bridge0_VoltageL			      ;/* 0x08 */
	//uint8_t			Bridge0_VoltageH			      ;/* 0x09 */
	uint16_t			Bridge0_Current			      ;/* 0x0A */
	//uint8_t			Bridge0_CurrentL			      ;/* 0x0A */
	//uint8_t			Bridge0_CurrentH			      ;/* 0x0B */
	uint16_t			Bridge1_Voltage			      ;/* 0x0C */
	//uint8_t			Bridge1_VoltageL			      ;/* 0x0C */
	//uint8_t			Bridge1_VoltageH			      ;/* 0x0D */
	uint16_t			Bridge1_Current			      ;/* 0x0E */
	//uint8_t			Bridge1_CurrentL			      ;/* 0x0E */
	//uint8_t			Bridge1_CurrentH			      ;/* 0x0F */
	uint16_t			Bridge2_Voltage			      ;/* 0x10 */
	//uint8_t			Bridge2_VoltageL			      ;/* 0x10 */
	//uint8_t			Bridge2_VoltageH			      ;/* 0x11 */
	uint16_t			Bridge2_Current			      ;/* 0x12 */
	//uint8_t			Bridge2_CurrentL			      ;/* 0x12 */
	//uint8_t			Bridge2_CurrentH			      ;/* 0x13 */
	uint16_t			Bridge3_Voltage			      ;/* 0x14 */
	//uint8_t			Bridge3_VoltageL			      ;/* 0x14 */
	//uint8_t			Bridge3_VoltageH			      ;/* 0x15 */
	uint16_t			Bridge3_Current			      ;/* 0x16 */
	//uint8_t			Bridge3_CurrentL			      ;/* 0x16 */
	//uint8_t			Bridge3_CurrentH			      ;/* 0x17 */
	uint8_t			Bridge0_WorkMode			      ;/* 0x18 */
	uint8_t			Bridge1_WorkMode			      ;/* 0x19 */	
	uint8_t			Bridge2_WorkMode			      ;/* 0x1A */
	uint8_t			Bridge3_WorkMode			      ;/* 0x1B */	
	uint8_t			Port1_Protocol				      ;/* 0x1C */
	uint8_t			Port2_Protocol				      ;/* 0x1D */
	uint8_t			Port3_Protocol				      ;/* 0x1E */
	uint8_t			Port4_Protocol				      ;/* 0x1F */	
	uint8_t			Port1_Chg_Power_Sp		      ;/* 0x20 */
	uint8_t			Port2_Chg_Power_Sp		      ;/* 0x21 */
	uint8_t			Port3_Chg_Power_Sp		      ;/* 0x22 */
	uint8_t			Port4_Chg_Power_Sp		      ;/* 0x23 */		
	uint8_t			Port1_Charger_Power		      ;/* 0x24 */
	uint8_t			Port2_Charger_Power		      ;/* 0x25 */
	uint8_t			Port3_Charger_Power		      ;/* 0x26 */
	uint8_t			Port4_Charger_Power		      ;/* 0x27 */
	uint16_t			Port1_Source_Cur			      ;/* 0x28 */
	//uint8_t			Port1_Source_CurL			      ;/* 0x28 */
	//uint8_t			Port1_Source_CurH			      ;/* 0x29 */
	uint16_t			Port2_Source_Cur			      ;/* 0x2A */
	//uint8_t			Port2_Source_CurH			      ;/* 0x2A */
	//uint8_t     Port2_Source_CurL			      ;/* 0x2B */
	uint16_t			Port3_Source_Cur			      ;/* 0x2C */
	//uint8_t			Port3_Source_CurH			      ;/* 0x2C */
	//uint8_t     Port3_Source_CurL			      ;/* 0x2D */
	uint16_t			Port4_Source_Cur			      ;/* 0x2E */
	//uint8_t			Port4_Source_CurH			      ;/* 0x2E */
	//uint8_t     Port4_Source_CurL			      ;/* 0x2F */
	uint8_t     Ports_EN_State              ;/* 0x30 */	
	uint8_t			Ntc_Status                  ;/* 0x31 */	
	uint8_t     Ntc_Temp                    ;/* 0x32 *//* NTC温度 0~145 <====> -20~125度 */
	uint8_t     Coulomb_Counter             ;/* 0x33 */
	uint16_t			VBAT_Voltage			          ;/* 0x34 */
	//uint8_t			VBAT_VoltageL			          ;/* 0x34 */
	//uint8_t			VBAT_VoltageH			          ;/* 0x35 */	
	uint16_t     IBAT_Current               ;/* 0x36 */	
	//uint8_t     IBAT_CurrentL               ;/* 0x36 */	
	//uint8_t     IBAT_CurrentH               ;/* 0x37 */	
	uint8_t     Chip_SWD_EN          	      ;/* 0x38 */
	uint8_t     Code_Version                ;/* 0x39 */		
	uint8_t			Port1_UFP_EN					      ;/* 0x3A *//* 写寄存器 */
	uint8_t			Port1_DFP_EN					      ;/* 0x3B */
	uint8_t			Port1_PD_OBJ_SEL			      ;/* 0x3C */
  uint8_t     Port1_Bridge_Num			      ;/* 0x3D */
	uint8_t			Port2_UFP_EN					      ;/* 0x3E */
	uint8_t			Port2_DFP_EN					      ;/* 0x3F */  
	uint8_t			Port2_PD_OBJ_SEL			      ;/* 0x40 */	
  uint8_t     Port2_Bridge_Num			      ;/* 0x41 */
	uint8_t     Port3_UFP_EN					      ;/* 0x42 */
	uint8_t     Port3_DFP_EN				
	;/* 0x43 */  
	uint8_t     Port3_PD_OBJ_SEL			      ;/* 0x44 */ 
	uint8_t     Port3_Bridge_Num			      ;/* 0x45 */ 	
	uint8_t     Port4_UFP_EN					      ;/* 0x46 */
	uint8_t     Port4_DFP_EN					      ;/* 0x47 */  
	uint8_t     Port4_PD_OBJ_SEL			      ;/* 0x48 */ 
	uint8_t     Port4_Bridge_Num			      ;/* 0x49 */
	uint8_t			Bridge0_WorkPort_Num	      ;/* 0x4A */
	uint8_t			Bridge1_WorkPort_Num	      ;/* 0x4B */	
	uint8_t			Bridge2_WorkPort_Num	      ;/* 0x4C */
	uint8_t			Bridge3_WorkPort_Num	      ;/* 0x4D */	
	uint8_t			Bat_Status                  ;/* 0x4E */		
	uint8_t			PortManage_Event			      ;/* 0x4F */	
	uint8_t			Key_Event_Deal				      ;/* 0x50 */
	uint8_t			Sink_VID_L		              ;/* 0x51 */
	uint8_t			Sink_VID_H		              ;/* 0x52 */
}DSI_REG_INFO;

#pragma pack()

#define DATA_BODY_SIZE 0x40
typedef struct
{
	ringbuff_t *rb;
	void (*hinit)(void);
	enum
	{
		RECEIVE_IDLE,
		RECEIVE_HEAD,
		RECEIVE_CMD,
		RECEIVE_LEN,
		RECEIVE_BODY,
		RECEIVE_CRC,
		RECEIVE_END,
	} receive_stat;
	uint8_t cmd;
	uint16_t to_receive_len;
	uint16_t receive_len;
	uint32_t last_receive_time;
	uint8_t buff[DATA_BODY_SIZE];
	uint16_t receive_crc;
	uint32_t is_receive_newdata;
} uart_recive_stat_t;

typedef union
{
	uint8_t buff[IIC_REG_SIZE];
	DSI_REG_INFO reg;
} i2c_reg_t;
extern i2c_reg_t device;

void sp86xx_init(uart_recive_stat_t *stat);
void sp86xx_recive_process(uart_recive_stat_t *stat);

#endif

