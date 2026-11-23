#include "logic/P3.h"
#include "ui_page.h"
#include "ui_res_image.h"
#include "display.h"
#include "pm.h"

static uint8_t lasten = 0;
static uint8_t lastan = 0;
int32_t total = 0;
static void update_p(void)
{
	uint8_t str[7] = {0};
	uint8_t i;
	total = 0;
	for (i = 0; i < 4; i++)
	{
		if (ports[i].enabled)
		{
			total += ports[i].power;
		}
		//LOG("port %d: %d %d %d\n", i, ports[i].voltage, ports[i].current, ports[i].power);
	}
	if (total >=100)
	{
		str[0] = '0' + (total / 100) % 10;
		str[1] = '0' + (total / 10) % 10;
		str[2] = '0' + total % 10;
		str[3] = 'w';
		str[4] = 0;
	}
	else if (total >= 10)
	{
		str[0] = '0' + (total / 10) % 10;
		str[1] = '0' + total % 10;
		str[2] = 'w';
		str[3] = ' ';
		str[4] = 0;
	}
	else
	{
		str[0] = '0' + total % 10;
		str[1] = 'w';
		str[2] = ' ';
		str[3] = ' ';
		str[4] = 0;
	}
	imagetext_set_text(UI_WIDGET_P3_p, (const char*)str);
}

static void update_prog(void)
{
	if (config.eng && !lasten)
	{
		widget_set_bg_img((widget_t*)UI_WIDGET_P3_BG, UI_RES_IMAGE_P4_EN);
		lasten = 1;
		update_p();
	}
	else if (!config.eng && lasten)
	{
		widget_set_bg_img((widget_t*)UI_WIDGET_P3_BG, UI_RES_IMAGE_P4_CN);
		lasten = 0;
		update_p();
	}
	if (total)
	{
		lastan++;
		if (lastan > 10) lastan = 0;
		uint8_t an = UI_RES_IMAGE_P4_CN_00;
		if (config.eng) an = UI_RES_IMAGE_P4_EN_00;
		an += lastan;
		widget_set_bg_img((widget_t*)UI_WIDGET_P3_prog, an);
	}
	else
	{
		widget_set_bg_img((widget_t*)UI_WIDGET_P3_prog, (config.eng)?UI_RES_IMAGE_P4_EN_00: UI_RES_IMAGE_P4_CN_00);
	}
}

void ui_P3_on_page_open(uint8_t page) {
	lastan = 0;
	lasten = 0;
	update_p();
	update_prog();
}

/**
 * @brief 定时器回调接口
 * @param millisecond   定时器周期，单位 ms
 * @param count  累计次数
 */
void ui_P3_on_timer(uint32_t millisecond, uint32_t count) {

	static uint8_t time = 0;
	static uint8_t time_p = 0;
	if (time++>=100)
	{
		time = 0;
		update_p();
	}
	if (time_p++>=10)
	{
		time_p = 0;
		update_prog();
	}
}

/**
 * @brief 按钮点击事件回调接口
 * @param button   按钮控件指针
 * @param type     触摸事件类型
 */
void ui_P3_on_button_touch_event(button_t *button, touch_type_e type) {

}

/**
 * @brief 进度条回调接口
 * @param progress  进度条控件指针
 * @param val       进度值
 */
void ui_P3_on_progress_changed(progress_t *progress, uint16_t val) {

}

/**
 * @brief 动画播放结束回调接口
 * @param animation  动画控件指针
 */
void ui_P3_on_animation_play_end(animation_t *animation) {

}
