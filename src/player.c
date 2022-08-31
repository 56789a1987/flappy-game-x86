#include "common.h"
#include "data.h"
#include "player.h"

#define PLAYER_SIZE_HALF (PLAYER_SIZE / 2)

unsigned char player_colors[7] = { 0x00, 0x0f, 0x16, 0x1d, 0x10, 0x36, 0xc6 };

void set_player_color(unsigned char color) {
	player_colors[5] = color + 0x38;
	player_colors[6] = color + 0x68;
}

void draw_player(short x, short y) {
	short sx = 0, sy = 0, sw = PLAYER_SIZE_HALF, sh = PLAYER_SIZE;
	short dx = x - PLAYER_SIZE_HALF, dy = y - PLAYER_SIZE_HALF;
	if (dy < 0) {
		sy -= dy;
		sh += dy;
		dy = 0;
	}
	if (dy + sh > VIDEO_HEIGHT) {
		sh = VIDEO_HEIGHT - dy;
	}

	unsigned char *source = PLAYER_BITMAP + sy * PLAYER_SIZE_HALF;
	unsigned char *target = (unsigned char *)VIDEO_MEMORY + dx + dy * VIDEO_WIDTH;
	unsigned char bits, color;
	for (short y = 0; y < sh; y++) {
		for (short x = 0; x < sw; x++) {
			bits = *source;
			color = bits & 0xf;
			if (color) {
				*target = player_colors[color];
			}
			target++;
			color = bits >> 4;
			if (color) {
				*target = player_colors[color];
			}
			target++;
			source++;
		}

		target += VIDEO_WIDTH - sw * 2;
	}
}
