#include "logic/P4.h"
#include "ui_page.h"
#include "ui_res_image.h"
#include "pm.h"
#include "display.h"

static void update_p(void)
{
	progress_set_value(UI_WIDGET_P4_prog, device.reg.Ntc_Temp);
}

static void update_prog(void)
{
	widget_set_bg_img((widget_t*)UI_WIDGET_P4_bg, (config.eng)?UI_RES_IMAGE_P3_EN: UI_RES_IMAGE_P3_CN);
}

void ui_P4_on_page_open(uint8_t page) {

	update_prog();
	update_p();
}

/**
 * @brief 定时器回调接口
 * @param millisecond   定时器周期，单位 ms
 * @param count  累计次数
 */
void ui_P4_on_timer(uint32_t millisecond, uint32_t count) {

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
void ui_P4_on_button_touch_event(button_t *button, touch_type_e type) {

}

/**
 * @brief 进度条回调接口
 * @param progress  进度条控件指针
 * @param val       进度值
 */
void ui_P4_on_progress_changed(progress_t *progress, uint16_t val) {

}

/**
 * @brief 动画播放结束回调接口
 * @param animation  动画控件指针
 */
void ui_P4_on_animation_play_end(animation_t *animation) {

}
