/*
 * gfx.h
 *
 *  Created on: Nov 11, 2020
 *      Author: ZKSWE Develop Team
 */

#ifndef _GRAPHICS_GFX_H_
#define _GRAPHICS_GFX_H_

#include "misc/rect.h"

#define GL_RGB_R(x)              ((x&0xF800) >> 8)
#define GL_RGB_G(x)              ((x&0x07F0) >> 3)
#define GL_RGB_B(x)              ((x&0x001F) << 3)
#define GL_ARGB_A(x)             (x != COLOR_KEY)

#define GL_RGB(r,g,b)            (((r&0xF8)<<8)|((g&0xFC)<<3)|((b&0xF8)>>3))

#define COLOR_KEY                0x00
#define BG_COLOR                 0x00

#define COLOR_WHITE              0xFFFF
#define COLOR_BLACK              0x0000
#define COLOR_BLUE               0x001F
#define COLOR_BRED               0XF81F
#define COLOR_GRED               0XFFE0
#define COLOR_GBLUE              0X07FF
#define COLOR_RED                0xF800
#define COLOR_MAGENTA            0xF81F
#define COLOR_GREEN              0x07E0
#define COLOR_CYAN               0x7FFF
#define COLOR_YELLOW             0xFFE0
#define COLOR_BROWN              0XBC40
#define COLOR_BRRED              0XFC07
#define COLOR_GRAY               0X8430

#define INVALID_IMG_ID           0xFFFF

typedef struct {
	uint16_t width;
	uint16_t height;

	uint32_t compressed_data_offset;
	uint32_t compressed_data_size;
	
	uint32_t color_table_offset;
	uint32_t color_table_size;

	uint8_t depth;
	uint8_t reserve[3];
} __attribute__ ((packed)) bmp_info_t;

typedef struct {
	/**
	 * @brief ??
	 * @param x   x???
	 * @param y   y???
	 * @param rgb ???
	 */
	void (*draw_pixel)(int16_t x, int16_t y, uint16_t rgb);

	/**
	 * @brief ????
	 * @param rc  ????
	 * @param rgb ???
	 */
	void (*fill_rect)(const rect_t *rc, uint16_t rgb);

	/**
	 * @brief ??bmp
	 * @param x   x???
	 * @param y   y???
	 * @param rc  ??bmp?????,?NULL?????
	 * @param bmpid bmp??id?
	 */
	void (*fill_bmp)(int16_t x, int16_t y, const rect_t *rc, uint16_t bmpid);

	/**
	 * @brief ??bmp?????
	 * @param x   x???
	 * @param y   y???
	 * @param rc  ??bmp?????,?NULL?????
	 * @param bmpid bmp??id?
	 * @param colorkey ???
	 */
	void (*fill_bmp_with_colorkey)(int16_t x, int16_t y, const rect_t *rc, uint16_t bmpid, uint16_t colorkey);
} gfx_op_t;

extern const gfx_op_t g_gfx_op;

/***
 * ????LCD??????????????????????????????????????????????
 * */
typedef struct {
	/**
	 * ??????????2a,2b,2c, cs_low
	 * LCD ????????????????????CS??LOW
	 */
	void (*lcd_start_rectfill)(int16_t x, int16_t y,int16_t w, int16_t h);
	/**
	 * LCD??????????????????????????????????????
	 */
	void (*lcd_piex_fill)(uint16_t rgb);
	/**
	 * cs ????????????????????????
	 */
	void (*lcd_end_rectfill)();

	void (*lcd_fill_rect)(int16_t x, int16_t y,int16_t w, int16_t h,uint16_t rgb);
}gfx_cpulcd;

extern const gfx_cpulcd* mlcd_gfx;
void gfx_set_cpulcd(const gfx_cpulcd* lcd);

typedef struct{
	bmp_info_t info;
	uint32_t* bmpData;
	
}zk_bmp_data;


void fill_bmp_zkbmpdata(int16_t x, int16_t y, const rect_t *rc,zk_bmp_data* zkbmp);
/***
 * in: 
 * bmpid 
 * info ,for readout bmp data
 * 
 * out:
 * 0 read ok
 * -1 not bmpid
 * -2 memalloc fail
 */ 
int preload_bmp(int bmpid,zk_bmp_data* bmpinfo);

int bitblit_zkbmp_alpha(zk_bmp_data* bmpinfo,int fillimgid,uint16_t colorkey);

/**
混合刷新两个图片
x,y；屏幕坐标
bmpid_bottom： 背景图id
bmpid_top： 前景图id

colorkey：前景图关键色透明

其他要求:两个图片尺寸必须要求一样的大小。
return :0 刷新失败，内存不足，尺寸不对。
1. 刷新成功
***/
bool fly_bitblt_multi_img(int16_t x, int16_t y, uint16_t bmpid_bottom,uint16_t bmpid_top,uint16_t colorkey);
#endif /* _GRAPHICS_GFX_H_ */
