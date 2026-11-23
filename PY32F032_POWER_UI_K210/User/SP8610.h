#ifndef __DEVICE_SLAVE_INTERFACE_H
#define __DEVICE_SLAVE_INTERFACE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define IIC_ADDR 0x11
#pragma pack(1)
typedef struct
{
	uint8_t 		PortStatus 				    ;/* 0x00 */
	uint8_t 		PortAbnormalStatus    ;/* 0x01 */
	uint8_t 		H1_Port_Error_Clear		;/* 0x02 */
	uint8_t 		H2_Port_Error_Clear		;/* 0x03 */
	uint8_t 		H1PortVoltage				  ;/* 0x04 */
	uint8_t 		H1PortCurrent				  ;/* 0x05 */
	uint8_t 		H2PortVoltage				  ;/* 0x06 */
	uint8_t 		H2PortCurrent				  ;/* 0x07 */
	uint16_t 		BoardNTCValue			  ;/* 0x08 0x09*/
	//uint8_t 		BoardNTCValue_L			  ;/* 0x08 0x09*/
	//uint8_t 		BoardNTCValueH			  ;/* 0x09 */
	uint16_t 		Charge_Voltage			;/* 0x0A */
	//uint8_t 		Charge_Voltage_L			;/* 0x0A */
	//uint8_t     Charge_Voltage_H      ;/* 0x0B */
	uint8_t     H1PortProtocol        ;/* 0x0C */
	uint8_t     H2PortProtocol        ;/* 0x0D */
	uint8_t 		MODULE_RESET					;/* 0x0E */
	uint8_t 		MODULE_SLEEP					;/* 0x0F */
	uint8_t 		TYPEA1_DFP_EN					;/* 0x10 */
	uint8_t 		TYPEC1_UFP_EN					;/* 0x11 */
	uint8_t 		TYPEC1_DFP_EN					;/* 0x12 */
	uint8_t 		TYPEA2_DFP_EN					;/* 0x13 */
	uint8_t 		TYPEC2_UFP_EN					;/* 0x14 */
	uint8_t 		TYPEC2_DFP_EN					;/* 0x15 */
	uint8_t 		TYPEA1_Halt				  	;/* 0x16 */
	uint8_t 		TYPEC1_Halt					  ;/* 0x17 */
	uint8_t     TYPEA2_Halt           ;/* 0x18 */
	uint8_t     TYPEC2_Halt           ;/* 0x19 */
	uint8_t     Port_All_On           ;/* 0x1A */
	uint8_t     H1_Bridge_EN          ;/* 0x1B */
	uint8_t     H2_Bridge_EN          ;/* 0x1C */
	uint16_t     Vbat                ;/* 0x1D */
	//uint8_t     Vbat_L                ;/* 0x1D */
	//uint8_t     Vbat_H                ;/* 0x1E */
	uint8_t     TYPEC_PD_OBJ_SEL      ;/* 0x1F */
	uint8_t     TYPEA_RM_Time         ;/* 0x20 */
	uint8_t     TYPEA_RM_Ctrl         ;/* 0x21 */
	uint8_t     Charge_Vol_W_F        ;/* 0x22 */
	uint8_t     Code_Version          ;/* 0x23 */
	uint8_t     Error_Auto_Clear      ;/* 0x24 */
	uint8_t			Chip_SWD_EN						;/* 0x25 */
	uint8_t			C1_PD_OBJ_SEL					;/* 0x26 */
	uint8_t			C2_PD_OBJ_SEL					;/* 0x27 */
	uint8_t			C1_Charge_Power				;/* 0x28 */
	uint8_t			C2_Charge_Power				;/* 0x29 */
	uint16_t			H1_Vout_Vol					;/* 0x2A */
	//uint8_t			H1_Vout_Vol_L					;/* 0x2A */
	//uint8_t			H1_Vout_Vol_H					;/* 0x2B */
	uint16_t			H2_Vout_Vol					;/* 0x2C */
	//uint8_t			H2_Vout_Vol_L					;/* 0x2C */
	//uint8_t			H2_Vout_Vol_H					;/* 0x2D */
	uint32_t			Bat_NTC_Value				;/* 0x2E */
	//uint8_t			Bat_NTC_Value_1				;/* 0x2E */
	//uint8_t			Bat_NTC_Value_2				;/* 0x2F */
	//uint8_t			Bat_NTC_Value_3				;/* 0x30 */
	//uint8_t			Bat_NTC_Value_4				;/* 0x31 */
	uint8_t			BatteryAutoDetectEn		;/* 0x32 */
	uint8_t			BatteryUVPStateConfig ;/* 0x33 */
	uint8_t			BatteryFullStateConfig ;/* 0x34 */
	uint16_t			BatteryDeadVol			;/* 0x35 */
	//uint8_t			BatteryDeadVol_L			;/* 0x35 */
	//uint8_t			BatteryDeadVol_H	    ;/* 0x36 */	
	uint8_t			BatteryPrecent	    ;/* 0x37 */
	uint8_t			Batterynum		;/* 0x38 */
}DSI_REG_INFO;

