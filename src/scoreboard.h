#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include <game_defines.h>

#define SC_BEGINNER_FILEPATH "./Scores/scb_bg.bs"
#define SC_INTERMIDIATE_FILEPATH "./Scores/scb_in.bs"
#define SC_EXPERT_FILEPATH "./Scores/scb_ex.bs"

#define SCB_NUM_MAX 10
#define SCB_MAX_NAME 4

typedef struct Scoreboard {
	FILE* file_handle;
	i32 score_count;
	i32 scores[SCB_NUM_MAX];
	i8 names[SCB_NUM_MAX][SCB_MAX_NAME + 1];
} Scoreboard;

b8 scoreboard_read(Scoreboard* sc, const char* filepath);
b8 scoreboard_write(const char* filepath, char name[SCB_MAX_NAME], i32 secs);

void scoreboard_print(void);

//NOTE: Determines if a score should be added to the scoreboard, returns TRUE if yes.
b8 scoreboard_should_add(const char* filepath, i32 secs);

//NOTE: Serialized scoreboard to a file.
b8 dump_to_file(Scoreboard* sc, const char* filepath);

#endif
