#include <stdbool.h>

#define VIDEO_WIDTH 320
#define VIDEO_HEIGHT 200
#define VIDEO_MEMORY 0xA0000

#define CLEAR_COLOR 0x00

#define CHAR_W 8
#define CHAR_H 12

#define STORAGE_LBA 23

#define BARRIER_COUNT 4
#define BARRIER_WIDTH 12
#define BARRIER_GAP 32

#define PLAYER_SIZE 24

typedef struct {
	short x;
	short y;
	unsigned char color;
	bool active;
} Barrier;

typedef struct {
	unsigned long sv_hi_score;
} Storage;

void press_key();
void toggle_pause();
void toggle_speed();
