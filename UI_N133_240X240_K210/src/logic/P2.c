#include "logic/P2.h"
#include "ui_page.h"
#include "ui_res_image.h"
#include "pm.h"


static uint8_t bgs[4] = {0};

static char number2w(uint16_t total, char* str)
{
	uint8_t idx = 0;
	if (total == 0)
	{
		str[idx++] = 'x';
		str[idx++] = 'x';
		str[idx++] = 'x';
		str[idx++] = 'y';
		str[idx++] = 0;
	}
	else
	{
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
			str[0] = 'h';
			str[1] = '0' + (total / 10) % 10;
			str[2] = '0' + total % 10;
			str[3] = 'w';
			str[4] = 'h';
			str[5] = 0;
		}
		else
		{
			str[0] = ' ';
			str[1] = '0' + total;
			str[2] = 'w';
			str[3] = ' ';
			str[4] = 0;
		}
	}
	return idx;
}

static void port2av(port_stat_t port, char* str)
{
	uint8_t idx = 0;
	if (port.power == 0)
	{
		//zzyzvwzyza
		str[idx++] = 'z';
		str[idx++] = 'z';
		str[idx++] = 'y';
		str[idx++] = 'z';
		str[idx++] = 'v';
		str[idx++] = 'w';
		str[idx++] = 'z';
		str[idx++] = 'y';
		str[idx++] = 'z';
		str[idx++] = 'a';
		str[idx++] = 0;
		return;
	}
	str[idx++] = port.voltage >= 1000? '0'+((port.voltage/1000)%10) : ' ';
	str[idx++] = (port.voltage / 100) %10 + '0';
	str[idx++] = '.';
	str[idx++] = (port.voltage / 10) %10 + '0';
	str[idx++] = 'V';
	str[idx++] = '/';
	//str[idx++] = port.current >= 1000? '0'+((port.current/1000)%10) : '0';
	str[idx++] = (port.current / 100) %10 + '0';
	str[idx++] = '.';
	str[idx++] = (port.current / 10) %10 + '0';
	str[idx++] = 'A';
	str[idx++] = 0;
}

static void update_p(void)
{
	int32_t total = 0;
	uint8_t str[7] = {0};
	uint8_t i;
	for (i = 0; i < 4; i++)
	{
		if (ports[i].enabled)
		{
			total += ports[i].power;
		}
		LOG("port %d: %d %d %d\n", i, ports[i].voltage, ports[i].current, ports[i].power);
	}

	// C1
	if (ports[0].enabled)
	{
		if (!bgs[0]) widget_set_bg_img((widget_t*)UI_WIDGET_P2_c1, UI_RES_IMAGE_P2_C1);
		bgs[0] = 1;
		number2w(ports[0].power, (char*)str);
		imagetext_set_text(UI_WIDGET_P2_w1, (const char*)str);
		port2av(ports[0], str);
		imagetext_set_text(UI_WIDGET_P2_a1, (const char*)str);
	}
	else if (bgs[0])
	{
		ports[0].power = 0;
		widget_set_bg_img((widget_t*)UI_WIDGET_P2_c1, UI_RES_IMAGE_P2_C1G1);
		bgs[0] = 0;
		number2w(0, (char*)str);
		imagetext_set_text(UI_WIDGET_P2_w1, (const char*)str);
		port2av(ports[0], str);
		imagetext_set_text(UI_WIDGET_P2_a1, (const char*)str);
	}
	// C2
	if (ports[1].enabled)
	{
		if (!bgs[1]) widget_set_bg_img((widget_t*)UI_WIDGET_P2_c2, UI_RES_IMAGE_P2_C2);
		bgs[1] = 1;
		number2w(ports[1].power, (char*)str);
		imagetext_set_text(UI_WIDGET_P2_w2, (const char*)str);
		port2av(ports[1], str);
		imagetext_set_text(UI_WIDGET_P2_a2, (const char*)str);
	}
	else if (bgs[1])
	{
		ports[1].power = 0;
		widget_set_bg_img((widget_t*)UI_WIDGET_P2_c2, UI_RES_IMAGE_P2_C2G2);
		bgs[1] = 0;
		number2w(0, (char*)str);
		imagetext_set_text(UI_WIDGET_P2_w2, (const char*)str);
		port2av(ports[1], str);
		imagetext_set_text(UI_WIDGET_P2_a2, (const char*)str);
	}
	// C3
	if (ports[2].enabled)
	{
		if (!bgs[2]) widget_set_bg_img((widget_t*)UI_WIDGET_P2_c3, UI_RES_IMAGE_P2_C3);
		bgs[2] = 1;
		number2w(ports[2].power, (char*)str);
		imagetext_set_text(UI_WIDGET_P2_w3, (const char*)str);
		port2av(ports[2], str);
		imagetext_set_text(UI_WIDGET_P2_a3, (const char*)str);
	}
	else if (bgs[2])
	{
		ports[2].power = 0;
		widget_set_bg_img((widget_t*)UI_WIDGET_P2_c3, UI_RES_IMAGE_P2_C3G3);
		bgs[2] = 0;
		number2w(0, (char*)str);
		imagetext_set_text(UI_WIDGET_P2_w3, (const char*)str);
		port2av(ports[2], str);
		imagetext_set_text(UI_WIDGET_P2_a3, (const char*)str);
	}
	// A
	if (ports[3].enabled && ports[3].power != 0)
	{
		if (!bgs[3]) widget_set_bg_img((widget_t*)UI_WIDGET_P2_c4, UI_RES_IMAGE_P2_A);
		bgs[3] = 1;
		number2w(ports[3].power, (char*)str);
		imagetext_set_text(UI_WIDGET_P2_w4, (const char*)str);
		port2av(ports[3], str);
		imagetext_set_text(UI_WIDGET_P2_a4, (const char*)str);
	}
	else if (bgs[3])
	{
		ports[3].power = 0;
		widget_set_bg_img((widget_t*)UI_WIDGET_P2_c4, UI_RES_IMAGE_P2_AG1);
		bgs[3] = 0;
		number2w(0, (char*)str);
		imagetext_set_text(UI_WIDGET_P2_w4, (const char*)str);
		port2av(ports[3], str);
		imagetext_set_text(UI_WIDGET_P2_a4, (const char*)str);
	}
}
void ui_P2_on_page_open(uint8_t page) {
	bgs[0] = bgs[1] = bgs[2] = bgs[3] = 0;
	update_p();
}

/**
 * @brief 定时器回调接口
 * @param millisecond   定时器周期，单位 ms
 * @param count  累计次数
 */
void ui_P2_on_timer(uint32_t millisecond, uint32_t count) {


	static uint8_t time = 0;
	if (time++<80) return;
	time = 0;
	update_p();
}

/**
 * @brief 按钮点击事件回调接口
 * @param button   按钮控件指针
 * @param type     触摸事件类型
 */
void ui_P2_on_button_touch_event(button_t *button, touch_type_e type) {

}

/**
 * @brief 进度条回调接口
 * @param progress  进度条控件指针
 * @param val       进度值
 */
void ui_P2_on_progress_changed(progress_t *progress, uint16_t val) {

}

/**
 * @brief 动画播放结束回调接口
 * @param animation  动画控件指针
 */
void ui_P2_on_animation_play_end(animation_t *animation) {

}
