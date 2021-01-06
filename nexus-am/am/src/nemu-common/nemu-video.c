#include <am.h>
#include <amdev.h>
#include <nemu.h>
#include <klib.h>
#define SCREEN_H 300
#define SCREEN_W 400
size_t __am_video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _DEV_VIDEO_INFO_t *info = (_DEV_VIDEO_INFO_t *)buf;
      info->width = SCREEN_W;
      info->height = SCREEN_H;
      return sizeof(_DEV_VIDEO_INFO_t);
    }
  }
  return 0;
}

size_t __am_video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      // _DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
			// /* write to video memory MMIO from 0xa0000000, MODE(400x300x32) */
			// uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
			// uint32_t *pixels = ctl->pixels; // pixels (0x00RRGGBB)
			// int x_off = ctl->x, y = ctl->y;
			// for(int i = 0; i < ctl->h; i++){
			// 	for(int j = 0; j < ctl->w; j++){
			// 		if((y + i) > (SCREEN_H - 1) || (x_off + j) > (SCREEN_W - 1))break;
			// 		fb[(y+i)*SCREEN_W + x_off + j] = pixels[i * ctl->w + j];
			// 	}
			// }
      // if (ctl->sync) {
      //   outl(SYNC_ADDR, 0);
      // }
      // return size;
			_DEV_VIDEO_FBCTL_t *ctl = (_DEV_VIDEO_FBCTL_t *)buf;
			int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;

			/* fb: the whole data that will be painted on the screen,
			 * but we only need to fill part of data (that is from (x, y) to (x + w, y + h)) by copy from pixels
			 */
			uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;

			/* pixels: the data that will be painted. we show copy the data in pixels to fb. */
			uint32_t *pixels = ctl->pixels;
			int copy_size = sizeof(uint32_t) * (SCREEN_W - x < w ? SCREEN_W - x : w);
			for (int i = 0; i < h && y + i < SCREEN_H; ++i)
			{
				memcpy(&fb[(i + y) * SCREEN_W + x], pixels + i * w, copy_size);
			}
			if (ctl->sync)
			{
				outl(SYNC_ADDR, 0);
			}
			return size;
		}
  }
  return 0;
}

void __am_vga_init() {
	int i;
	int size = screen_width() * screen_height();
	uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
	for(i = 0; i < size; i++){
		fb[i] = i;
	}
	draw_sync();
}
