#ifndef __POWER_MANAGER__H___
#define __POWER_MANAGER__H___
#include "main.h"

#define SP8630 0
#define SP8612 1
#define SP8650 2
#define SP8610 3
#define SW2302 4
#define HUSB385A 5

#define DEVICE_TYPE SP8612

#if (SP8610 == DEVICE_TYPE)
#include "SP8610.h"
#elif (SP8612 == DEVICE_TYPE || SP8630 == DEVICE_TYPE)
#include "SP8630_SP8612.h"
#elif (SP8650 == DEVICE_TYPE)
#include "SP8650.h"
#elif (SW2302 == DEVICE_TYPE)
#include "SW2302.h"
#elif (HUSB385A == DEVICE_TYPE)
#include "husb385a.h"
#endif


typedef enum
{
	TYPE_C = 'C',
	TYPE_A = 'A',
} PORT_TYPE_T;

typedef struct
{
	uint8_t enabled; // 0: disable; 1: enable;
	uint8_t lastenabled;
	PORT_TYPE_T type;
	uint8_t idx;
	uint16_t current; // 0.1a
	uint16_t current_filtered;
	uint16_t voltage; // 0.1v
	uint16_t power;   // 1w
#if DEVICE_TYPE == SP8630 || DEVICE_TYPE == SP8612 || DEVICE_TYPE == HUSB385A
	ENUM_PROTOCOL_MODE protocol;
#endif
} port_stat_t;

typedef enum
{
	IDLE_STAT = 0,
	OUTPUT_STAT = 1,
	INPUT_STAT = 2,
	BYPASS_STAT = OUTPUT_STAT|INPUT_STAT,
	TRICKLE_STAT = 4,
} WORK_STAT_T;

#define PORT_4C  0
#define PORT_3C1A 1
#define PORT_2C2A 2
#define PORT_3C   3
#define PORT_2C1A 4
#define PORT_1C1A 5
#define PORT_2C   6
#define PORT_1C1AC	7
#ifdef P3C1A
#define PORT_TYPE PORT_3C1A
#elif defined(P2C1A)
#define PORT_TYPE PORT_2C1A
#elif defined(P4C)
#define PORT_TYPE PORT_4C
#elif defined(P3C)
#define PORT_TYPE PORT_3C
#endif
#if PORT_TYPE == PORT_4C || PORT_TYPE == PORT_3C1A || PORT_TYPE == PORT_2C2A
#define PORT_NUM 4
#elif PORT_TYPE == PORT_3C || PORT_TYPE == PORT_2C1A || PORT_TYPE == PORT_1C1AC
#define PORT_NUM 3
#elif PORT_TYPE == PORT_2C || PORT_TYPE == PORT_1C1A
#define PORT_NUM 2
#endif

extern port_stat_t ports[];
extern i2c_reg_t device;
extern uint8_t recived_new;

void update_port_status(void); // run in time interval
void pm_process(void); // run in main loop
void pm_init(void);

#endif
