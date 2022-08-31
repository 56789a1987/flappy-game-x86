#include "common.h"
#include "utils.h"
#include "keyboard.h"
#include "mouse.h"
#include "font.h"
#include "player.h"
#include "barriers.h"

unsigned char START_TEXT[] = "Press space key to start";
unsigned char OVER_TEXT[] = "Game Over";
unsigned char BACK_TEXT[] = "Made by Polyethylene";

unsigned char START_TEXT_X = (VIDEO_WIDTH - (sizeof(START_TEXT) - 1) * CHAR_W) / 2;
unsigned char OVER_TEXT_X = (VIDEO_WIDTH - (sizeof(OVER_TEXT) - 1) * CHAR_W) / 2;

unsigned long tick = 0;

float player_x = VIDEO_WIDTH / 2;
float player_y = VIDEO_HEIGHT / 2;
float vy = 0;
unsigned long score = 0;

bool is_inited = false;
bool is_gameover = false;


unsigned char sound_tick = 0;
unsigned long next_freq = 0;

Barrier barriers[BARRIER_COUNT];
unsigned char barrier_index = 0;

void beep_seq(unsigned short freq1, unsigned short freq2) {
	start_beep(freq1);
	next_freq = freq2;
	sound_tick = 30;
}

void reset_game() {
	for (unsigned char i = 0; i < BARRIER_COUNT; i++) {
		Barrier *b = barriers + i;
		b->x = -32;
		b->y = 0;
		b->active = false;
	}

	score = 0;
	player_x = VIDEO_WIDTH / 2;
	player_y = VIDEO_HEIGHT / 2;
	is_gameover = false;
	is_inited = false;
}

void draw_score() {
	short x = VIDEO_WIDTH - CHAR_W, y = CHAR_H;
	unsigned long s = score;

	if (s == 0) {
		x -= CHAR_W;
		draw_char(x, y, '0', 0xf);
		return;
	}

	while (s) {
		x -= CHAR_W;
		unsigned char chr = (s % 10) + '0';
		draw_char(x, y, chr, 0xf);
		s /= 10;
	}
}

unsigned char color_index = 0;

void game_frame() {
	if ((tick % 96) == 0) {
		Barrier *b = barriers + barrier_index;
		b->x = VIDEO_WIDTH + 32;
		b->y = BARRIER_GAP + (xorshf96() % (VIDEO_HEIGHT - BARRIER_GAP * 2));
		b->color = color_index++ % 8;
		b->active = true;
		barrier_index = ++barrier_index % BARRIER_COUNT;
	}

	// clearing the whole screen every time is inefficient, so only clear updated areas
	for (unsigned char i = 0; i < BARRIER_COUNT; i++) {
		Barrier *b = barriers + i;
		clear_barrier_frame(b->x, b->y);
	}
	clear_rect(player_x - 12, player_y - 12, PLAYER_SIZE, PLAYER_SIZE);
	//draw_chars(CHAR_W, VIDEO_HEIGHT - CHAR_H * 2, BACK_TEXT, 0x12);

	player_y += vy;
	vy += 0.12;

	if (player_y < 0) {
		player_y = 0;
		vy = 0;
	} else if (player_y > VIDEO_HEIGHT) {
		is_gameover = true;
		beep_seq(110, 110);
	}

	draw_player(player_x, player_y);

	for (unsigned char i = 0; i < BARRIER_COUNT; i++) {
		Barrier *b = barriers + i;
		if (b->x > -32) {
			b->x--;
			draw_barrier(b->x, b->y, b->color);
		}

		if (b->active) {
			if (player_x > b->x - BARRIER_WIDTH && player_x < b->x + BARRIER_WIDTH) {
				if (player_y < b->y - BARRIER_GAP || player_y > b->y + BARRIER_GAP) {
					is_gameover = true;
					beep_seq(110, 110);
				}
			}

			if (player_x > b->x + BARRIER_WIDTH) {
				beep_seq(523, 784);
				score++;
				b->active = false;
			}
		}
	}

	draw_score();
}

// called by registration in kernel-loader
void timer_handler_cb() {
	tick++;

	if (sound_tick > 0) {
		sound_tick--;
		if (sound_tick == 25) {
			start_beep(next_freq);
		} else if (sound_tick == 0) {
			stop_beep();
		}
	}

	if (is_gameover) {
		draw_chars(OVER_TEXT_X, 24, OVER_TEXT, 0x40 + (tick % 0x18));
		return;
	}

	if (!is_inited) {
		draw_chars(CHAR_W, 176, BACK_TEXT, 0x12);
		draw_chars(START_TEXT_X, 24, START_TEXT, 0x40 + (tick % 0x18));
		draw_player(player_x, player_y);
		set_player_color(tick % 0x18);
		xorshf96(); // prepare random seeds
		return;
	}

	game_frame();
}

void press_key() {
	if (!is_inited) {
		clear_rect(0, 24, VIDEO_WIDTH, CHAR_H);
		is_inited = true;
	}
	if (is_gameover) {
		reset_game();
		clear_rect(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	vy = -3;
}

void main() {
	init_mouse();
	init_barrier_bitmap();
	reset_game();
	set_timer(60);
}
