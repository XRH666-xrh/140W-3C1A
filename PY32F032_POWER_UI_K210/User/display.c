#include "display.h"
#include "context/guicontext.h"
#include "logic/insert.h"
#include "logic/p1.h"
#include "ui_page.h"
#include "ui_res_image.h"
#include "pm.h"
#include "key.h"
#include "flash.h"
#include "video.h"
#include "usermain.h"

#define PAGE_FLASH_INTERVAL  100
#define PAGE_DISPLAY_INTERVAL 800

#define IDLE_SLEEP_TIME (60*1000)
#define IDLE_RUN_START_TIME (60*1000)

enum
{
	PORT_IDLE,
	PORT_WORKING,
	PORT_INSERT,
};

power_stat_t power_stat = POWER_UP;

static uint32_t idle_time = 0;
static uint32_t run_time = 0;
static uint8_t current_page_id = 0;
uint8_t last_page = UI_PAGE_P1;
config_t config;

void open_powerup(void)
{
	video_init();
	gui_context_open_page(UI_PAGE_POWERUP);
	gui_context_run();
}

void open_poweroff(void)
{
	gui_context_open_page(UI_PAGE_POWEROFF);
	gui_context_run();
}

void save_config(void)
{
	uint8_t dat = 0x80 | config.eng<<2 | config.upside;
	FlashUpdateByteConfig(dat);
}

void key_handler(void)
{
	get_key_event();
	if (keys[0].key_value == KEY_SHORT_PRES_RELESE)
	{
		idle_time = 0;
		run_time = 0;
		if (power_stat != POWER_UP)
		{
			if (power_stat == POWER_IDLE)
			{
				video_init();
				gui_context_open_page(UI_PAGE_P1);
			}
			else
			{
				gui_context_open_page(last_page);
			}
			power_stat = POWER_UP;
			return;
		}
		current_page_id = gui_context_get_current_page();

		if (current_page_id == UI_PAGE_POWERUP)
		{
			gui_context_open_page(UI_PAGE_P1);
		}
		else if (current_page_id == UI_PAGE_RUN || current_page_id == UI_PAGE_POWEROFF)
		{
			gui_context_open_page(last_page);
		}
		else if (current_page_id == UI_PAGE_P1)
		{
			gui_context_open_page(UI_PAGE_P2);
		}
		else if (current_page_id == UI_PAGE_P2)
		{
			gui_context_open_page(UI_PAGE_P3);
		}
		else if (current_page_id == UI_PAGE_P3)
		{
			gui_context_open_page(UI_PAGE_P4);
		}
		else if (current_page_id == UI_PAGE_P4)
		{
			gui_context_open_page(UI_PAGE_P1);
		}
	}
	else if (keys[0].key_value == KEY_LONG_PRES)
	{
#ifndef EN_ONLY
		idle_time = 0;
		run_time = 0;
		config.eng = !config.eng;
		save_config();
		uint8_t page = gui_context_get_current_page();
		if (page == UI_PAGE_P2)
		{
			gui_context_open_page(UI_PAGE_P3);
		}
#endif
	}
	else if (keys[0].key_value == KEY_DOUBLE_CLICK)
	{
		idle_time = 0;
		run_time = 0;
		config.upside++;
		if (config.upside > 3) config.upside = 0;
		video_rotate(config.upside);
		rotate_ui(config.upside);
		save_config();
		uint8_t page = gui_context_get_current_page();
		gui_context_open_page(page);
	}
}

void check_idle(uint32_t millisecond)
{
	uint8_t i, idlestat = PORT_IDLE;
	for (i = 0; i<PORT_NUM; i++)
	{
		if (ports[i].enabled && ports[i].power>0)
		{
			idle_time = 0;
			if (!ports[i].lastenabled)
			{
				idlestat = PORT_INSERT;
			}
			else if (idlestat != PORT_INSERT)
			{
				idlestat = PORT_WORKING;
			}
			ports[i].lastenabled = 5;
		}
		else if (ports[i].lastenabled)
		{
			ports[i].lastenabled--;
		}
	}
	if (idlestat == PORT_INSERT)
	{
		run_time = 0;
		if (power_stat == POWER_IDLE)
		{
			video_init();
		}
		power_stat = POWER_UP;
		gui_context_open_page(UI_PAGE_P3);
	}
	else if (idlestat == PORT_IDLE)
	{
		if (idle_time < IDLE_SLEEP_TIME) idle_time += millisecond;
		if (run_time >= IDLE_RUN_START_TIME && power_stat == POWER_RUNNING)
		{
			gui_context_open_page(UI_PAGE_POWEROFF);
		}
		run_time = 0;
	}

	if (run_time < IDLE_RUN_START_TIME && idlestat != PORT_IDLE) run_time += millisecond;

	if (idle_time >= IDLE_SLEEP_TIME && power_stat == POWER_UP)
	{
		last_page = gui_context_get_current_page();
		gui_context_open_page(UI_PAGE_POWEROFF);
		power_stat = POWER_RUNNING;
	}
	else if (run_time >= IDLE_RUN_START_TIME && power_stat == POWER_UP)
	{
		last_page = gui_context_get_current_page();
		gui_context_open_page(UI_PAGE_RUN);
		power_stat = POWER_RUNNING;
	}
}

void display_init(void)
{
	uint8_t dat = FlashReadByteConfig();
	if (dat == 0xff)
	{
		config.eng = 0;
		config.upside = 0;
	}
	else
	{
		config.eng = (dat & (1<<2)) ? 1: 0;
		config.upside = dat & 0x03;
	}
#ifdef EN_ONLY
	config.eng = 1;
#endif
	if (config.upside)
	{
		video_rotate(config.upside);
		rotate_ui(config.upside);
	}
	config.temperature = 0;
}

void update_display(uint32_t millisecond)
{
	static uint32_t on_time = 0;

	//LOG("%d %d, %d\n", millisecond, count, keys[0].key_value);
	on_time += millisecond;
	key_handler();
	if (on_time < PAGE_FLASH_INTERVAL) return;
	on_time = 0;
	update_port_status();
	check_idle(PAGE_FLASH_INTERVAL);
}
