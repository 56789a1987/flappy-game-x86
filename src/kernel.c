#include "common.h"
#include "utils.h"
// hardwares
#include "keyboard.h"
#include "mouse.h"
#include "ide.h"
// graphics
#include "font.h"
#include "player.h"
#include "barriers.h"

#define TEXT_X(TEXT) (VIDEO_WIDTH - (sizeof(TEXT) - 1) * CHAR_W) / 2;

const unsigned char START_TEXT[] = "Press space key to start";
const unsigned char PAUSE_TEXT[] = "Paused";
const unsigned char OVER_TEXT[] = "Game Over";
const unsigned char HI_SCORE_TEXT[] = "Hi-score ";
const unsigned char BACK_TEXT[] = "Made by Polyethylene";

const short START_TEXT_X = TEXT_X(START_TEXT);
const short PAUSE_TEXT_X = TEXT_X(PAUSE_TEXT);
const short OVER_TEXT_X = TEXT_X(OVER_TEXT);
const short HI_SCORE_TEXT_X = TEXT_X(HI_SCORE_TEXT);

union {
	Storage data;
	unsigned char buffer[IDE_BUFFER_SIZE];
} storage;

Storage *saved_data = &storage.data;
#define hi_score saved_data->sv_hi_score

const unsigned short FREQS[] = { 30, 45, 60, 90, 120 };
unsigned char freq_index = 2;

float player_x = VIDEO_WIDTH / 2;
float player_y = VIDEO_HEIGHT / 2;
float vy = 0;

unsigned long score = 0;

bool is_inited = false;
bool is_gameover = false;
bool is_paused = false;

unsigned long game_tick = 0;
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

	game_tick = 0;
	score = 0;
	player_x = VIDEO_WIDTH / 2;
	player_y = VIDEO_HEIGHT / 2;
	is_gameover = false;
	is_inited = false;
}

void game_over() {
	if (is_gameover) {
		return;
	}

	is_gameover = true;
	beep_seq(110, 110);

	if (score > hi_score) {
		hi_score = score;
		ide_write_sector(STORAGE_LBA, storage.buffer);
	}
}

void draw_score() {
	short x = VIDEO_WIDTH - CHAR_W, y = CHAR_H;
	draw_uint32_right(x, y, score, 0xf) - CHAR_W;
}

unsigned char color_index = 0;

void game_frame() {
	if ((game_tick % 96) == 0) {
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
		game_over();
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
					game_over();
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

	game_tick++;
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
		unsigned char color = 0x40 + (tick % 0x18);
		draw_chars(OVER_TEXT_X, 24, OVER_TEXT, color);

		if (hi_score) {
			short x = HI_SCORE_TEXT_X - number_size(hi_score) * CHAR_W / 2;
			draw_chars(x, 48, HI_SCORE_TEXT, color);
			draw_uint32_right(VIDEO_WIDTH - x, 48, hi_score, color);
		}
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

	if (is_paused) {
		draw_chars(PAUSE_TEXT_X, 24, PAUSE_TEXT, 0x40 + (tick % 0x18));
		return;
	}

	game_frame();
}

void press_key() {
	if (!is_inited) {
		clear_rect(START_TEXT_X, 24, VIDEO_WIDTH - START_TEXT_X * 2, CHAR_H);
		is_inited = true;
	}
	if (is_gameover) {
		reset_game();
		clear_rect(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}
	if (!is_paused) {
		vy = -3;
	}
}

void toggle_pause() {
	if (is_inited && !is_gameover) {
		is_paused = !is_paused;
		if (!is_paused) {
			clear_rect(PAUSE_TEXT_X, 24, VIDEO_WIDTH - PAUSE_TEXT_X * 2, CHAR_H);
		}
	}
}

void toggle_speed() {
	freq_index = (freq_index + 1) % (sizeof(FREQS) / sizeof(short));
	set_timer(FREQS[freq_index]);
}

void main() {
	asm volatile ("cli");
	init_mouse();
	asm volatile ("sti");

	init_barrier_bitmap();
	reset_game();
	set_timer(FREQS[freq_index]);

	zero_memory(storage.buffer, sizeof(storage.buffer));
	ide_read_sector(STORAGE_LBA, storage.buffer);
}
