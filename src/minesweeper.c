#include <game_defines.h>
#include <minesweeper.h>

#include <scoreboard.h>

#include <time.h>
#include <unistd.h>
#include <stdlib.h>

//NOTE: Includes for the kbhit function.
#include <sys/ioctl.h>
#include <termios.h>

void minesweeper_init(Board* board, game_settings* settings) {
	board->bomb_cnt = settings->bomb_cnt;
	board->flags = settings->bomb_cnt;
	board->rows = settings->rows;
	board->cols = settings->cols;
	board->difficulty = settings->difficulty;

	i32 tile_cnt = board->rows * board->cols;
	board->tiles_remaining = tile_cnt - board->bomb_cnt;

	init_board(board);
	solve_board(board);
		
	board->main_window = newwin(MAIN_WINDOW_SIZE_Y(board->rows), MAIN_WINDOW_SIZE_X(board->cols), 0, 0);
	board->help_window = newwin(4, 50, MAIN_WINDOW_SIZE_Y(board->rows), 0);
	board->flags_remaining = newwin(3, 23, MAIN_WINDOW_SIZE_Y(board->rows) + 3, 0);

	refresh();

	draw_help(board);	
	draw_flags(board);	
	
	//Prepare main window.
	box(board->main_window, 0, 0);
	mvwprintw(board->main_window, 0, 2, "FIELD");
	mvwprintw(board->main_window, 0, TIMER_POS_COL(board->cols), "%03d", 0);
	print_board(board, FIRST_ROW, FIRST_COL);	

	curs_set(1);


	return;
}

b8 minesweeper_run(Board* board) {
	//NOTE: Current row and column of board array.
	i32 row = 0;
	i32 column = 0;

	b8 first_expand = FALSE;

	while(board->tiles_remaining > 0) {
		i8 c = getch();
		
		board->pos_r = TRANSLATE_ROW(row);
		board->pos_c = TRANSLATE_COL(column);

		//NOTE: Handle input.
		switch(c) {
			case 'w': {
				if(board->pos_r > FIRST_ROW) {
					--row;
					--board->pos_r;

					cursor_move(board->main_window, board->pos_r, board->pos_c);			
				}
				
				continue;
			}
			case 'a': {
				if(board->pos_c > FIRST_COL) {
					column--;
					board->pos_c -= 2;

					cursor_move(board->main_window, board->pos_r, board->pos_c);			
				}

				continue;
			}	
			case 's': {
				if(board->pos_r < MAIN_WINDOW_SIZE_Y(board->rows) - 2) {
					row++;
					board->pos_r++;

					cursor_move(board->main_window, board->pos_r, board->pos_c);			
				}

				continue;
			}
			case 'd': {
				if(board->pos_c < MAIN_WINDOW_SIZE_X(board->cols) - 4) {
					column++;
					board->pos_c += 2;

					cursor_move(board->main_window, board->pos_r, board->pos_c);			
				}

				continue;
			}
			case '\n': 
			case 'e': {
				i8 tile_val = board->final_board[row][column];

				//NOTE: If this is the first action, start clock.
				if(first_expand == FALSE && tile_val != '*') {
					first_expand = TRUE;

					pthread_create(&board->timer.timer_tid, NULL, timer_start, (void*)board);
				}

				//NOTE: Do not expand flagged tiles.
				if(board->board[row][column] == 'F') {
					continue;
				}
				else if(board->board[row][column] > '0' && board->board[row][column] < '9') {
					i32 flag_count = flags_around(board, row, column);	

					/*
					 * NOTE: In classic minesweeper, you can only expand a tile, if the flags you set
					 * are the same with the number of bombs around a revealed tile.
					 */
					if(flag_count == (board->board[row][column] - '0')) {
						b8 is_bomb = reveal_tiles_around(board, row, column);
						print_board(board, TRANSLATE_ROW(row), TRANSLATE_COL(column)); 

						if(is_bomb == TRUE) {
							game_over(board);	
							b8 should_play_again = play_again(board);
							return should_play_again;
						}
					}

					continue;
				}

				/*
				 * NOTE: In classic minesweeper when you select a tile with 0 bombs around,
				 * it reveals all tiles with 0 bombs recursively around tile.
				 */
				if(tile_val == '0') {
					expand_zero_tile(board, row, column);
					print_board(board, TRANSLATE_ROW(row), TRANSLATE_COL(column)); 
					
					continue;
				}
				else if(tile_val == '*') {
					board->board[row][column] = '*';	
					
					print_board(board, TRANSLATE_ROW(row), TRANSLATE_COL(column));
					game_over(board);

					b8 should_play_again = play_again(board);

					return should_play_again;
				}
				else  {
					board->tiles_remaining--;
					board->board[row][column] = tile_val;	

					print_board(board, TRANSLATE_ROW(row), TRANSLATE_COL(column));

					continue;	
				}
			}
			case 'f': {
				place_flag(board, row, column);

				continue;
			}
			case 'p': {
				//NOTE: End the game.
				board->timer.timer_finished = TRUE;
				pthread_join(board->timer.timer_tid, NULL);

				return FALSE;
			}
				
			default: break;
		}
	}

	i32 congrats_row = MAIN_WINDOW_SIZE_Y(board->rows) + 4 + 3;
	i32 congrats_column = 0;

	board->timer.timer_finished = TRUE;
	pthread_join(board->timer.timer_tid, NULL);

	move(congrats_row, congrats_column);
	printw("Congrats, YOU WON!, your time was %d:%d", board->timer.minutes_elapsed, board->timer.seconds_elapsed);

	scoreboard_check(board, (board->timer.minutes_elapsed * 60 + board->timer.seconds_elapsed));

	b8 should_play_again = play_again(board);

	return should_play_again;
}

