#include <menu.h>
#include <scoreboard.h>

b8 scoreboard_read(Scoreboard* sc, const char* filepath) {
	sc->file_handle = fopen(filepath, "r+");
	sc->score_count = 0;

	//NOTE: If file does not exist, create one.
	if(sc->file_handle == NULL) {
		fclose(sc->file_handle);
		sc->file_handle = fopen(filepath, "w+");
		
		if(sc->file_handle == NULL)
			return FALSE;

		fwrite(&sc->score_count, sizeof(i32), 1, sc->file_handle);
		fclose(sc->file_handle);

		sc->file_handle = fopen(filepath, "r");

		if(sc->file_handle == NULL)
			return FALSE;
	}

	fread((void*)&sc->score_count, sizeof(i32), 1, sc->file_handle);	

	//NOTE: Read all names and scores to struct.
	for(i32 i = 0; i < sc->score_count; i++) {
		fread(sc->names[i], sizeof(i8), 4, sc->file_handle);
		sc->names[i][SCB_MAX_NAME] = '\0';
		fread(&sc->scores[i], sizeof(i32), 1, sc->file_handle);
	}

	fclose(sc->file_handle);

	return TRUE;
}

b8 scoreboard_write(const char* filepath, char name[SCB_MAX_NAME], i32 secs) {
	Scoreboard sc;
	scoreboard_read(&sc, filepath);

	//NOTE: If scoreboard is not full or if there are no entrys add score to the end.
	if((sc.score_count < SCB_NUM_MAX && sc.scores[sc.score_count - 1] <= secs) || sc.score_count == 0) {
		for(i32 i = 0; i < SCB_MAX_NAME; i++)
			sc.names[sc.score_count][i] = name[i];

		sc.scores[sc.score_count] = secs;
		sc.score_count++;

		dump_to_file(&sc, filepath);	

		return TRUE;
	}	

	i32 index = -1;

	//NOTE: If scoreboard is full find in wich position it should be added.
	for(i32 i = sc.score_count - 1; i >= 0; i--) {
		if(secs <= sc.scores[i])
			index = i;
	}

	//NOTE: This score should not be added.
	if(index == -1)
		return TRUE;

	//NOTE: Add score to the correct position.
	char temp_name[4];

	for(i32 j = 0; j < SCB_MAX_NAME; j++)
		temp_name[j] = sc.names[index][j];

	i32 temp_num = sc.scores[index]; 

	sc.scores[index] = secs;

	for(i32 j = 0; j < SCB_MAX_NAME; j++)
		sc.names[index][j] = name[j];

	if(sc.score_count < SCB_NUM_MAX)
		sc.score_count++;

	//NOTE: Swap names.
	for(i32 j = index + 1; j < sc.score_count; j++) {
		char prev_name[4];
		i32 prev_num;

		for(i32 k = 0; k < SCB_MAX_NAME; k++)
			prev_name[k] = sc.names[j][k];

		for(i32 k = 0; k < SCB_MAX_NAME; k++)
			sc.names[j][k] = temp_name[k];

		for(i32 k = 0; k < SCB_MAX_NAME; k++)
			temp_name[k] = prev_name[k];

		prev_num = sc.scores[j];
		sc.scores[j] = temp_num;
		temp_num = prev_num; 
	}

	dump_to_file(&sc, filepath);	

	return TRUE;
}

void scoreboard_print(void) {
	Scoreboard beg_sc;
	Scoreboard int_sc;
	Scoreboard exp_sc;

	scoreboard_read(&beg_sc, SC_BEGINNER_FILEPATH); 
	scoreboard_read(&int_sc, SC_INTERMIDIATE_FILEPATH);
	scoreboard_read(&exp_sc, SC_EXPERT_FILEPATH);

	logo_print();

	WINDOW* beg_win = newwin(12, 16, LOGO_ROW_CNT + 2, 0);
	WINDOW* int_win = newwin(12, 16, LOGO_ROW_CNT + 2, 18);
	WINDOW* exp_win = newwin(12, 16, LOGO_ROW_CNT + 2, 36);

	curs_set(0);
	refresh();

	box(beg_win, 0, 0);
	box(int_win, 0, 0);
	box(exp_win, 0, 0);

	mvwprintw(beg_win, 0, 4, "Beginner");
	mvwprintw(int_win, 0, 2, "Intermediate");
	mvwprintw(exp_win, 0, 5, "Expert");

	for(i32 i = 0; i < beg_sc.score_count; i++) {
		i32 mins = beg_sc.scores[i]/60;
		i32 secs = beg_sc.scores[i]%60;
		
		if(i < SCB_NUM_MAX - 1)
			mvwprintw(beg_win, i + 1, 1, "%d. %s %02d:%02d", i+1, beg_sc.names[i], mins, secs);
		else
			mvwprintw(beg_win, i + 1, 1, "%d %s %02d:%02d", i+1, beg_sc.names[i], mins, secs);
	}

	for(i32 i = 0; i < int_sc.score_count; i++) {
		i32 mins = int_sc.scores[i]/60;
		i32 secs = int_sc.scores[i]%60;

		if(i < SCB_NUM_MAX - 1)
			mvwprintw(int_win, i + 1, 1, "%d. %s %02d:%02d", i+1, int_sc.names[i], mins, secs);
		else
			mvwprintw(int_win, i + 1, 1, "%d %s %02d:%02d", i+1, int_sc.names[i], mins, secs);
	}

	for(i32 i = 0; i < exp_sc.score_count; i++) {
		i32 mins = exp_sc.scores[i]/60;
		i32 secs = exp_sc.scores[i]%60;

		if(i < SCB_NUM_MAX - 1)
			mvwprintw(exp_win, i + 1, 1, "%d. %s %02d:%02d", i+1, exp_sc.names[i], mins, secs);
		else
			mvwprintw(exp_win, i + 1, 1, "%d %s %02d:%02d", i+1, exp_sc.names[i], mins, secs);
	}

	wrefresh(int_win);
	wrefresh(beg_win);
	wrefresh(exp_win);

	mvprintw(20, 0, "Press any button to go back...");

	getch();

	wclear(beg_win);
	delwin(beg_win);
	
	wclear(int_win);
	delwin(int_win);

	wclear(exp_win);
	delwin(exp_win);

	return;
}

b8 scoreboard_should_add(const char* filepath, i32 secs) {
	Scoreboard sc;

	if(!scoreboard_read(&sc, filepath))
		return FALSE;

	if(sc.score_count < SCB_NUM_MAX)
		return TRUE;

	if(sc.scores[SCB_NUM_MAX - 1] >= secs)
		return TRUE;

	return FALSE;
}

b8 dump_to_file(Scoreboard* sc, const char* filepath) {
	FILE* fp = fopen(filepath, "w");

	fwrite(&sc->score_count, sizeof(i32), 1, fp);

	for(i32 i = 0; i < sc->score_count; i++) {
		fwrite(sc->names[i], sizeof(char), SCB_MAX_NAME, fp);
		fwrite(&sc->scores[i], sizeof(i32), 1, fp);
	}

	fclose(fp);

	return TRUE;
}