typedef enum
{
	ADDR_PortStatus 				    ,/* 0x00 */
	ADDR_PortAbnormalStatus    ,/* 0x01 */
	ADDR_H1_Port_Error_Clear		,/* 0x02 */
	ADDR_H2_Port_Error_Clear		,/* 0x03 */
	ADDR_H1PortVoltage				  ,/* 0x04 */
	ADDR_H1PortCurrent				  ,/* 0x05 */
	ADDR_H2PortVoltage				  ,/* 0x06 */
	ADDR_H2PortCurrent				  ,/* 0x07 */
	ADDR_BoardNTCValueL			  ,/* 0x08 */
	ADDR_BoardNTCValueH			  ,/* 0x09 */
	ADDR_Charge_Voltage_L			,/* 0x0A */
	ADDR_Charge_Voltage_H      ,/* 0x0B */
	ADDR_H1PortProtocol        ,/* 0x0C */
	ADDR_H2PortProtocol        ,/* 0x0D */
	ADDR_MODULE_RESET					,/* 0x0E */
	ADDR_MODULE_SLEEP					,/* 0x0F */
	ADDR_TYPEA1_DFP_EN					,/* 0x10 */
	ADDR_TYPEC1_UFP_EN					,/* 0x11 */
	ADDR_TYPEC1_DFP_EN					,/* 0x12 */
	ADDR_TYPEA2_DFP_EN					,/* 0x13 */
	ADDR_TYPEC2_UFP_EN					,/* 0x14 */
	ADDR_TYPEC2_DFP_EN					,/* 0x15 */
	ADDR_TYPEA1_Halt				  	,/* 0x16 */
	ADDR_TYPEC1_Halt					  ,/* 0x17 */
	ADDR_TYPEA2_Halt           ,/* 0x18 */
	ADDR_TYPEC2_Halt           ,/* 0x19 */
	ADDR_Port_All_On           ,/* 0x1A */
	ADDR_H1_Bridge_EN          ,/* 0x1B */
	ADDR_H2_Bridge_EN          ,/* 0x1C */
	ADDR_Vbat_L                ,/* 0x1D */
	ADDR_Vbat_H                ,/* 0x1E */
	ADDR_TYPEC_PD_OBJ_SEL      ,/* 0x1F */
	ADDR_TYPEA_RM_Time         ,/* 0x20 */
	ADDR_TYPEA_RM_Ctrl         ,/* 0x21 */
	ADDR_Charge_Vol_W_F        ,/* 0x22 */
	ADDR_Code_Version          ,/* 0x23 */
	ADDR_Error_Auto_Clear      ,/* 0x24 */
	ADDR_Chip_SWD_EN						,/* 0x25 */
	ADDR_C1_PD_OBJ_SEL					,/* 0x26 */
	ADDR_C2_PD_OBJ_SEL					,/* 0x27 */
	ADDR_C1_Charge_Power				,/* 0x28 */
	ADDR_C2_Charge_Power				,/* 0x29 */
	ADDR_H1_Vout_Vol_L					,/* 0x2A */
	ADDR_H1_Vout_Vol_H					,/* 0x2B */
	ADDR_H2_Vout_Vol_L					,/* 0x2C */
	ADDR_H2_Vout_Vol_H					,/* 0x2D */
	ADDR_Bat_NTC_Value_1				,/* 0x2E */
	ADDR_Bat_NTC_Value_2				,/* 0x2F */
	ADDR_Bat_NTC_Value_3				,/* 0x30 */
	ADDR_Bat_NTC_Value_4				,/* 0x31 */
	ADDR_BatteryAutoDetectEn		,/* 0x32 */
	ADDR_BatteryUVPStateConfig ,/* 0x33 */
	ADDR_BatteryFullStateConfig ,/* 0x34 */
	ADDR_BatteryDeadVol_L			,/* 0x35 */
	ADDR_BatteryDeadVol_H	    ,/* 0x36 */	
	ADDR_BatteryPrecent	    ,/* 0x37 */
	ADDR_Batterynum		,/* 0x38 */
	IIC_REG_SIZE,
} I2C_REG_ADDRS;
#pragma pack()
#endif