void minesweeper_shutdown(Board* board) {
	clear();

	delwin(board->main_window);
	delwin(board->help_window);
	delwin(board->flags_remaining);

	free(board->board);
	free(board->final_board);

	return;
}

void place_bombs(Board* board) {
	srand(time(NULL));

	for(int i = 0; i < board->bomb_cnt; ++i) {
		i32 x = 0;
		i32 y = 0;

		do {
			x = rand() % board->rows;
			y = rand() % board->cols;
		}
		while(board->final_board[x][y] == '*');
		
		board->final_board[x][y] = '*';
	}

	return;
}

void init_board(Board* board) {
	board->pos_r = 0;
	board->pos_c = 0;

	board->board = (i8**)malloc(board->rows * sizeof(i8*));
	board->final_board = (i8**)malloc(board->rows * sizeof(i8*));

	for(i32 i = 0; i < board->rows; i++) {
		board->board[i] = (i8*)malloc(board->cols * sizeof(i8));
		board->final_board[i] = (i8*)malloc(board->cols * sizeof(i8));
	}

	for(i32 i = 0; i < board->rows; ++i)
		for(i32 j = 0; j < board->cols; ++j) {
			board->board[i][j] = '-';
			board->final_board[i][j] = '0';
		}

	place_bombs(board);

	return;
}

void game_over(Board* board) {
	curs_set(0);

	i32 congrats_row = MAIN_WINDOW_SIZE_Y(board->rows) + 4 + 3;
	i32 congrats_column = 0;

	board->timer.timer_finished = TRUE;
	pthread_join(board->timer.timer_tid, NULL);

	pthread_t reveal_bombs_thread;

	pthread_mutex_init(&board->go_mutex, NULL);
	pthread_create(&reveal_bombs_thread, NULL, reveal_bombs, (void*)board);
	pthread_mutex_lock(&board->go_mutex);

	mvprintw(congrats_row, congrats_column, "You Lost :(");			
	mvprintw(congrats_row + 1, congrats_column, "Press any button to reveal all the mines!");			
	refresh();	

	pthread_mutex_unlock(&board->go_mutex);
	pthread_join(reveal_bombs_thread, NULL);
	pthread_mutex_destroy(&board->go_mutex);

	move(congrats_row + 1, congrats_column);
	clrtoeol();

	refresh();

	//Flush input for when the user presses Y.
	flushinp();

	return;
}

b8 play_again(Board* board) {
	i32 row = MAIN_WINDOW_SIZE_Y(board->rows) + 4 + 6;
	i32 column = 0;

	move(row, column);

	printw("Do you want to play again(y/n)?");
	
	i8 c;

	while((c = getch())) {
		if(c == 'n')
			return FALSE;
		else if(c == 'y')
			return TRUE;
	}

	return FALSE;
}

//NOTE: Calculate bombs around x,y tile.
i32 bombs_around(Board* board, i32 x, i32 y) {
	i32 bombs = 0;

	for(i32 i = x - 1; i <= x + 1; ++i) {
		if(i < 0 || i >= board->rows) continue;

		for(i32 j = y - 1; j <= y + 1; ++j) {
			if(j < 0 || j >= board->cols || (i == x && j == y))
			       continue;	

			if(board->final_board[i][j] == '*')
				++bombs;
		}
	}

	return bombs;
}

