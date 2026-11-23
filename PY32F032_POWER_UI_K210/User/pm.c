#include "pm.h"
#include "iic_soft.h"

#if DEVICE_TYPE == SP8612 || DEVICE_TYPE == SP8630
uart_recive_stat_t sp =
{
	.rb = &rb2,
	.hinit = &initUart
};
#endif

port_stat_t ports[PORT_NUM] =
{
	{.type = TYPE_C, .idx = '1', .enabled = 0},
#if PORT_TYPE == PORT_1C1A
	{.type = TYPE_A, .idx = '1', .enabled = 0},
#else
	{.type = TYPE_C, .idx = '2', .enabled = 0},
#endif
#if PORT_TYPE == PORT_3C1A || PORT_TYPE == PORT_4C || PORT_TYPE == PORT_3C
	{.type = TYPE_C, .idx = '3', .enabled = 0},
#endif
#if PORT_TYPE == PORT_2C2A || PORT_TYPE == PORT_2C1A || PORT_TYPE == PORT_3C1A || PORT_TYPE == PORT_1C1AC
	{.type = TYPE_A, .idx = '1', .enabled = 0},
#endif
#if PORT_TYPE == PORT_4C
	{.type = TYPE_C, .idx = '4', .enabled = 0},
#elif PORT_TYPE == PORT_2C2A
	{.type = TYPE_A, .idx = '2', .enabled = 0},
#endif
};
uint16_t filters[PORT_NUM][17] = {0};

void update_port_status(void)
{
#if SP8610 == DEVICE_TYPE
	ports[0].enabled = SP8610_getPortStatus();
	ports[0].current = SP8610_get_port_cur();
	ports[0].voltage = SP8610_get_port_vol();
	ports[0].power = ports[0].current * ports[0].voltage / 100;
#elif SP8612 == DEVICE_TYPE || SP8630 == DEVICE_TYPE || SP8650 == DEVICE_TYPE
	static uint8_t idle_cnt = 0;
	uint16_t current = 0;
	#define IDLE_TIMEOUT 20  //20*100MS = 2S TIMEOUT
	if (sp.is_receive_newdata)
	{
		idle_cnt = 0;
		sp.is_receive_newdata = 0;
		ports[0].enabled = device.reg.Port1_Mode;
		ports[0].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port1_Protocol;
		current = (device.reg.Bridge0_Current)/10;
		if (current <=10) current = 0;
		ports[0].current = (current + filters[0][0] - filters[0][1]);
		for (uint8_t i = 1; i < 8; i++)
		{
			filters[0][i] = filters[0][i+1];
		}
		filters[0][0] = ports[0].current;
		filters[0][8] = current;
		ports[0].current >>= 3;
		ports[0].voltage = (device.reg.Bridge0_Voltage)/10;
		ports[0].power = ports[0].current * ports[0].voltage / 10000;
#if PORT_TYPE == PORT_1C1A
		ports[1].enabled = device.reg.Port3_Mode;
		ports[1].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port3_Protocol;
		ports[1].current = (device.reg.Bridge2_Current)/10;
		ports[1].voltage = (device.reg.Bridge2_Voltage)/10;
		ports[1].power = ports[1].current * ports[1].voltage / 10000;
#elif (PORT_1C1AC == PORT_TYPE)
		ports[1].enabled = device.reg.Port2_Mode;
		ports[2].enabled = device.reg.Port3_Mode;
		if (ports[1].enabled)
		{
			if (ports[2].enabled)
			{
				ports[1].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port2_Protocol;
				ports[1].current = (device.reg.Bridge1_Current)/10/2;
				ports[1].voltage = (device.reg.Bridge1_Voltage)/10;
				ports[1].power = ports[1].current * ports[1].voltage / 10000;
				ports[2].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port2_Protocol;
				ports[2].current = (device.reg.Bridge1_Current)/10/2;
				ports[2].voltage = (device.reg.Bridge1_Voltage)/10;
				ports[2].power = ports[2].current * ports[2].voltage / 10000;
			}
			else
			{
				ports[1].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port2_Protocol;
				ports[1].current = (device.reg.Bridge1_Current)/10;
				ports[1].voltage = (device.reg.Bridge1_Voltage)/10;
				ports[1].power = ports[1].current * ports[1].voltage / 10000;
				ports[2].current = 0;
			}
		}
		else if (ports[2].enabled)
		{
			ports[2].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port2_Protocol;
			ports[2].current = (device.reg.Bridge1_Current)/10;
			ports[2].voltage = (device.reg.Bridge1_Voltage)/10;
			ports[2].power = ports[2].current * ports[2].voltage / 10000;
			ports[1].current = 0;
		}
		else
		{
			ports[1].current = 0;
			ports[2].current = 0;
		}
#else
		ports[1].enabled = device.reg.Port2_Mode;
		ports[1].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port2_Protocol;
		ports[1].voltage = (device.reg.Bridge1_Voltage)/10;
		current = (device.reg.Bridge1_Current)/10;
		if (current <=10) current = 0;
		ports[1].current = (current + filters[1][0] - filters[1][1]);
		for (uint8_t i = 1; i < 8; i++)
		{
			filters[1][i] = filters[1][i+1];
		}
		filters[1][0] = ports[1].current;
		filters[1][8] = current;
		ports[1].current >>= 3;
		ports[1].power = ports[1].current * ports[1].voltage / 10000;
#if PORT_NUM >=3
		ports[2].enabled = device.reg.Port3_Mode;
		ports[2].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port3_Protocol;
		ports[2].current = (device.reg.Port3_Source_Cur)/10;
		ports[2].voltage = (device.reg.Bridge2_Voltage)/10;
		if (ports[2].current <=12) ports[2].current = 0;
		else ports[2].current -= 10;
		ports[2].power = ports[2].current * ports[2].voltage / 10000;
#endif
#endif
#if PORT_NUM == 4
		ports[3].enabled = device.reg.Port4_Mode;
		ports[3].protocol = (ENUM_PROTOCOL_MODE)device.reg.Port4_Protocol;
		ports[3].current = (device.reg.Port4_Source_Cur)/10;
		ports[3].voltage = (device.reg.Bridge3_Voltage)/10;
		ports[3].power = ports[3].current * ports[3].voltage / 10000;
#endif
	}
	else
	{
		idle_cnt++;
		if (idle_cnt >= IDLE_TIMEOUT)
		{
			LOG("ERROR: NO POWER DATA RECIVED IN TIME \r\n");
			idle_cnt = 0;
			ports[0].enabled = 0;
			ports[1].enabled = 0;
#if PORT_NUM >=3
			ports[2].enabled = 0;
#endif
#if PORT_NUM == 4
			ports[3].enabled = 0;
#endif
		}
	}
#else
	uint8_t i;
	for (i = 0; i<PORT_NUM; i++)
	{
		ports[i].current++;
		ports[i].voltage++;
		ports[i].power = ports[i].current * ports[i].voltage / 100;
	}
#endif
}

// run in main process
inline void pm_process(void)
{
#if DEVICE_TYPE == SP8612 || DEVICE_TYPE == SP8630
	sp86xx_recive_process(&sp);
#endif
}

void pm_init(void)
{
#if SW2302 == DEVICE_TYPE
	sw2302_init();
#elif SP8610 == DEVICE_TYPE
#elif DEVICE_TYPE == SP8612 || DEVICE_TYPE == SP8630	
	sp86xx_init(&sp);
#elif DEVICE_TYPE == HUSB385A
	husb385a_init();
#endif
}
