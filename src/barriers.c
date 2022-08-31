#include "common.h"
#include "barriers.h"

unsigned char barrier_bitmap[VIDEO_HEIGHT * 2][24];

unsigned char barrier_colors[8][5] = {
	{ 0x00, 0x70, 0x58, 0x40, 0x28 },
	{ 0x00, 0x72, 0x5a, 0x42, 0x2a },
	{ 0x00, 0x74, 0x5c, 0x44, 0x2c },
	{ 0x00, 0x78, 0x60, 0x48, 0x30 },
	{ 0x00, 0x7c, 0x64, 0x4c, 0x34 },
	{ 0x00, 0x68, 0x50, 0x38, 0x20 },
	{ 0x00, 0x6a, 0x52, 0x3a, 0x22 },
	{ 0x00, 0x6c, 0x54, 0x3c, 0x24 }
};

unsigned char get_barrier_color(char x) {
	if (x < 0 || x >= 24) {
		return 0;
	} else if (x == 0 || x == 23) {
		return 1;
	} else if (x < 4) {
		return 2;
	} else if (x < 8) {
		return 3;
	} else {
		return 4;
	}
}

void copy_row(unsigned char *source, unsigned char *target) {
	for (char x = 0; x < 24; x++) {
		target[x] = source[x];
	}
}

void init_barrier_bitmap() {
	for (char x = 0; x < 24; x++) {
		barrier_bitmap[0][x] = get_barrier_color(x < 12 ? x - 2 : x + 2);
		barrier_bitmap[VIDEO_HEIGHT - BARRIER_GAP - 9][x] = get_barrier_color(x);
		barrier_bitmap[VIDEO_HEIGHT - BARRIER_GAP - 10][x] = 1;
		barrier_bitmap[VIDEO_HEIGHT - BARRIER_GAP][x] = 1;
	}

	for (unsigned short y = 1; y < VIDEO_HEIGHT - BARRIER_GAP - 10; y++) {
		copy_row(barrier_bitmap[0], barrier_bitmap[y]);
	}
	for (unsigned short y = VIDEO_HEIGHT - BARRIER_GAP - 9; y < VIDEO_HEIGHT - BARRIER_GAP; y++) {
		copy_row(barrier_bitmap[VIDEO_HEIGHT - BARRIER_GAP - 9], barrier_bitmap[y]);
	}
	for (unsigned short y = VIDEO_HEIGHT; y < VIDEO_HEIGHT * 2; y++) {
		copy_row(barrier_bitmap[400 - y], barrier_bitmap[y]);
	}
}

void draw_barrier(short ox, short oy, unsigned char color_schema) {
	short sx = 0, sy = VIDEO_HEIGHT - oy;
	short sw = BARRIER_WIDTH * 2;
	short dx = ox - BARRIER_WIDTH;
	if (dx < 0) {
		sx -= dx;
		sw += dx;
		dx = 0;
	}
	if (dx + sw > VIDEO_WIDTH) {
		sw = VIDEO_WIDTH - dx;
	}

	unsigned char *color_set = barrier_colors[color_schema];
	unsigned char *target = (unsigned char *)VIDEO_MEMORY + dx;

	for (short y = 0; y < VIDEO_HEIGHT; y++) {
		unsigned char *row = barrier_bitmap[y + sy];

		for (short x = 0; x < sw; x++) {
			unsigned char index = row[x + sx];
			if (index) {
				*target = color_set[index];
			}
			target++;
		}

		target += VIDEO_WIDTH - sw;
	}
}

void clear_barrier_frame(short ox, short oy) {
	short x1 = ox + BARRIER_WIDTH - 1;
	short x2 = ox + BARRIER_WIDTH - 3;
	short y1 = oy - BARRIER_GAP - 10;
	short y2 = y1 + 11;
	short y3 = oy + BARRIER_GAP;
	short y4 = y3 + 11;
	unsigned char *target;

	if (x1 >= 0 && x1 < VIDEO_WIDTH) {
		target = (unsigned char *)VIDEO_MEMORY + y1 * VIDEO_WIDTH + x1;
		for (short y = y1; y < y2; y++) {
			*target = CLEAR_COLOR;
			target += VIDEO_WIDTH;
		}

		target = (unsigned char *)VIDEO_MEMORY + y3 * VIDEO_WIDTH + x1;
		for (short y = y3; y < y4; y++) {
			*target = CLEAR_COLOR;
			target += VIDEO_WIDTH;
		}
	}

	if (x2 >= 0 && x2 < VIDEO_WIDTH) {
		target = (unsigned char *)VIDEO_MEMORY + x2;
		for (short y = 0; y < y1; y++) {
			*target = CLEAR_COLOR;
			target += VIDEO_WIDTH;
		}

		target = (unsigned char *)VIDEO_MEMORY + y4 * VIDEO_WIDTH + x2;
		for (short y = y4; y < VIDEO_HEIGHT; y++) {
			*target = CLEAR_COLOR;
			target += VIDEO_WIDTH;
		}
	}
}