void solve_board(Board* board) {
	i32 bombs = 0;

	for(i32 i = 0; i < board->rows; i++) {
		for(i32 j = 0; j < board->cols; j++) {
			if(board->final_board[i][j] == '*')
				continue;

			bombs = bombs_around(board, i, j);

			if(bombs == 0)
				continue;

			board->final_board[i][j] = '0' + bombs;
		}
	}

	return;
}

//NOTE: Recursively calculate '0' tiles until a number or a flag is found.
void expand_zero_tile(Board* board, i32 x, i32 y) {
	if(x == -1 || y == -1 || x == board->rows || y == board->cols || board->board[x][y] != '-')
		return;

	if(board->final_board[x][y] == '0') {
		board->board[x][y] = '0';
		--board->tiles_remaining;		

		expand_zero_tile(board, x + 1, y);
		expand_zero_tile(board, x + 1, y - 1);
		expand_zero_tile(board, x, y  + 1);
		expand_zero_tile(board, x - 1, y - 1);
		expand_zero_tile(board, x - 1, y);
		expand_zero_tile(board, x, y - 1);
		expand_zero_tile(board, x + 1, y + 1);
		expand_zero_tile(board, x - 1, y + 1);

		return;		
	}

	if(board->board[x][y] != 'F') {		
		--board->tiles_remaining;		
		board->board[x][y] = board->final_board[x][y];
	}

	return;
}

b8 reveal_tiles_around(Board* board, i32 x, i32 y) {
	for(i32 i = x - 1; i <= x + 1; ++i) {
		if(i < 0 || i >= board->rows ) continue;

		for(i32 j = y - 1; j <= y + 1; ++j) {
			if(j < 0 || j >= board->cols || (i == x && j == y) || board->board[i][j] != '-') 
			       continue;	
				
			if(board->board[i][j] == 'F')
				continue;

			if(board->final_board[i][j] == '*') {
				board->board[i][j] = board->final_board[i][j];
				return TRUE;
			}
			else if(board->final_board[i][j] == '0') {
				expand_zero_tile(board, i, j);
				continue;
			}

			board->board[i][j] = board->final_board[i][j];
			--board->tiles_remaining;		
		}
	}

	return FALSE;
}

i32 flags_around(Board* board, i32 x, i32 y) {
	i32 flags = 0;

	for(i32 i = x - 1; i <= x + 1; ++i) {
		if(i < 0 || i >= board->rows) continue;

		for(i32 j = y - 1; j <= y + 1; ++j) {
			if(j < 0 || j >= board->cols || (i == x && j == y))
			       continue;	

			if(board->board[i][j] == 'F')
				++flags;
		}
	}

	return flags;
}

void place_flag(Board* board, i32 row, i32 column) {
	if(!(board->board[row][column] == '-') && !(board->board[row][column] == 'F'))
		return;

	if(board->board[row][column] == 'F') {
		board->flags++;
		board->board[row][column] = '-';
	}
	else if(board->flags > 0) {	
		board->flags--;
		board->board[row][column] = 'F';
	}
	
	draw_flags(board);

	print_board(board, TRANSLATE_ROW(row), TRANSLATE_COL(column));

	return;
}

void* reveal_bombs(void* board_vp) {
	Board* board = (Board*)board_vp;

	b8 wants_to_exit = FALSE;

	for(i32 i = 0; i < board->rows; i++) {
		for(i32 j = 0; j < board->cols; j++) {
			if(board->final_board[i][j] == '*' && board->board[i][j] != 'F') {
				pthread_mutex_lock(&board->go_mutex);
				wmove(board->main_window, TRANSLATE_ROW(i), TRANSLATE_COL(j));
				wprintw(board->main_window, "*");
				wrefresh(board->main_window);
				pthread_mutex_unlock(&board->go_mutex);
				
				if(kbhit())	
					wants_to_exit = TRUE;

				if(!wants_to_exit) {
					i32 nano_to_milli = 300 * 1000;
					usleep(nano_to_milli);
				}

				continue;
			}
		}
	}

	pthread_exit(NULL);

	return NULL;
}

