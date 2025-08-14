#ifndef GAME_DEFINES_H
#define GAME_DEFINES_H

//NOTE: Definition bellow is for usleep use.
#define _DEFAULT_SOURCE

#include <stdint.h>
#include <ncurses.h>

#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE 
	#define FALSE 0
#endif

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;
typedef float f32;

typedef i8 b8;

typedef enum GAME_DIFFICULTY {
	BEGGINER = 0x00,
	INTERMIDIATE = 0x01,
	EXPERT = 0x02,
	CUSTOM = 0x03	
} GAME_DIFFICULTY;

typedef struct game_settings {
	i32 rows, cols;
	i32 bomb_cnt;
	i32 flags;

	GAME_DIFFICULTY difficulty;
} game_settings;

#endif
