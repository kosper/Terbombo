#ifndef GAME_H
#define GAME_H

#include <minesweeper.h>
#include <menu.h>

typedef struct Game {
	Board board;
	WINDOW* menu;
	b8 is_running;
} Game;

void game_init(Game* game);
void game_run(Game* game);
void game_shutdown(Game* game);

b8 handle_menu(Game* game, game_settings* settings);

#endif
