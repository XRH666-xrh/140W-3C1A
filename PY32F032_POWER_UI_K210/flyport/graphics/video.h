/*
 * video.h
 *
 *  Created on: Nov 29, 2020
 *      Author: ZKSWE Develop Team
 */

#ifndef __GRAPHICS_VIDEO_H_
#define __GRAPHICS_VIDEO_H_

#include "context/common.h"

bool video_init();
void video_deinit();
void video_flip();
void LCD_Backlight(bool on);
void video_rotate(uint8_t dir);

#endif /* _GRAPHICS_VIDEO_H_ */