//TODO: ncurses uses only one cursor for rendering.
//that may cause problems in this multithreaded function
//maybe i should lock the thread before writing.
//(TEST FUNCTION).
void* timer_start(void* board_vp) {
	Board* board = (Board*)board_vp;

	board->timer.timer_finished = FALSE;
	board->timer.seconds_elapsed = 0;
	board->timer.minutes_elapsed = 0;

	i32 gran_ms = 100;
	i32 sec_test = 0;
	i32 sec_passed = 0;

	while(board->timer.timer_finished == FALSE) {
		if(sec_test == (gran_ms * 10) && sec_passed <= 999) {
			sec_test = 0;
			
			curs_set(0);

			wmove(board->main_window, 0, TIMER_POS_COL(board->cols));
			wprintw(board->main_window, "%03d", ++sec_passed);
			wmove(board->main_window, board->pos_r, board->pos_c);

			wrefresh(board->main_window);
			curs_set(1);
		}

		usleep(100000);
		sec_test += gran_ms;
	}
	
	board->timer.minutes_elapsed = sec_passed / 60; 
	board->timer.seconds_elapsed = sec_passed % 60; 

	return NULL;
}

void print_board(Board* board, i32 x, i32 y) {
	i8 c;	

	for(i32 i = 0; i < board->rows; ++i) {
		wmove(board->main_window, i + FIRST_ROW, FIRST_COL);

		for(i32 j = 0; j < board->cols; ++j) {
			c = board->board[i][j];
			wprintw(board->main_window, "%c ", c);	 
		}
	}

	wmove(board->main_window, x, y);
	wrefresh(board->main_window);

	return;
}

void draw_flags(Board* board) {
	wclear(board->flags_remaining);

	box(board->flags_remaining, 0, 0);
	mvwprintw(board->flags_remaining, 1, 1, " Flags Remaining: %d", board->flags);

	wrefresh(board->flags_remaining);

	return;
}

void draw_help(Board* board) {
	mvwprintw(board->help_window, 0, 1, "Move: W,A,S,D");
	mvwprintw(board->help_window, 1, 1, "Expand: E/Enter");
	mvwprintw(board->help_window, 2, 1, "Place Flags: F  Quit: P");

	wrefresh(board->help_window);

	return;
}

void get_name(Board* board, char* name) {
	mvprintw(MAIN_WINDOW_SIZE_Y(board->rows) + 3 + 6, 0, "You made it to the scoreboard, please type your 4 letter alias: ");
	
	for(i32 i = 0; i < SCB_MAX_NAME; ++i)
		name[i] = '\0';


	i32 x;
	i32 y;

	for(i32 i = 0; i < SCB_MAX_NAME; ++i) {
		u8 c;

		c = getch();

		if(c == 127) {
			if(i == 0) {
				--i;
				continue;
			}

			x = getcurx(stdscr);
			y = getcury(stdscr);

			mvaddch(y, x - 1, ' ');
			move(y, x - 1);

			i-=2;

			continue;
		}
		else if((c < 'A' || c > 'Z') && (c < 'a' || c > 'z')) {
			--i;

			continue;
		}

		printw("%c", c);
		name[i] = c;
	}

	noecho();
	curs_set(0);
		
	return;
}

void scoreboard_check(Board* board, i32 secs) {
	switch(board->difficulty) {
		case BEGGINER: {
			if(scoreboard_should_add(SC_BEGINNER_FILEPATH, secs) == FALSE)
				return;
	
			char name[SCB_MAX_NAME];		
			get_name(board, name);

			scoreboard_write(SC_BEGINNER_FILEPATH, name, secs);
			break;
		};
		case INTERMIDIATE: {
			if(scoreboard_should_add(SC_INTERMIDIATE_FILEPATH, secs) == FALSE)
				return;
	
			char name[SCB_MAX_NAME];		
			get_name(board, name);

			scoreboard_write(SC_INTERMIDIATE_FILEPATH, name, secs);
			break;
		};
		case EXPERT: {
			if(scoreboard_should_add(SC_EXPERT_FILEPATH, secs) == FALSE)
				return;
	
			char name[SCB_MAX_NAME];		
			get_name(board, name);

			scoreboard_write(SC_EXPERT_FILEPATH, name, secs);
			
			break;
	        };
		default:
			return;
	}

	return;
}

void cursor_move(WINDOW* window, i32 row, i32 column) {
        wmove(window, row, column);
        wrefresh(window);

	return;
}

//NOTE: This function is like a non-blocking getchar, it
//checks if a button was hit without blocking. 
i32 kbhit(void) {
	const int STDIN = 0;
	static bool initialized = false;

	if (!initialized) {
		struct termios term;
		tcgetattr(STDIN, &term);
		term.c_lflag &= ~ICANON;
		tcsetattr(STDIN, TCSANOW, &term);
		setbuf(stdin, NULL);
		initialized = true;
	}

	int bytesWaiting;
	ioctl(STDIN, FIONREAD, &bytesWaiting);

	return bytesWaiting;
}
