#include "common.h"
#include "utils.h"

void outb(unsigned short port, unsigned char value) {
    asm volatile("outb %1, %0"
                 :
                 : "dN"(port), "a"(value));
}

unsigned char inb(unsigned short port) {
    unsigned char ret;
    asm volatile("inb %1, %0"
                 : "=a"(ret)
                 : "dN"(port));
    return ret;
}

unsigned short inw(unsigned short port) {
    unsigned short ret;
    asm volatile("inw %1, %0"
                 : "=a"(ret)
                 : "dN"(port));
    return ret;
}

void set_timer(unsigned long frequency) {
	// the divisor must be small enough to fit into 16-bits
	unsigned long divisor = 1193180 / frequency;

	// send the command byte
	outb(0x43, 0x36);

	// send the frequency divisor
	outb(0x40, divisor & 0xff);
	outb(0x40, (divisor >> 8) & 0xff);
}

void start_beep(unsigned long frequency) {
	unsigned long divisor = 1193180 / frequency;
	outb(0x43, 182);

	// send the frequency divisor
	outb(0x42, divisor & 0xff);
	outb(0x42, (divisor >> 8) & 0xff);

	// enable PC speaker
	outb(0x61, inb(0x61) | 0x03);
}

void stop_beep() {
	// disable PC speaker
	outb(0x61, inb(0x61) & 0xfc);
}

void clear_rect(short x, short y, short w, short h) {
	if (x >= VIDEO_WIDTH || y >= VIDEO_HEIGHT || x + w < 0 || y + h < 0) {
		return;
	}

	short i, j;
	
	if (x < 0) {
		w += x;
		x = 0;
	}
	if (y < 0) {
		h += y;
		y = 0;
	}
	if (x + w > VIDEO_WIDTH) {
		w = VIDEO_WIDTH - x;
	}
	if (y + h > VIDEO_HEIGHT) {
		h = VIDEO_HEIGHT - y;
	}

	unsigned char *target = (unsigned char *)VIDEO_MEMORY + y * VIDEO_WIDTH + x;
	for (i = 0; i < h; i++) {
		for (j = 0; j < w; j++) {
			*target = CLEAR_COLOR;
			target++;
		}
		target += VIDEO_WIDTH - w;
	}
}

// random

unsigned long seed1 = 123456789, seed2 = 362436069, seed3 = 521288629;

unsigned long xorshf96() {
	unsigned long t;
	seed1 ^= seed1 << 16;
	seed1 ^= seed1 >> 5;
	seed1 ^= seed1 << 1;

	t = seed1;
	seed1 = seed2;
	seed2 = seed3;
	seed3 = t ^ seed1 ^ seed2;

	return seed3;
}
