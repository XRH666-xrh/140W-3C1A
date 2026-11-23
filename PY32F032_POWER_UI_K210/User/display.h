#ifndef __UI_DISPLAY_H__
#define __UI_DISPLAY_H__
#include "main.h"

typedef enum
{
	PAGE_OFF,
	PAGE_ANIMATING,
	PAGE_ANIMATION_END,
	PAGE_ANIMATION_OFF,
} ANIMATING_Stat;


typedef enum
{
	POWER_UP,
	POWER_RUNNING,
	POWER_IDLE,
	POWER_FORCE_OFF,
} power_stat_t;
extern power_stat_t power_stat;

#pragma anon_unions
typedef struct
{
	uint8_t eng;
	uint8_t upside;
	uint8_t temperature;
} config_t;

extern config_t config;

void update_page(uint8_t pageid);
void update_display(uint32_t millisecond);
void display_init(void);

#endif
