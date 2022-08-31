#include "common.h"
#include "utils.h"
#include "mouse.h"

unsigned char mouse_cycle = 0;
char mouse_byte[3];

bool hold_mouse_left = false;

void ps2_wait_input() {
	unsigned long timeout = 100000;
	while (timeout-- && !(inb(0x64) & 1));
}

void ps2_wait_output() {
	unsigned long timeout = 100000;
	while (timeout-- && inb(0x64) & 2);
}

void mouse_write(unsigned char write) {
	ps2_wait_output();
	outb(0x64, 0xd4);
	ps2_wait_output();
	outb(0x60, write);
}

unsigned char mouse_read() {
	ps2_wait_input();
	return inb(0x60);
}

void init_mouse() {
	unsigned char status;

	asm volatile ("cli");

	// enable the auxiliary mouse device
	ps2_wait_output();
	outb(0x64, 0xa8);

	// enable the interrupts
	ps2_wait_output();
	outb(0x64, 0x20);
	ps2_wait_input();
	status = (inb(0x60) | 2);
	ps2_wait_output();
	outb(0x64, 0x60);
	ps2_wait_output();
	outb(0x60, status);

	// use default settings
	mouse_write(0xf6);
	mouse_read();

	// enable the mouse
	mouse_write(0xf4);
	mouse_read();

	asm volatile ("sti");
}

void handle_mouse_packet() {
	bool left_button = mouse_byte[0] & 0x01;

	if (hold_mouse_left != left_button) {
		if (left_button) {
			press_key();
		}
		hold_mouse_left = left_button;
	}
}

// called by registration in kernel-loader
void mouse_handler_cb() {
	unsigned char status = inb(0x64);
	char input = inb(0x60);

	if (status & 0x20) {
		mouse_byte[mouse_cycle++] = input;
		if (mouse_cycle > 2) {
			mouse_cycle = 0;
			handle_mouse_packet();
		}
	}
}
