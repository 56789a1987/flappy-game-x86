#include "common.h"
#include "utils.h"
#include "keyboard.h"

bool hold_space = false;

// called by registration in kernel-loader
void keyboard_handler_cb() {
	unsigned char input = inb(0x60);

	switch (input) {
	case MAKE_SPACE:
		if (!hold_space) {
			press_key();
		}
		hold_space = true;
		break;
	case BREAK_SPACE:
		hold_space = false;
		break;
	}
}