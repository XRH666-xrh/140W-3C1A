#ifndef __DEVICE_SLAVE_INTERFACE_H
#define __DEVICE_SLAVE_INTERFACE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

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
	Bridge0_Abnormal_Status                 ,/* 0x04 */
	Bridge1_Abnormal_Status                 ,/* 0x05 */
	Bridge0_VoltageL				                ,/* 0x06 */
	Bridge0_VoltageH				                ,/* 0x07 */
	Bridge0_CurrentL				                ,/* 0x08 */
	Bridge0_CurrentH				                ,/* 0x09 */
	Bridge1_VoltageL				                ,/* 0x0A */
	Bridge1_VoltageH				                ,/* 0x0B */
	Bridge1_CurrentL				                ,/* 0x0C */
	Bridge1_CurrentH				                ,/* 0x0D */
	Bridge0_WorkMode				                ,/* 0x0E */
	Bridge1_WorkMode				                ,/* 0x0F */
	Port1_Protocol					                ,/* 0x10 */
	Port2_Protocol					                ,/* 0x11 */
	Port3_Protocol					                ,/* 0x12 */
	Port4_Protocol					                ,/* 0x13 */
	Port1_Chg_Power_Sp			                ,/* 0x14 */
	Port2_Chg_Power_Sp			                ,/* 0x15 */
	Port3_Chg_Power_Sp			                ,/* 0x16 */
	Port4_Chg_Power_Sp			                ,/* 0x17 */
	Port1_Charger_Power 		                ,/* 0x18 */
	Port2_Charger_Power 		                ,/* 0x19 */
	Port3_Charger_Power 		                ,/* 0x1A */
	Port4_Charger_Power 		                ,/* 0x1B */
	Port1_Source_CurL 			                ,/* 0x1C */
	Port1_Source_CurH 			                ,/* 0x1D */
	Port2_Source_CurH 			                ,/* 0x1E */
	Port2_Source_CurL 			                ,/* 0x1F */
	Port3_Source_CurH 			                ,/* 0x20 */
	Port3_Source_CurL 			                ,/* 0x21 */
	Port4_Source_CurH 			                ,/* 0x22 */
	Port4_Source_CurL 			                ,/* 0x23 */
	Ports_EN_State					                ,/* 0x24 */
	BatVol_Ntc_Status 			                ,/* 0x25 */
	Ntc_Temp								                ,/* 0x26 */
	Coulomb_Counter 				                ,/* 0x27 */
	Port_Ocp_State                          ,/* 0x28 */
	VBAT_VoltageL			                      ,/* 0x29 */
	VBAT_VoltageH			                      ,/* 0x2A */
	IBAT_CurrentL			                      ,/* 0x2B */
	IBAT_CurrentH			                      ,/* 0x2C */
	Chip_SWD_EN 						                ,/* 0x2D */
	Code_Version						                ,/* 0x2E */
  RCVD                                    ,/* 0x2F */		
	Port1_UFP_EN						                ,/* 0x30 */
	Port1_DFP_EN						                ,/* 0x31 */
	Port1_PD_OBJ_SEL				                ,/* 0x32 */
	Port1_Bridge_Num				                ,/* 0x33 */
	Port2_UFP_EN						                ,/* 0x34 */
	Port2_DFP_EN						                ,/* 0x35 */
	Port2_PD_OBJ_SEL				                ,/* 0x36 */
	Port2_Bridge_Num				                ,/* 0x37 */
	Port3_UFP_EN						                ,/* 0x38 */
	Port3_DFP_EN						                ,/* 0x39 */
	Port3_PD_OBJ_SEL				                ,/* 0x3A */
	Port3_Bridge_Num				                ,/* 0x3B */
	Port4_UFP_EN						                ,/* 0x3C */
	Port4_DFP_EN						                ,/* 0x3D */
	Port4_PD_OBJ_SEL				                ,/* 0x3E */
	Port4_Bridge_Num				                ,/* 0x3F */
	Bridge0_WorkPort_Num	                  ,/* 0x40 */
	Bridge1_WorkPort_Num	                  ,/* 0x41 */
	Bat_Status                              ,/* 0x42 */
	PortManage_Event			                  ,/* 0x43 */
	Key_Event_Deal				                  ,/* 0x44 */
	Sink_VID_L							                ,/* 0x45 */
	Sink_VID_H							                ,/* 0x46 */
	IIC_REG_SIZE,
} DSI_REG_ADDR;

