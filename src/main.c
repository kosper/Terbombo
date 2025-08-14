#include <game.h>

int main(int argc, char** argv) {
	(void)argc;
	(void)argv;

	Game bosnia_simulator = {0};

	game_init(&bosnia_simulator);
	game_run(&bosnia_simulator);
	game_shutdown(&bosnia_simulator);

	return 0;
}
