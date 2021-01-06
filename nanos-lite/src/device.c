#include "common.h"
#include <amdev.h>
#define KEYDOWN_MASK 0x8000

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *p_buf = (char *)buf;
	for(size_t i = 0; i < len; i++){
		_putc(p_buf[i]);
	}
	return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  int keycode = read_key();
	/* handle keyboard event */
	if(keycode != _KEY_NONE){
		/* key down */
		if(keycode & KEYDOWN_MASK){
			keycode = keycode ^ KEYDOWN_MASK;
			sprintf(buf, "kd %s\n", keyname[keycode]);
		}
		/* key up */
		else{
			sprintf(buf, "ku %s\n", keyname[keycode]);
		}
	}
	/* timer event */
	else{
		sprintf(buf, "t %u\n", uptime());
	}
	return strlen(buf);
}

static char dispinfo[128] __attribute__((used)) = {};

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  strncpy((char *)buf, dispinfo + offset, len);
	return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  /* write the len bytes from buf to the screen(offset -> coord) */
	/* calc w from len */
	int w = len / sizeof(uint32_t);
	int h = 1;
	/* convert to int unit offset */
	offset = offset / sizeof(uint32_t);
	int x = offset % screen_width();
	int y = offset / screen_width();
	draw_rect((uint32_t *)buf, x, y, w, h);
	return len;
}

size_t fbsync_write(const void *buf, size_t offset, size_t len) {
  draw_sync();
	return 0;
}

void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention
	sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", screen_width(), screen_height());
}