typedef struct
{
	uint8_t			Port1_Mode						      ;/* 0x00 *//* ¶Á¼Ä´æÆ÷ */
	uint8_t			Port2_Mode						      ;/* 0x01 */
	uint8_t			Port3_Mode						      ;/* 0x02 */	
	uint8_t			Port4_Mode						      ;/* 0x03 */
	uint8_t			Bridge0_Abnormal_Status     ;/* 0x04 */
	uint8_t			Bridge1_Abnormal_Status     ;/* 0x05 */	
	uint16_t			Bridge0_Voltage			      ;/* 0x06 */
	//uint8_t			Bridge0_VoltageL			      ;/* 0x06 */
	//uint8_t			Bridge0_VoltageH			      ;/* 0x07 */
	uint16_t			Bridge0_Current			      ;/* 0x08 */
	//uint8_t			Bridge0_CurrentL			      ;/* 0x08 */
	//uint8_t			Bridge0_CurrentH			      ;/* 0x09 */
	uint16_t			Bridge1_Voltage			      ;/* 0x0A */
	//uint8_t			Bridge1_VoltageL			      ;/* 0x0A */
	//uint8_t			Bridge1_VoltageH			      ;/* 0x0B */
	uint16_t			Bridge1_Current			      ;/* 0x0C */
	//uint8_t			Bridge1_CurrentL			      ;/* 0x0C */
	//uint8_t			Bridge1_CurrentH			      ;/* 0x0D */
	uint8_t			Bridge0_WorkMode			      ;/* 0x0E */
	uint8_t			Bridge1_WorkMode			      ;/* 0x0F */	
	uint8_t			Port1_Protocol				      ;/* 0x10 */
	uint8_t			Port2_Protocol				      ;/* 0x11 */
	uint8_t			Port3_Protocol				      ;/* 0x12 */
	uint8_t			Port4_Protocol				      ;/* 0x13 */	
	uint8_t			Port1_Chg_Power_Sp		      ;/* 0x14 */
	uint8_t			Port2_Chg_Power_Sp		      ;/* 0x15 */
	uint8_t			Port3_Chg_Power_Sp		      ;/* 0x16 */
	uint8_t			Port4_Chg_Power_Sp		      ;/* 0x17 */		
	uint8_t			Port1_Charger_Power		      ;/* 0x18 */
	uint8_t			Port2_Charger_Power		      ;/* 0x19 */
	uint8_t			Port3_Charger_Power		      ;/* 0x1A */
	uint8_t			Port4_Charger_Power		      ;/* 0x1B */
	uint16_t			Port1_Source_Cur			      ;/* 0x1C */
	//uint8_t			Port1_Source_CurL			      ;/* 0x1C */
	//uint8_t			Port1_Source_CurH			      ;/* 0x1D */
	uint16_t			Port2_Source_Cur			      ;/* 0x1E */
	//uint8_t			Port2_Source_CurH			      ;/* 0x1E */
	//uint8_t     Port2_Source_CurL			      ;/* 0x1F */
	uint16_t			Port3_Source_Cur			      ;/* 0x20 */
	//uint8_t			Port3_Source_CurH			      ;/* 0x20 */
	//uint8_t     Port3_Source_CurL			      ;/* 0x21 */
	uint16_t			Port4_Source_Cur			      ;/* 0x22 */
	//uint8_t			Port4_Source_CurH			      ;/* 0x22 */
	//uint8_t     Port4_Source_CurL			      ;/* 0x23 */
	uint8_t     Ports_EN_State              ;/* 0x24 */	
	uint8_t			Ntc_Status                  ;/* 0x25 */	
	uint8_t     Ntc_Temp                    ;/* 0x26 *//* NTCÎÂ¶È 0~145 <====> -20~125¶È */
	uint8_t     Coulomb_Counter             ;/* 0x27 */
	uint8_t			Port_Ocp_State              ;/* 0x28 */	
	uint16_t			VBAT_Voltage			          ;/* 0x29 */
	//uint8_t			VBAT_VoltageL			          ;/* 0x29 */
	//uint8_t			VBAT_VoltageH			          ;/* 0x2A */	
	uint16_t     IBAT_Current               ;/* 0x2B */	
	//uint8_t     IBAT_CurrentL               ;/* 0x2B */	
	//uint8_t     IBAT_CurrentH               ;/* 0x2C */	
	uint8_t     Chip_SWD_EN          	      ;/* 0x2D */
	uint8_t     Code_Version                ;/* 0x2E */		
	uint8_t     RCVD                        ;/* 0x2F */		
	uint8_t			Port1_UFP_EN					      ;/* 0x30 *//* Ð´¼Ä´æÆ÷ */
	uint8_t			Port1_DFP_EN					      ;/* 0x31 */
	uint8_t			Port1_PD_OBJ_SEL			      ;/* 0x32 */
  uint8_t     Port1_Bridge_Num			      ;/* 0x33 */
	uint8_t			Port2_UFP_EN					      ;/* 0x34 */
	uint8_t			Port2_DFP_EN					      ;/* 0x35 */  
	uint8_t			Port2_PD_OBJ_SEL			      ;/* 0x36 */	
  uint8_t     Port2_Bridge_Num			      ;/* 0x37 */
	uint8_t     Port3_UFP_EN					      ;/* 0x38 */
	uint8_t     Port3_DFP_EN					      ;/* 0x39 */  
	uint8_t     Port3_PD_OBJ_SEL			      ;/* 0x3A */ 
	uint8_t     Port3_Bridge_Num			      ;/* 0x3B */ 	
	uint8_t     Port4_UFP_EN					      ;/* 0x3C */
	uint8_t     Port4_DFP_EN					      ;/* 0x3D */  
	uint8_t     Port4_PD_OBJ_SEL			      ;/* 0x3E */ 
	uint8_t     Port4_Bridge_Num			      ;/* 0x3F */
	uint8_t			Bridge0_WorkPort_Num	      ;/* 0x40 */
	uint8_t			Bridge1_WorkPort_Num	      ;/* 0x41 */	
	uint8_t			Bat_Status                  ;/* 0x42 */		
	uint8_t			PortManage_Event			      ;/* 0x43 */	
	uint8_t			Key_Event_Deal				      ;/* 0x44 */
	uint8_t			Sink_VID_L		              ;/* 0x45 */
	uint8_t			Sink_VID_H		              ;/* 0x46 */	
}DSI_REG_INFO;

#pragma pack()
extern DSI_REG_INFO gstDsiCmdInfo;
extern DSI_REG_INFO gstDsiRegInfo;

void g_dsi_init(void);
void g_dsi_handle_process(void);
void g_dsi_port_disable(uint8_t n);
uint8_t g_dsi_get_dfp_cfg(uint8_t n);
uint8_t g_dsi_get_ufp_cfg(uint8_t n);
uint8_t g_dsi_get_power_level(uint8_t n);
void g_dsi_set_power_level(uint8_t n, uint8_t u8PdoSel);
#endif

