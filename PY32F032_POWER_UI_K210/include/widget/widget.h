/*
 * widget.h
 *
 *  Created on: Nov 11, 2020
 *      Author: ZKSWE Develop Team
 */

#ifndef _WIDGET_WIDGET_H_
#define _WIDGET_WIDGET_H_

#include "context/message.h"
#include "misc/rect.h"
#include "graphics/gfx.h"

#define UI_REG_SIZE         1
#define UI_REG_BASE_ADDR    0
#define UI_REG_MKWORD16(x)  ((uint16_t) (((uint16_t) x << 8) | (uint8_t) (x >> 8)))
#define UI_REG_VAL(reg)     g_ui_regdata[reg - UI_REG_BASE_ADDR]

extern uint16_t g_ui_regdata[UI_REG_SIZE];

/**
 * @brief 控件类型
 */
typedef enum {
	E_WIDGET_IMAGE,
	E_WIDGET_LABEL,
	E_WIDGET_ANIMATION,
	E_WIDGET_BUTTON,
	E_WIDGET_PROGRESS,
	E_WIDGET_IMAGETEXT,
	E_WIDGET_QRCODE,
	E_WIDGET_INCREMENT,
	E_WIDGET_NUMTEXT,
	E_WIDGET_RADIO,
	E_WIDGET_LIST,
	E_WIDGET_WAVE,
	E_WIDGET_WINDOW,
} widget_type_e;

/**
 * @brief 图像填充类型，颜色、图片二选一
 */
typedef union {
	uint16_t color;
	uint16_t imgid;
} graph_t;

#define WIDGET_FLAG_INVALIDATE      0x0001
#define WIDGET_FLAG_VISIBLE         0x0002
#define WIDGET_FLAG_TOUCHABLE       0x0004
#define WIDGET_FLAG_PRESSED         0x0008
#define WIDGET_FLAG_TIMER           0x0010
#define WIDGET_FLAG_CLIP_REFRESH    0x0100
#define WIDGET_FLAG_FILL_IMG        0x0200

#define WIDGET_HAS_FLAG(w, f)       (w->flag & f)
#define WIDGET_SET_FLAG(w, f)       w->flag |= f
#define WIDGET_CLEAR_FLAG(w, f)     w->flag &= ~f

#define WIDGET_COMMON	\
	uint8_t id; \
	uint8_t type; \
	uint16_t regid; \
	uint16_t flag; \
	graph_t bg; \
	rect_t pos;

typedef struct {
	WIDGET_COMMON
} widget_t;

typedef struct {
	uint8_t type;
	int (*proc_fun)(widget_t *widget, const msg_t *m);
} widget_proc_fun_t;

void widget_draw_graph(widget_t *widget, const graph_t *graph);

/**
 * @brief 设置控件背景图
 * @param widget 控件指针
 * @param imgid  图片id
 */
void widget_set_bg_img(widget_t *widget, uint16_t imgid);

/**
 * @brief 设置控件背景色
 * @param widget 控件指针
 * @param color  颜色值 RGB565色
 */
void widget_set_bg_color(widget_t *widget, uint16_t color);

/**
 * @brief 请求控件重绘
 * @param widget 控件指针
 * @param dirty  脏区
 */
void widget_invalidate(widget_t *widget, const rect_t *dirty);

bool widget_is_touch_hit(widget_t *widget, int16_t x, int16_t y);
int widget_proc(widget_t *widget, const msg_t *m);

#endif /* _WIDGET_WIDGET_H_ */
