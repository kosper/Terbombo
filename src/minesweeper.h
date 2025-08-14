#ifndef MINESWEEPER_H
#define MINESWEEPER_H

#include <scoreboard.h>

#include <game_defines.h>
#include <pthread.h>

#define FIRST_ROW 1
#define FIRST_COL 2

#define BORDER_PADDING_ROW 2
#define BORDER_PADDING_COL 4

#define MAIN_WINDOW_SIZE_Y(x) x + BORDER_PADDING_ROW
#define MAIN_WINDOW_SIZE_X(x) (x * 2) + BORDER_PADDING_COL - 1

#define TRANSLATE_ROW(x) x + FIRST_ROW
#define TRANSLATE_COL(y) (y*2) + FIRST_COL

#define TIMER_POS_COL(x) MAIN_WINDOW_SIZE_X(x) - 5

typedef struct Timer {
	pthread_t timer_tid;
	pthread_mutex_t timer_mutex;

	b8 timer_finished;

	i32 minutes_elapsed;
	i32 seconds_elapsed;
} Timer;

typedef struct Board {
	i8** board;
	i8** final_board;
	
	i32 rows, cols;
	i32 bomb_cnt;	
	i32 flags;

	i32 pos_r;
	i32 pos_c;

	i32 tiles_remaining;

	Timer timer;

	pthread_mutex_t go_mutex;

	WINDOW* main_window;
	WINDOW* help_window;
	WINDOW* flags_remaining;

	GAME_DIFFICULTY difficulty;
} Board;

void minesweeper_init(Board* board, game_settings* settings);
b8 minesweeper_run(Board* board);
void minesweeper_shutdown(Board* board);

void place_bombs(Board* board);
void init_board(Board* board);

void game_over(Board* board);
b8 play_again(Board* board);

i32 bombs_around(Board* board, i32 x, i32 y);
i32 flags_around(Board* board, i32 x, i32 y);
void solve_board(Board* board);
void expand_zero_tile(Board* board, i32 x, i32 y);
b8 reveal_tiles_around(Board* board, i32 x, i32 y);
void place_flag(Board* board, i32 row, i32 column);

void* reveal_bombs(void* board_vp); //NOTE: Threaded function.
void* timer_start(void* boiard_vp); //NOTE: Threaded function.

void print_board(Board* board, i32 x, i32 y);
void draw_flags(Board* board);
void draw_help(Board* board);

void get_name(Board* board, char* name);
void scoreboard_check(Board* board, i32 secs); 

//NOTE: Util Functions.
i32 kbhit(void); //NOTE: Get keyboard hit evetn.
void cursor_move(WINDOW* window, i32 row, i32 column);

#endif
