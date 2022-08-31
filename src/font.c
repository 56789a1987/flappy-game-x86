#include "common.h"
#include "data.h"
#include "font.h"

void draw_char(short x, short y, unsigned char chr, unsigned char color) {
	// uncomment, if characters may be drawn off screen
	/*
	if (x < -CHAR_W || y < -CHAR_H || x > VIDEO_WIDTH || y > VIDEO_HEIGHT) {
		return;
	}
	*/

	short x1 = 0, y1 = 0, x2 = CHAR_W, y2 = CHAR_H, pr = 0, pb = 0;

	/*
	if (x < 0) {
		x1 -= x;
	}
	if (y < 0) {
		y1 -= y;
	}
	if (x > VIDEO_WIDTH - CHAR_W) {
		pr = x + CHAR_W - VIDEO_WIDTH;
		x2 -= pr;
	}
	if (y > VIDEO_HEIGHT - CHAR_H) {
		pb = y + CHAR_H - VIDEO_HEIGHT;
		y2 -= pb;
	}
	*/

	unsigned char *target = (unsigned char *)VIDEO_MEMORY + x + x2 - 1 + (y + y1) * VIDEO_WIDTH;
	for (short dy = y1; dy < y2; dy++) {
		unsigned char bits = FONT_BITMAP[chr * CHAR_H + dy] >> pr;
		for (short dx = x2 - 1; dx >= x1; dx--) {
			if (bits & 1) {
				*target = color;
			} else {
				*target = CLEAR_COLOR;
			}
			target--;
			bits >>= 1;
		}
		target += VIDEO_WIDTH + x2 - x1;
	}
}

void draw_chars(short x, short y, unsigned char *chars, unsigned char color) {
	unsigned char i = 0, chr = chars[0];
	while (chr) {
		draw_char(x, y, chr, color);
		chr = chars[++i];
		x += CHAR_W;
	}
}
