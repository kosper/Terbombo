#include <game.h>

void game_init(Game* game) {
	initscr();
	noecho();
	
	game->is_running = TRUE;

	return;
}

void game_run(Game* game) {
	/*
	 * NOTE:
	 * 	When game starts, menu is handled. If user
	 * selects "Play", the actual game starts. User can
	 * "Play Again" if he chooses to, or "End" the game
	 * prematurely, and state returns back to "Menu".
	 */
	while(game->is_running == TRUE) {
		game_settings settings = {0};
		game->is_running = handle_menu(game, &settings);	

		if(game->is_running == FALSE)
			continue;

		b8 play_again;

		do {
			minesweeper_init(&game->board, &settings);
			play_again = minesweeper_run(&game->board);
			minesweeper_shutdown(&game->board);
		}
		while(play_again == TRUE);
	}

	return;
}

void game_shutdown(Game* game) {
	(void)game;
	endwin();		

	return;
}

b8 handle_menu(Game* game, game_settings* settings) {
	b8 go_back = FALSE;

	do{
		menu_init(&game->menu);

		if(menu_run(&game->menu) == FALSE) {
			menu_shutdown(game->menu);

			return FALSE;
		}


		go_back = get_settings(&game->menu, settings);
		menu_shutdown(game->menu);
	}
	while(go_back == TRUE);

	return TRUE;
}
