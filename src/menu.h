#ifndef MENU_H
#define MENU_H

#include <game_defines.h>

#define LOGO_ROW_CNT 5

#define MIN_CUSTOM_ROWS 7
#define MIN_CUSTOM_COLS 7
#define MIN_BOMB_CNT  1

#define MAX_CUSTOM_ROWS 20
#define MAX_CUSTOM_COLS 30 
#define MAX_BOMB_CNT(rows, cols) (rows * cols - 1)

void menu_init(WINDOW** menu);

//NOTE: Returns TRUE if "play game" was selected.
b8 menu_run(WINDOW** menu);
void menu_shutdown(WINDOW* menu);

void menu_print(WINDOW* menu);
void logo_print(void);
void controls_print(void);
void settings_print(WINDOW* menu);

/*
 * NOTE: Accepts pointer to a WINDOW*, because it modifies ptr.
 * Returns TRUE if "exit game" option was selected.
 */
b8 get_settings(WINDOW** menu, game_settings* settings); 

//NOTE: Lets user select a menu option, returns the selected row.
i32 get_choice(WINDOW* menu, i32 row_cnt); 
void custom_game_config(game_settings* settings);

#endif
