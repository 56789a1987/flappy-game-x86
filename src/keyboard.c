#include "common.h"
#include "utils.h"
#include "keyboard.h"

bool hold_space = false;
bool hold_esc = false;
bool hold_s = false;

// called by registration in kernel-loader
void keyboard_handler_cb() {
	unsigned char input = inb(0x60);

	switch (input) {
	case MAKE_SPACE:
		if (!hold_space) {
			press_key();
			hold_space = true;
		}
		break;
	case BREAK_SPACE:
		hold_space = false;
		break;
	case MAKE_ESC:
		if (!hold_esc) {
			toggle_pause();
			hold_esc = true;
		}
		break;
	case BREAK_ESC:
		hold_esc = false;
		break;
	case MAKE_S:
		if (!hold_s) {
			toggle_speed();
			hold_s = true;
		}
		break;
	case BREAK_S:
		hold_s = false;
		break;
	}
}