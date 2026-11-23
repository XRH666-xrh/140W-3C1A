/*
 * video.h
 *
 *  Created on: Nov 29, 2020
 *      Author: ZKSWE Develop Team
 */

#ifndef _GRAPHICS_VIDEO_H_
#define _GRAPHICS_VIDEO_H_

#include "context/common.h"

typedef struct {
	uint16_t width;
	uint16_t height;
	uint8_t bpp;
	uint32_t *disp_buff;
} video_info_t;

bool video_init();
void video_deinit();
void video_flip();

#endif /* _GRAPHICS_VIDEO_H_ */
