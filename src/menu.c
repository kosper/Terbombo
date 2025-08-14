#include <menu.h>
#include <scoreboard.h>

void menu_init(WINDOW** menu) {
	*menu = newwin(4, 20, LOGO_ROW_CNT + 1, 0);

	curs_set(0);
	refresh();

	logo_print();
	menu_print(*menu);
	controls_print();	

	wmove(*menu, 1, 1);	
	wprintw(*menu, ">");
	wmove(*menu, 1, 1);	

	wrefresh(*menu);

	return;
}

b8 menu_run(WINDOW** menu) {
	while(TRUE) {
		i32 choice = get_choice(*menu, 3);	

		/*
		 * NOTE: 
		 * 	Choice 1 = Play
		 * 	Choice 2 = Scoreboard
		 * 	Choice 3 = Exit
		 *
		 * No need for enum.
		 */
		if(choice == 1)
			return TRUE;
		else if(choice == 2) {
			menu_shutdown(*menu);

			scoreboard_print();
			clear();

			menu_init(menu);
		}

		else break;
	}

	return FALSE;
}

void menu_shutdown(WINDOW* menu) {
	wclear(menu);
	delwin(menu);

	clear();
	refresh();

	curs_set(1);

	return;
}

void menu_print(WINDOW* menu) {
	wmove(menu, 1, 1);	
	wprintw(menu, " Play");

	wmove(menu, 2, 1);
	wprintw(menu, " Scoreboard");

	wmove(menu, 3, 1);	
	wprintw(menu, " Quit");

	return;
}

void logo_print(void) {
	mvprintw(0, 0, ",--------.             ,--.                    ,--.           ");
	mvprintw(1, 0, "'--.  .--',---. ,--.--.|  |-.  ,---. ,--,--,--.|  |-.  ,---.  ");
  	mvprintw(2, 0, "   |  |  | .-. :|  .--'| .-. '| .-. ||        || .-. '| .-. | ");
  	mvprintw(3, 0, "   |  |  \\   --.|  |   | `-' |' '-' '|  |  |  || `-' |' '-' ' ");
   	mvprintw(4, 0, "   `--'   `----'`--'    `---'  `---' `--`--`--' `---'  `---' ");
	mvprintw(5, 0, "______________________________________VERSION_1.0____________");

	refresh();

	return;
}

void controls_print(void) {
	mvprintw(11, 1, "Move: W,A,S,D");
	mvprintw(12, 1, "Select: E/Enter");

	refresh();
	
	return;
}

void settings_print(WINDOW* menu) {
	wmove(menu, 1, 1);	
	wprintw(menu, " Easy");

	wmove(menu, 2, 1);	
	wprintw(menu, " Medium");

	wmove(menu, 3, 1);	
	wprintw(menu, " Expert");

	wmove(menu, 4, 1);	
	wprintw(menu, " Custom");

	wmove(menu, 5, 1);
	wprintw(menu, " Back");

	wrefresh(menu);

	return;
}

i32 get_choice(WINDOW* menu, i32 row_cnt) {
	i32 row = 1;

	while(true) {
		i8 input = getch();

		switch(input) {
			case 'w':
				if(row <= 1)
					continue;

				wprintw(menu, " ");

				row--;

				wmove(menu, row, 1);
				wprintw(menu, ">");
				wmove(menu, row, 1);

				wrefresh(menu);

				continue;
			case 's':
				if(row >= row_cnt)
					continue;

				wprintw(menu, " ");

				row++;

				wmove(menu, row, 1);
				wprintw(menu, ">");
				wmove(menu, row, 1);

				wrefresh(menu);

				continue;
			case 'e':
			case '\n':
				return row;
			default: continue;
		};
	}

	return row;
}

b8 get_settings(WINDOW** menu, game_settings* settings) {
	*menu = newwin(7, 20, LOGO_ROW_CNT + 1, 0);

	curs_set(0);

	refresh();

	logo_print();
	settings_print(*menu);	

	wmove(*menu, 1, 1);
	wprintw(*menu, ">");
	wmove(*menu, 1, 1);

	wrefresh(*menu);

	i32 choice = get_choice(*menu, 5);

	switch(choice) {
		case 1: {
			settings->rows = 10;
			settings->cols = 10;
			settings->bomb_cnt = 10;
			settings->difficulty = BEGGINER;

			break;
		}
		case 2: {
			settings->rows = 15;
			settings->cols = 13;
			settings->bomb_cnt = 40;
			settings->difficulty = INTERMIDIATE;

			break;
		}
		case 3: {
			settings->rows = 16;
			settings->cols = 30;
			settings->bomb_cnt = 99;
			settings->difficulty = EXPERT;

			break;
		}
		case 4: {
			custom_game_config(settings);		
			settings->difficulty = CUSTOM;

			break;
		}
		case 5:
			clear();
			return TRUE;

		default:
			break;
	}

	return FALSE;
}

void custom_game_config(game_settings* settings) {
	i32 input_rows;
	i32 input_cols;
	i32 input_bombs;

	WINDOW* cs_choice_win = newwin(6, 50, 13, 0);
	refresh();

	while(TRUE) {
		curs_set(1);
		echo();

		wmove(cs_choice_win, 0, 1);
		wprintw(cs_choice_win, "Rows: ");
		wrefresh(cs_choice_win);
		wscanw(cs_choice_win, "%d", &input_rows);

		wmove(cs_choice_win, 1, 1);
		wprintw(cs_choice_win, "Columns: ");
		wrefresh(cs_choice_win);
		wscanw(cs_choice_win, "%d", &input_cols);

		wmove(cs_choice_win, 2, 1);
		wprintw(cs_choice_win, "Bombs: ");
		wrefresh(cs_choice_win);
		wscanw(cs_choice_win, "%d", &input_bombs);

		curs_set(0);
		noecho();

		if(input_rows < MIN_CUSTOM_ROWS || input_rows > MAX_CUSTOM_ROWS) {
			wmove(cs_choice_win, 4, 1);
			wprintw(cs_choice_win, "Rows must be less than %d and more than %d!", MAX_CUSTOM_ROWS, MIN_CUSTOM_ROWS); 
			wrefresh(cs_choice_win);
			wgetch(cs_choice_win);

			wclear(cs_choice_win);

			continue;
		}
		else if(input_cols < MIN_CUSTOM_COLS || input_cols > MAX_CUSTOM_COLS) {
			wmove(cs_choice_win, 4, 1);
			wprintw(cs_choice_win, "Columns must be less than %d and more than %d!", MAX_CUSTOM_COLS, MIN_CUSTOM_COLS); 
			wrefresh(cs_choice_win);
			wgetch(cs_choice_win);

			wclear(cs_choice_win);

			continue;
		}
		else if(input_bombs < MIN_BOMB_CNT || input_bombs > MAX_BOMB_CNT(input_rows, input_cols)) {
			wmove(cs_choice_win, 5, 1);
			wprintw(cs_choice_win, "Bombs must be less than %d and more than %d!", MAX_BOMB_CNT(input_rows, input_cols), MIN_BOMB_CNT); 
			wrefresh(cs_choice_win);
			wgetch(cs_choice_win);

			wclear(cs_choice_win);

			continue;
		}

		break;
	};

	delwin(cs_choice_win);

	clear();
	refresh();

	settings->rows = input_rows;
	settings->cols = input_cols;
	settings->bomb_cnt = input_bombs;

	return;
}
