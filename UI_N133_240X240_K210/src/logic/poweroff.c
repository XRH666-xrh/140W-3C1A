#include "logic/poweroff.h"
#include "ui_page.h"
#include "ui_res_image.h"
#include "display.h"

#define EXIT_TIMEOUT 10*1000
uint8_t pageoff_stat;
static uint32_t timeout = 0;

void ui_poweroff_on_page_open(uint8_t page) {
	pageoff_stat = PAGE_ANIMATING;
	timeout = 0;
}

/**
 * @brief 定时器回调接口
 * @param millisecond   定时器周期，单位 ms
 * @param count  累计次数
 */
void ui_poweroff_on_timer(uint32_t millisecond, uint32_t count) {
	if (timeout >= EXIT_TIMEOUT)
	{
		if (pageoff_stat == PAGE_ANIMATING)
		{
			video_deinit();
			power_stat = POWER_IDLE;
		}
	}
	else timeout += millisecond;
}

/**
 * @brief 按钮点击事件回调接口
 * @param button   按钮控件指针
 * @param type     触摸事件类型
 */
void ui_poweroff_on_button_touch_event(button_t *button, touch_type_e type) {

}

/**
 * @brief 进度条回调接口
 * @param progress  进度条控件指针
 * @param val       进度值
 */
void ui_poweroff_on_progress_changed(progress_t *progress, uint16_t val) {

}

/**
 * @brief 动画播放结束回调接口
 * @param animation  动画控件指针
 */
void ui_poweroff_on_animation_play_end(animation_t *animation) {

}
